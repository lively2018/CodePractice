
#include <stdio.h>
#include <string.h>

#include "nexus_gpio.h"
#include "nexus_pwm_init.h"
#include "nexus_pwm.h"
#include <pthread.h>

#include "dsFPDSettings.h"
#include "mfr_fpd_hmx.h"
#ifdef FPD_DEBUG
#include "dshallogger.h"
#else
#define DEBUG(...)
#define ERROR(...)
#define WARN(...)
#define INFO(...)
#endif

typedef enum{
    LED_OFF = 0,
    LED_ON,
    LED_BLINK,
} LED_STATUS_e;

typedef struct{
	unsigned int uSleepTime;
	unsigned int uCount;
}FPD_LED_BlinkInfo_t;

typedef struct{
	FPD_INDICATOR eIndicator;	
	LED_STATUS_e eStatus;
	FPD_COLOR FPDColor;
	FPD_BRIGHTNESS eBrightness;
	unsigned int uBlinkPeriod;
	bool	bOpenDrain;
	NEXUS_PwmFreqModeType eFreqMode;
	unsigned int uPWMIndex;
	NEXUS_PwmChannelHandle PWMChannelHandle;	
	NEXUS_GpioHandle GpioHandle;	
}FPD_LED_StateInfo_t;

#define FPD_LED_BLINK_CHECK_MS_TIME	100 // 100 mill sec
#define FPD_LED_BLINK_MS				1000 
#define FPD_LED_STATEINFO_MAX	3
#define FPD_LED_PWM_INTERVAL_MAX 0xFF
#define FPD_LED_PWM_INTERVAL_MIN 0x00

static FPD_LED_StateInfo_t FPD_LED_StateInfoTable[FPD_LED_STATEINFO_MAX] = {

	{ FPD_INDICATOR_POWER,	LED_OFF,	FPD_COLOR_RED, 0,	 0, false,	NEXUS_PwmFreqModeType_eConstant,	2, NULL, NULL},	 
	{ FPD_INDICATOR_WIFI,	LED_OFF,	FPD_COLOR_RED,	0,	 0, false,	NEXUS_PwmFreqModeType_eConstant,	3,	 NULL, NULL},
	{ FPD_INDICATOR_NET,		LED_OFF,	FPD_COLOR_RED,	0,	 0, false,	NEXUS_PwmFreqModeType_eConstant,	1,	 NULL, NULL},
};
static pthread_t fpd_blink_threadID; // Use for fpd led blink
static void* fpd_blink_thread(void *arg);


FPD_API_RESULT FPD_Init(void)
{

	NEXUS_GpioSettings gpioSettings;
	NEXUS_GpioType gpioType ;
	unsigned short pinNumber ;
	NEXUS_PwmChannelSettings	PwmChnDefSettings;
	NEXUS_Error errCode;
	int i;
	NEXUS_PwmChannelHandle PwmChannelHandle;
	
	int rc;
	NEXUS_GpioHandle gpioHandle;
	DEBUG("[%s][%d] Initialize GPIO handle",__func__,__LINE__);

	
	gpioHandle = NULL;
	gpioType = NEXUS_GpioType_eAonStandard;
	NEXUS_Gpio_GetDefaultSettings(gpioType, &gpioSettings);
	gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
	gpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
	pinNumber = 1;
	gpioHandle = NEXUS_Gpio_Open(gpioType, pinNumber, &gpioSettings);
	if(gpioHandle != NULL)
	{
		INFO("[%s][%d]GPIO OPEN SUCCESS!!, gpio_id(%d) ",__func__,__LINE__,pinNumber);
	}
	else
	{
	 	ERROR("[%s][%d]GPIO OPEN FAIL!!, gpio_id(%d)",__func__,__LINE__,pinNumber);
		return FPD_API_RESULT_FAILED;
	}
	FPD_LED_StateInfoTable[FPD_INDICATOR_POWER].GpioHandle = gpioHandle;
	/* PWN init */
	/* Initialize Channel handle */
	for(i=0; i<FPD_LED_STATEINFO_MAX; i++)
	{
		PwmChannelHandle = NULL;
		NEXUS_Pwm_GetDefaultChannelSettings(&PwmChnDefSettings);
		PwmChnDefSettings.openDrain = FPD_LED_StateInfoTable[i].bOpenDrain;
		PwmChnDefSettings.eFreqMode = FPD_LED_StateInfoTable[i].eFreqMode;
		PwmChannelHandle = NEXUS_Pwm_OpenChannel(FPD_LED_StateInfoTable[i].uPWMIndex,&PwmChnDefSettings);
		FPD_LED_StateInfoTable[i].PWMChannelHandle  = PwmChannelHandle;
		errCode = NEXUS_Pwm_SetControlWord(PwmChannelHandle, 0x3CF0);
		if(errCode != NEXUS_SUCCESS)
		{
			ERROR("NEXUS_Pwm_SetControlWord(): ErrCode(0x%08x) \n", errCode);
			return FPD_API_RESULT_FAILED;
		}

		errCode = NEXUS_Pwm_SetPeriodInterval(PwmChannelHandle, 0xfe);
		if(errCode != NEXUS_SUCCESS)
		{
			ERROR("NEXUS_Pwm_SetPeriodInterval(): rc(0x%08x) \n", errCode);
			return FPD_API_RESULT_FAILED;
		}

		errCode = NEXUS_Pwm_SetOnInterval(PwmChannelHandle, 0);
		if(errCode != NEXUS_SUCCESS)
		{
			ERROR("NEXUS_Pwm_SetOnInterval(): ErrCode(0x%08x) \n", errCode);
			return FPD_API_RESULT_FAILED;
		}

		errCode= NEXUS_Pwm_Start(PwmChannelHandle);
		if(errCode != NEXUS_SUCCESS)
		{
			ERROR("%s->NEXUS_Pwm_Start(): eNexusError(0x%08x) Line(%d), %s\n", __FUNCTION__, errCode, __LINE__, __FILE__);
			return FPD_API_RESULT_FAILED;
		}
		else
		{
			INFO("%s->NEXUS_Pwm_Start(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		}
		
	}
	  rc = pthread_create (&fpd_blink_threadID, NULL,fpd_blink_thread, (void *)&(FPD_LED_StateInfoTable[0]));
    if (rc)
   		 ERROR("FPD : Failed to Ceate FPD blink Thread ....\r\n");


   return FPD_API_RESULT_SUCCESS;

}

FPD_API_RESULT FPD_DeInit(void)
{

	int rc;
	int i;
	NEXUS_Error ErrCode;
	NEXUS_PwmChannelHandle pwmChannelHandle;
	NEXUS_GpioHandle gpioHandle;
	
	rc = pthread_join(fpd_blink_threadID, NULL);
	if(rc)
	{
		ERROR("FPD : Failed to terminate FPD blink Thread ....\r\n");
	}

	for(i=0; i< FPD_LED_STATEINFO_MAX; i++)
	{
		pwmChannelHandle =  FPD_LED_StateInfoTable[i].PWMChannelHandle;
		if(NULL != pwmChannelHandle)
		{
			ErrCode = NEXUS_Pwm_Stop(pwmChannelHandle);
			if(ErrCode == NEXUS_SUCCESS)
			{
			 	INFO("[%s][%d]PWM Stop SUCCESS!!, fpd_indicator(%d) ",__func__,__LINE__,i);

			}
			else
			{
			 	ERROR("[%s][%d]WM StopFAIL!!, fpd_indicator(%d)",__func__,__LINE__,i);
				return FPD_API_RESULT_FAILED;
			}
			NEXUS_Pwm_CloseChannel(pwmChannelHandle);
		}
		gpioHandle =  FPD_LED_StateInfoTable[i].GpioHandle;
		if(NULL != gpioHandle)
		{
			NEXUS_Gpio_Close(gpioHandle);
		}
	}
	return FPD_API_RESULT_SUCCESS;
 
}

FPD_API_RESULT FPD_SetBlink (FPD_INDICATOR eIndicator, unsigned int uBlinkDuration, unsigned int uBlinkIterations)
{

	unsigned int uPeriod;

	if (eIndicator > FPD_INDICATOR_WIFI)
  {
    ERROR("%s - GetState for indicator %d NOT SUPPORTED\n", __FUNCTION__, eIndicator);
    return FPD_API_RESULT_INVALID_PARAM;
  }

	if((uBlinkDuration == 0)|| (uBlinkIterations == 0))
	{
		
		return FPD_API_RESULT_INVALID_PARAM;
	}

	
	DEBUG("[%s][%d] Indicator:%d uBlinkDuration: %d uBlinkIteration:%d",__func__,__LINE__,eIndicator,uBlinkIterations,uBlinkDuration);
	uPeriod = (uBlinkDuration*FPD_LED_BLINK_MS)/(uBlinkIterations*2*FPD_LED_BLINK_CHECK_MS_TIME);
	FPD_LED_StateInfoTable[eIndicator].eStatus = LED_BLINK;
	FPD_LED_StateInfoTable[eIndicator].uBlinkPeriod = uPeriod;
	 
	return FPD_API_RESULT_SUCCESS;
}

FPD_API_RESULT FPD_SetState(FPD_INDICATOR eIndicator, bool state)
{
	NEXUS_Error ErrCode ;
	NEXUS_GpioSettings gpioSettings;
	NEXUS_GpioHandle gpioHandle;
	NEXUS_GpioValue		gpio_value;
	NEXUS_PwmChannelHandle pwmChannelHandle;
	unsigned char duty;
	FPD_COLOR ledColor;

	DEBUG("[%s][%d]!!, fpd_indicator(%d) is set to %d \r\n",__func__,__LINE__,eIndicator, state);
	if (eIndicator > FPD_INDICATOR_WIFI)
  {
    ERROR("%s - GetState for indicator %d NOT SUPPORTED\n", __FUNCTION__, eIndicator);
    return FPD_API_RESULT_INVALID_PARAM;
  }
	
	gpioHandle = NULL;
	pwmChannelHandle = FPD_LED_StateInfoTable[eIndicator].PWMChannelHandle;
	ledColor = FPD_LED_StateInfoTable[eIndicator].FPDColor;
	FPD_LED_StateInfoTable[eIndicator].uBlinkPeriod = 0;
	if(state)
	{
		FPD_LED_StateInfoTable[eIndicator].eStatus = LED_ON;
	}
	else
	{
		FPD_LED_StateInfoTable[eIndicator].eStatus = LED_OFF;
	}

	switch (eIndicator)
	{
		case FPD_INDICATOR_POWER:
			
			gpioHandle = FPD_LED_StateInfoTable[eIndicator].GpioHandle;
	
			if( ledColor == FPD_COLOR_ORANGE)
			{
				if(state)
				{
					duty = FPD_LED_PWM_INTERVAL_MAX;	
					gpio_value = NEXUS_GpioValue_eHigh;
				}
				else // off
				{
					duty = FPD_LED_PWM_INTERVAL_MIN;
					gpio_value = NEXUS_GpioValue_eLow;
				}
			}
			else if(ledColor == FPD_COLOR_RED)
			{
				duty = FPD_LED_PWM_INTERVAL_MIN;	
				if(state)
				{
					gpio_value = NEXUS_GpioValue_eHigh;
				}
				else
				{
					gpio_value = NEXUS_GpioValue_eLow;
				}
			}
			else if(ledColor == FPD_COLOR_YELLOW)
			{
				gpio_value = NEXUS_GpioValue_eLow;
				if(state)
				{
					duty = FPD_LED_PWM_INTERVAL_MAX/4;	
				}
				else 
				{
					duty = FPD_LED_PWM_INTERVAL_MIN;
				}				
			}		
			else if(ledColor == FPD_COLOR_GREEN)
			{
				gpio_value = NEXUS_GpioValue_eLow;
				if(state)
				{
					duty = FPD_LED_PWM_INTERVAL_MAX;	
				}
				else 
				{
					duty = FPD_LED_PWM_INTERVAL_MIN;
				}				
			}		
			else
			{
				return FPD_API_RESULT_FAILED;
			}
			break;
		case FPD_INDICATOR_WIFI:
		case FPD_INDICATOR_NET:	
			if(ledColor == FPD_COLOR_RED)
			{

				if(state)
				{
					duty = FPD_LED_PWM_INTERVAL_MAX;
				}
				else //off
				{
					duty = FPD_LED_PWM_INTERVAL_MIN;
				}				
			}
			else
			{
				return FPD_API_RESULT_FAILED;
			}
			break;

		default:
			break;

	}
	if(gpioHandle != NULL)
	{
		NEXUS_Gpio_GetSettings(gpioHandle, &gpioSettings);
		gpioSettings.value = gpio_value;
		ErrCode = NEXUS_Gpio_SetSettings(gpioHandle, &gpioSettings);
		if(ErrCode == NEXUS_SUCCESS)
		{
			INFO("[%s][%d]GPIO WRITE SUCCESS!!, fpd_indicator(%d) ",__func__,__LINE__,eIndicator);

		}
		else
		{
 			ERROR("[%s][%d]GPIO WRITE FAIL!!, fpd_indicator(%d)",__func__,__LINE__,eIndicator);
			return FPD_API_RESULT_FAILED;
		}
	}
	if(pwmChannelHandle != NULL)
	{
		ErrCode = NEXUS_Pwm_SetOnInterval(pwmChannelHandle, duty);
		if(ErrCode == NEXUS_SUCCESS)
		{
			INFO("[%s][%d]PWM Channel Set Interval SUCCESS!!, fpd_indicator(%d) ",__func__,__LINE__,eIndicator);

		}
		else
		{
 			ERROR("[%s][%d]Channel Set Interval FAIL!!, fpd_indicator(%d)",__func__,__LINE__,eIndicator);
			return FPD_API_RESULT_FAILED;
		}
	}
	 return FPD_API_RESULT_SUCCESS;
  
 }

FPD_API_RESULT FPD_GetState(FPD_INDICATOR eIndicator, bool *state)
{
	if (eIndicator > FPD_INDICATOR_WIFI)
  {
    ERROR("%s - GetState for indicator %d NOT SUPPORTED\n", __FUNCTION__, eIndicator);
    return FPD_API_RESULT_INVALID_PARAM;
  }
  
	*state = FPD_LED_StateInfoTable[eIndicator].eStatus;
	
	return FPD_API_RESULT_SUCCESS;
}

FPD_API_RESULT FPD_SetBrightness(FPD_INDICATOR eIndicator, FPD_BRIGHTNESS eBrightness)
{

	DEBUG("[%s][%d]!!, fpd_indicator(%d) brightness: %d ",__func__,__LINE__,eIndicator, eBrightness);
	if (eIndicator > FPD_INDICATOR_WIFI)
  {
    ERROR("%s - GetState for indicator %d NOT SUPPORTED\n", __FUNCTION__, eIndicator);
    return FPD_API_RESULT_INVALID_PARAM;
  }
  
	if((eBrightness < 0) || (eBrightness > FPD_BRIGHTNESS_MAX))
	{
		return FPD_API_RESULT_INVALID_PARAM;
	}

	FPD_LED_StateInfoTable[eIndicator].eBrightness = eBrightness;

	return FPD_API_RESULT_SUCCESS;

}

FPD_API_RESULT FPD_GetBrightness(FPD_INDICATOR eIndicator, FPD_BRIGHTNESS *pBrightness)
{
 	if (eIndicator > FPD_INDICATOR_WIFI)
  {
    ERROR("%s - GetState for indicator %d NOT SUPPORTED\n", __FUNCTION__, eIndicator);
    return FPD_API_RESULT_INVALID_PARAM;
  }
  
	if (NULL == pBrightness)
	{
		return FPD_API_RESULT_INVALID_PARAM;
	}
	*pBrightness = FPD_LED_StateInfoTable[eIndicator].eBrightness;

	return FPD_API_RESULT_SUCCESS;
}

FPD_API_RESULT FPD_SetColor(FPD_INDICATOR eIndicator, FPD_COLOR eColor)
{
	DEBUG("[%s][%d] Indicator[%d] is set to %d color",__func__,__LINE__,eIndicator, eColor);

	if (eIndicator > FPD_INDICATOR_WIFI)
  {
    ERROR("%s - GetState for indicator %d NOT SUPPORTED\n", __FUNCTION__, eIndicator);
    return FPD_API_RESULT_INVALID_PARAM;
  }

	if((eIndicator == FPD_INDICATOR_POWER) && (eColor != FPD_COLOR_RED)&&(eColor != FPD_COLOR_ORANGE)
		&&(eColor != FPD_COLOR_YELLOW)&& (eColor != FPD_COLOR_GREEN))
	{
		return FPD_API_RESULT_FAILED;
	}
	if((eIndicator == FPD_INDICATOR_WIFI) && (eColor != FPD_COLOR_RED))
	{
		return FPD_API_RESULT_FAILED;
	}
	if((eIndicator == FPD_INDICATOR_NET) && (eColor != FPD_COLOR_RED))
	{
		return FPD_API_RESULT_FAILED;
	}
	FPD_LED_StateInfoTable[eIndicator].FPDColor = eColor;
	
	return FPD_API_RESULT_SUCCESS;
}

FPD_API_RESULT FPD_RegisterKeyCallback(FPD_CALLBACK pCallbackFunction, char *pCookie  )
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  return ret;
}

FPD_API_RESULT FPD_UnRegisterKeyCallback(FPD_CALLBACK pCallbackFunction)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  return ret;
}


FPD_API_RESULT FPD_clockDisplay (int nEnable)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  return ret;
}

FPD_API_RESULT FPD_SetSoftKeyText(FPD_KEY eSoftKey, char *pszLabel)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  return ret;
}

FPD_API_RESULT FPD_SetText(char * pszChars)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  return ret;
}

FPD_API_RESULT FPD_GetText(FPD_STR * ppszChars)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  return ret;
}

FPD_API_RESULT FPD_SetChars(int nCharacters, char * paChars)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  return ret;
}

FPD_API_RESULT FPD_SetParagraph (int nLines, char * paszLines[])
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  return ret;
}

FPD_API_RESULT FPD_SetTime (FPD_TIME eTime, int nHours, int nMinutes)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  return ret;
}

FPD_API_RESULT FPD_SetScroll(int nScrollHoldOnDur, int nHorzScrollIterations, int nVertScrollIterations)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  return ret;
}

static void * fpd_blink_thread(void *arg)
{
	
	unsigned int uSleepTime[FPD_LED_STATEINFO_MAX]={0,0,0};	
	FPD_LED_StateInfo_t *param = (FPD_LED_StateInfo_t *) arg;	
	int i;
	NEXUS_GpioSettings gpioSettings;
	NEXUS_GpioHandle gpioHandle;
	int duty;
	NEXUS_Error ErrCode;
	NEXUS_PwmChannelHandle pwmChannelHandle;
	
	if (arg == NULL)
	{
      INFO("[%s][%d]ERROR in thread function argument \r\n",__func__,__LINE__);
	}
	while(1)
	{
		usleep(FPD_LED_BLINK_CHECK_MS_TIME*FPD_LED_BLINK_MS); // every 100 msec
		

		for(i=0; i<FPD_LED_STATEINFO_MAX; i++)			
		{
			
			if(param[i].eStatus == LED_BLINK)
			{
				if(uSleepTime[i] > 0)
				{
					uSleepTime[i]--;					
				}
				else
				{
					uSleepTime[i] = param[i].uBlinkPeriod;					
					
					if(i == dsFPD_INDICATOR_POWER)
					{
						gpioHandle = param[dsFPD_INDICATOR_POWER].GpioHandle;
						NEXUS_Gpio_GetSettings(gpioHandle, &gpioSettings);
						pwmChannelHandle = param[dsFPD_INDICATOR_POWER].PWMChannelHandle;
						if(gpioSettings.value  == NEXUS_GpioValue_eHigh)  // OFF
						{
							gpioSettings.value = NEXUS_GpioValue_eLow;
							duty = FPD_LED_PWM_INTERVAL_MIN;
						}
						else//  ON
						{
							
							switch (param[FPD_INDICATOR_POWER].FPDColor)
							{
								case FPD_COLOR_ORANGE:							
									duty = FPD_LED_PWM_INTERVAL_MAX;
									gpioSettings.value = NEXUS_GpioValue_eHigh;
									break;
								case FPD_COLOR_RED:
									duty = FPD_LED_PWM_INTERVAL_MIN;
									gpioSettings.value = NEXUS_GpioValue_eHigh;
									break;
								case FPD_COLOR_YELLOW:
									duty = FPD_LED_PWM_INTERVAL_MAX/4;
									gpioSettings.value = NEXUS_GpioValue_eHigh;
									break;
								case FPD_COLOR_GREEN:
									gpioSettings.value = NEXUS_GpioValue_eLow;
									duty = FPD_LED_PWM_INTERVAL_MAX;
									break;
								default:
									duty = FPD_LED_PWM_INTERVAL_MIN;
									gpioSettings.value = NEXUS_GpioValue_eLow;
									break;
							}

						
						}
						ErrCode = NEXUS_Gpio_SetSettings(gpioHandle, &gpioSettings);
						if(ErrCode == NEXUS_SUCCESS)
						{
							//INFO("[%s][%d]GPIO WRITE SUCCESS!!, fpd_indicator(%d) ",__func__,__LINE__,i);

						}
						else
						{
				 			ERROR("[%s][%d]GPIO WRITE FAIL!!, fpd_indicator(%d)",__func__,__LINE__,i);
							continue;
						}

					}
					else
					{
						if(duty != 0) // Off
						{						
							duty = FPD_LED_PWM_INTERVAL_MIN;
						}
						else //  ON
						{
							
							if(FPD_COLOR_RED == param[i].FPDColor) 
							{
								duty = FPD_LED_PWM_INTERVAL_MAX;
							}
							else
							{
								duty = FPD_LED_PWM_INTERVAL_MIN;
							}
							
						
						}
			
					}
					ErrCode = NEXUS_Pwm_SetOnInterval(pwmChannelHandle, duty);
					if(ErrCode == NEXUS_SUCCESS)
					{
						//INFO("[%s][%d]PWM Channel Set Interval SUCCESS!!, fpd_indicator(%d) ",__func__,__LINE__,i);

					}
					else
					{
			 			ERROR("[%s][%d]Channel Set Interval FAIL!!, fpd_indicator(%d)",__func__,__LINE__,i);
						continue;
					}
				
				}
			}
		}

	}
}



