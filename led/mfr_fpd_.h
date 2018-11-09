
#ifndef MFR_FPD_LGI_H_
#define MFR_FPD_LGI_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef enum _FPD_API_RESULT
{
  FPD_API_RESULT_SUCCESS = 0,
  FPD_API_RESULT_FAILED,
  FPD_API_RESULT_NULL_PARAM,
  FPD_API_RESULT_INVALID_PARAM,
  FPD_API_RESULT_NOT_INITIALIZED
}FPD_API_RESULT;

typedef enum _FPD_COLOR
{
  FPD_COLOR_BLUE    = (0x01<<0),  /* 1 */
  FPD_COLOR_GREEN   = (0x01<<1),  /* 2 */
  FPD_COLOR_RED     = (0x01<<2),  /* 4 */
  FPD_COLOR_YELLOW  = (0x01<<3),  /* 8 */
  FPD_COLOR_ORANGE  = (0x01<<4),  /* 16 */
  FPD_COLOR_WHITE   = (0x01<<5)   /* 32 */
}FPD_COLOR;

typedef int FPD_BRIGHTNESS;
#define FPD_BRIGHTNESS_MAX 100


#define FPD_NUM_INDICATORS               3

typedef enum _FPD_INDICATOR {
  FPD_INDICATOR_POWER                    = (0x01<<0),
  FPD_INDICATOR_NETWORK                  = (0x01<<1),
  FPD_INDICATOR_WIFI                     = (0x01<<2)
} FPD_INDICATOR;


FPD_API_RESULT FPD_Init(void);
FPD_API_RESULT FPD_DeInit(void);
FPD_API_RESULT FPD_SetBlink (FPD_INDICATOR eIndicator, unsigned int uBlinkDuration, unsigned int uBlinkIterations);
FPD_API_RESULT FPD_SetState(FPD_INDICATOR eIndicator, bool state);
FPD_API_RESULT FPD_GetState(FPD_INDICATOR eIndicator, bool *state);
FPD_API_RESULT FPD_SetBrightness(FPD_INDICATOR eIndicator, FPD_BRIGHTNESS eBrightness);
FPD_API_RESULT FPD_GetBrightness(FPD_INDICATOR eIndicator, FPD_BRIGHTNESS *pBrightness);
FPD_API_RESULT FPD_SetColor(FPD_INDICATOR eIndicator, FPD_COLOR eColor);
FPD_API_RESULT FPD_SetText(char * pszChars);
FPD_API_RESULT FPD_GetText(FPD_STR * ppszChars);
FPD_API_RESULT FPD_SetChars(int nCharacters, char * paChars);
FPD_API_RESULT FPD_SetParagraph (int nLines, char * paszLines[]);
FPD_API_RESULT FPD_SetTime (FPD_TIME eTime, int nHours, int nMinutes);
FPD_API_RESULT FPD_SetScroll(int nScrollHoldOnDur, int nHorzScrollIterations, int nVertScrollIterations);
FPD_API_RESULT FPD_clockDisplay (int nEnable);
FPD_API_RESULT FPD_SetSoftKeyText(FPD_KEY eSoftKey, char *pszLabel);
FPD_API_RESULT FPD_RegisterKeyCallback(FPD_CALLBACK pCallbackFunction, char *pCookie  );
FPD_API_RESULT FPD_UnRegisterKeyCallback(FPD_CALLBACK pCallbackFunction);

#ifdef __cplusplus
}
#endif

#endif