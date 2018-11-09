/*
 * mfr_fpd_lgi.c
 *
 *  Created on: Dec 6, 2016
 *      Author: L.Plewa
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "mfr_fpd_lgi.h"
#include "bstd.h"
#include "nexus_platform.h"
#include <nexus_types.h>
#include <nexus_gpio.h>
#include <nexus_pwm.h>
#ifdef FPD_DEBUG
#include "dshallogger.h"
#else
#define DEBUG(...)
#define ERROR(...)
#define WARN(...)
#define INFO(...)
#endif

/*
 * Platform specific definitions
 */
#define ABPWMGPIO   89
#define LEDAREDGPIO 90
#define LEDAGRNGPIO 91
#define LEDBREDGPIO 95
#define LEDBGRNGPIO 96

#define GPIO_PINMUX_PWM_AB 2

#define EOS_LED_I2C_CHANNEL  0x02
#define LP5562_CHIP_ADDRESS  0x30
#define PWM_DATA_LENGTH      0x01
#define I2C_RETRY_COUNT      0x05

#define LP5562_REG_ENABLE    0x00
#define LP5562_MASTER_ENABLE 0x40    /* Chip master enable */

/* CONFIG Register 08h */
#define LP5562_REG_CONFIG    0x08
#define LP5562_CLK_INT       0x01    /* Internal clock */

#define LP5562_REG_OP_MODE   0x01
#define LP5562_CMD_DIRECT    0x3F

#define LP5562_REG_R_PWM     0x04
#define LP5562_REG_G_PWM     0x03
#define LP5562_REG_B_PWM     0x02
#define LP5562_REG_W_PWM     0x0E

/* LEDMAP Register 70h */
#define LP5562_REG_ENG_SEL   0x70
#define LP5562_ENG_SEL_PWM   0x00

#define LED_MAX_PWM_VALUE    0xFF
#define LED_MIN_PWM_VALUE    0x00

#define BLINK_CHECK_US   10000
#define BLINK_CHECK_MS   (BLINK_CHECK_US/1000)

typedef struct PWMType
{
  bool     bPWM;
  bool     bOn;
  uint16_t Intensity;
}PowerLED_State;

typedef enum {
    POWER = 0,
    LED_A_RED,
    LED_A_GREEN,
    LED_B_RED,
    LED_B_GREEN,
    LED_FUNCTION_MAX,
    LED_FUNCTION_UNKNOWN = 0xFFFF
} LEDfunction_t;

typedef struct {
  uint32_t ColorPercentage[3];
  uint32_t Intensity;
  bool On;
} LEDcurrentStateParamsStruct_t;

typedef struct ledBlinkParams {
  int interval;
  int duration;   // interval * iterations
  int iterations;
  FPD_INDICATOR indicator;
  bool last_state;
}ledBlinkParams_t;


/*
 * Implementation variables
 */
static NEXUS_GpioHandle       leda_r;
static NEXUS_GpioHandle       leda_g;
static NEXUS_GpioHandle       ledb_r;
static NEXUS_GpioHandle       ledb_g;

static NEXUS_GpioSettings     gpioSettingsleda_r;
static NEXUS_GpioSettings     gpioSettingsleda_g;
static NEXUS_GpioSettings     gpioSettingsledb_r;
static NEXUS_GpioSettings     gpioSettingsledb_g;
static NEXUS_PwmChannelHandle hPwmChannel = NULL;


static PowerLED_State  PowerLEDRed;
static PowerLED_State  PowerLEDGreen;
static PowerLED_State  PowerLEDBlue;
static PowerLED_State  AB_LEDs;

static NEXUS_I2cHandle hI2cHandle;
static pthread_mutex_t mutex;
static pthread_t *blinking_thread;

static pthread_mutex_t  blink_mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t            blink_change_semaphore;
static bool             blink_thread_active = 0;
static ledBlinkParams_t blinking_status[FPD_NUM_INDICATORS];

static LEDcurrentStateParamsStruct_t currentParams[LED_FUNCTION_MAX];


static inline void lp5562_wait_opmode_done(void)
{
  /* operation mode change needs to be longer than 153 us */
  usleep(160);
}

static void mfrfpd_Configure_PWM(void);
static FPD_API_RESULT mfrfpd_Write_I2c(uint8_t I2csubAddr, uint8_t I2CData);
static FPD_API_RESULT mfrfpd_LedSet(LEDfunction_t LED, bool bOn);
static FPD_API_RESULT mfrfpd_Getintensity(LEDfunction_t LED, uint32_t *intensity);
static FPD_API_RESULT mfrfpd_Setintensity(LEDfunction_t LED, uint32_t intensity);
static FPD_API_RESULT mfrfpd_SetColor(LEDfunction_t LED, uint32_t red, uint32_t green, uint32_t blue);
static bool mfrfpd_indicatorState(FPD_INDICATOR eIndicator);
static void mfrfpd_StopBlink(FPD_INDICATOR eIndicator, bool bOn);
static FPD_API_RESULT mfrfpd_IndicatorSetState(FPD_INDICATOR eIndicator, bool state);
void *mfrfpd_blinking_thread(void *param);

/*
 * API implementation
 */
FPD_API_RESULT FPD_Init(void)
{
  NEXUS_Error               rc;
  FPD_API_RESULT            ret = FPD_API_RESULT_FAILED;
  FPD_API_RESULT            Rtn = FPD_API_RESULT_FAILED;
  NEXUS_I2cSettings         settings;
  NEXUS_PwmChannelSettings  PwmChannelSettings;
  uint32_t                  chan = EOS_LED_I2C_CHANNEL;
  uint32_t                  pinNumber;
  uint32_t                  u32TypeAndPin;
  uint32_t                  u32Addr;
  uint32_t                  u32Mask;
  uint32_t                  u32Shift;
  uint32_t                  u32Val;
  int                       i = 0;

  pthread_mutex_init(&mutex, NULL);

  u32TypeAndPin = (NEXUS_GpioType_eStandard << 16) | ABPWMGPIO;
  rc = NEXUS_Gpio_GetPinMux(u32TypeAndPin, &u32Addr, &u32Mask, &u32Shift);
  if (rc != BERR_SUCCESS)
  {
    return ret;
  }

  /* Test mux reg for PWM configuration */
  NEXUS_Platform_ReadRegister(u32Addr, &u32Val);
  if (((u32Val & u32Mask) >> u32Shift) != GPIO_PINMUX_PWM_AB)
  {
    /* Not configure for PWM operation...Reconfigure for PWM */
    DEBUG("%s - Configure_PWM: pin %d (POWER RED) not configured for PWM\r\n", __FUNCTION__, (int)ABPWMGPIO);
    u32Val &= 0xffffffff ^ u32Mask;
    u32Val |= (GPIO_PINMUX_PWM_AB << u32Shift) & u32Mask;
    NEXUS_Platform_WriteRegister(u32Addr, u32Val);
  }

  NEXUS_Pwm_GetDefaultChannelSettings(&PwmChannelSettings);
  PwmChannelSettings.eFreqMode = NEXUS_PwmFreqModeType_eConstant;
  hPwmChannel = NEXUS_Pwm_OpenChannel(2, &PwmChannelSettings);
  if (hPwmChannel == NULL)
  {
    ERROR("%s - NEXUS_Pwm_OpenChannel returned NULL channel handle!!!\n", __FUNCTION__);
    return ret;
  }

  AB_LEDs.Intensity = LED_MIN_PWM_VALUE;
  AB_LEDs.bOn    = true;
  AB_LEDs.bPWM   = true;
  NEXUS_Pwm_SetPeriodInterval(hPwmChannel, LED_MAX_PWM_VALUE-1);
  NEXUS_Pwm_SetControlWord(hPwmChannel,0x80);                     /* Configure for 206 Hz */
  NEXUS_Pwm_SetOnInterval(hPwmChannel,LED_MAX_PWM_VALUE - AB_LEDs.Intensity);

  /*Led A Red*/
  pinNumber = LEDAREDGPIO;
  NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &gpioSettingsleda_r);
  gpioSettingsleda_r.mode = NEXUS_GpioMode_eOutputPushPull;
  gpioSettingsleda_r.interruptMode = NEXUS_GpioInterrupt_eDisabled;
  gpioSettingsleda_r.value = NEXUS_GpioValue_eLow;
  leda_r = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, pinNumber, &gpioSettingsleda_r);
  NEXUS_Gpio_SetSettings(leda_r, &gpioSettingsleda_r);

  /*Led A Green*/
  pinNumber = LEDAGRNGPIO;
  NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &gpioSettingsleda_g);
  gpioSettingsleda_g.mode = NEXUS_GpioMode_eOutputPushPull;
  gpioSettingsleda_g.interruptMode = NEXUS_GpioInterrupt_eDisabled;
  gpioSettingsleda_g.value = NEXUS_GpioValue_eLow;
  leda_g = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, pinNumber, &gpioSettingsleda_g);
  NEXUS_Gpio_SetSettings(leda_g, &gpioSettingsleda_g);

  /*Led B Red*/
  pinNumber = LEDBREDGPIO;
  NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &gpioSettingsledb_r);
  gpioSettingsledb_r.mode = NEXUS_GpioMode_eOutputPushPull;
  gpioSettingsledb_r.interruptMode = NEXUS_GpioInterrupt_eDisabled;
  gpioSettingsledb_r.value = NEXUS_GpioValue_eLow;
  ledb_r = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, pinNumber, &gpioSettingsledb_r);
  NEXUS_Gpio_SetSettings(ledb_r, &gpioSettingsledb_r);

  /*Led B Green*/
  pinNumber = LEDBGRNGPIO;
  NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &gpioSettingsledb_g);
  gpioSettingsledb_g.mode = NEXUS_GpioMode_eOutputPushPull;
  gpioSettingsledb_g.interruptMode = NEXUS_GpioInterrupt_eDisabled;
  gpioSettingsledb_g.value = NEXUS_GpioValue_eLow;
  ledb_g = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, pinNumber, &gpioSettingsledb_g);
  NEXUS_Gpio_SetSettings(ledb_g, &gpioSettingsledb_g);

  NEXUS_I2c_GetDefaultSettings(&settings);
  settings.clockRate = NEXUS_I2cClockRate_e400Khz;
  hI2cHandle = NEXUS_I2c_Open(chan, &settings);

  /* Get the specific mux reg. info addr, mask, shift for Power LED */
  PowerLEDRed.Intensity = LED_MAX_PWM_VALUE;
  PowerLEDGreen.Intensity = LED_MAX_PWM_VALUE;
  PowerLEDBlue.Intensity = LED_MAX_PWM_VALUE;

  /* Enable Chip */
  Rtn = mfrfpd_Write_I2c(LP5562_REG_ENABLE, LP5562_MASTER_ENABLE);
  if(Rtn != FPD_API_RESULT_SUCCESS)
  {
    ERROR("%s - NexusTI_0DiscretesRGBPWMDriver::NexusTI_0DiscretesRGBPWMDriver Master Enable I2C Write Error\n", __FUNCTION__);
    return ret;
  }
  usleep(500);

  /* Update configuration for the clock setting to use internal clock */
  Rtn = mfrfpd_Write_I2c(LP5562_REG_CONFIG, LP5562_CLK_INT);
  if(Rtn != FPD_API_RESULT_SUCCESS)
  {
    ERROR("%s - NexusTI_0DiscretesRGBPWMDriver::NexusTI_0DiscretesRGBPWMDriver Config Internal Clock I2C Write Error\n", __FUNCTION__);
    return ret;
  }
  lp5562_wait_opmode_done();

  Rtn = mfrfpd_Write_I2c(LP5562_REG_ENG_SEL, LP5562_ENG_SEL_PWM);
  if(Rtn != FPD_API_RESULT_SUCCESS)
  {
    ERROR("%s - NexusTI_0DiscretesRGBPWMDriver::NexusTI_0DiscretesRGBPWMDriver Master Enable I2C Write Error\n", __FUNCTION__);
    return ret;
  }
  lp5562_wait_opmode_done();

  /* Initialize all channels PWM to zero -> leds off */
  for (i = 0; i < LED_FUNCTION_MAX; i++)
  {
    currentParams[i].ColorPercentage[0] = 0;
    currentParams[i].ColorPercentage[1] = 0;
    currentParams[i].ColorPercentage[2] = 0;
    currentParams[i].Intensity = 100;
    currentParams[i].On = false;
  }

  currentParams[POWER].ColorPercentage[0] = 0;
  currentParams[POWER].ColorPercentage[1] = 0;
  currentParams[POWER].ColorPercentage[2] = 0;

  Rtn = mfrfpd_LedSet(POWER, true);
  if (Rtn != FPD_API_RESULT_SUCCESS)
  {
    ERROR("%s - Error while mfrfpd_LedSet(POWER)\n", __FUNCTION__);
    ret = FPD_API_RESULT_FAILED;
  }
  else
  {
    ret = FPD_API_RESULT_SUCCESS;
  }

  blinking_thread = (pthread_t *)malloc(sizeof(*blinking_thread));
  pthread_create(blinking_thread, NULL, mfrfpd_blinking_thread, NULL);
  return ret;
}


FPD_API_RESULT FPD_DeInit(void)
{
  FPD_API_RESULT ret = FPD_API_RESULT_SUCCESS;
  int i;

  /* Stop blinking thread */
  pthread_mutex_lock(&blink_mutex);
  blink_thread_active = false;
  for (i=0; i < FPD_NUM_INDICATORS; i++)
    blinking_status[i].iterations = 0;
  pthread_mutex_unlock(&blink_mutex);
  sem_post(&blink_change_semaphore);
  pthread_join(*blinking_thread, NULL);
  free((void*)blinking_thread);
  sem_destroy(&blink_change_semaphore);

  NEXUS_Pwm_Stop(hPwmChannel);
  NEXUS_Pwm_CloseChannel(hPwmChannel);
  NEXUS_I2c_Close(hI2cHandle);

  return ret;
}


FPD_API_RESULT FPD_SetBlink(FPD_INDICATOR eIndicator, int nBlinkDuration, int nBlinkIterations)
{
  FPD_API_RESULT ret = FPD_API_RESULT_SUCCESS;

  if ( eIndicator > FPD_INDICATOR_WIFI || nBlinkDuration < FPD_INDICATOR_MIN_BLINK_DURATION_MS || nBlinkIterations < 0)
  {
    ret = FPD_API_RESULT_INVALID_PARAM;
  }
  else
  {
    nBlinkDuration -= (nBlinkDuration % BLINK_CHECK_MS);
    nBlinkIterations *= 2;
    pthread_mutex_lock(&blink_mutex);
    switch (eIndicator)
    {
    case FPD_INDICATOR_POWER:
      blinking_status[0].last_state = (blinking_status[0].iterations > 0 ? blinking_status[0].last_state : mfrfpd_indicatorState(eIndicator));
      blinking_status[0].interval   = nBlinkDuration;
      blinking_status[0].iterations = nBlinkIterations;
      blinking_status[0].duration   = nBlinkDuration * nBlinkIterations;
      break;
    case FPD_INDICATOR_NETWORK:
      blinking_status[1].last_state = (blinking_status[1].iterations > 0 ? blinking_status[1].last_state : mfrfpd_indicatorState(eIndicator));
      blinking_status[1].interval   = nBlinkDuration;
      blinking_status[1].iterations = nBlinkIterations;
      blinking_status[1].duration   = nBlinkDuration * nBlinkIterations;
      break;
    case FPD_INDICATOR_WIFI:
      blinking_status[2].last_state = (blinking_status[2].iterations > 0 ? blinking_status[2].last_state : mfrfpd_indicatorState(eIndicator));
      blinking_status[2].interval   = nBlinkDuration;
      blinking_status[2].iterations = nBlinkIterations;
      blinking_status[2].duration   = nBlinkDuration * nBlinkIterations;
      break;
    }
    pthread_mutex_unlock(&blink_mutex);
    sem_post(&blink_change_semaphore);
  }
  return ret;
}


FPD_API_RESULT FPD_SetBrightness(FPD_INDICATOR eIndicator, FPD_BRIGHTNESS eBrightness)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;

  if (eBrightness > FPD_BRIGHTNESS_MAX || eIndicator > FPD_INDICATOR_WIFI)
  {
    ERROR("%s - ERORR invalid params\n", __FUNCTION__);
    return FPD_API_RESULT_INVALID_PARAM;
  }

  if (eBrightness)

  switch (eIndicator)
  {
  case FPD_INDICATOR_POWER:
    ret = mfrfpd_Setintensity(POWER, (uint32_t)eBrightness);
    break;

  case FPD_INDICATOR_NETWORK:
    ret = mfrfpd_Setintensity(LED_A_RED, (uint32_t)eBrightness);
    ret = mfrfpd_Setintensity(LED_A_GREEN, (uint32_t)eBrightness);
    break;

  case FPD_INDICATOR_WIFI:
    ret = mfrfpd_Setintensity(LED_B_RED, (uint32_t)eBrightness);
    ret = mfrfpd_Setintensity(LED_B_GREEN, (uint32_t)eBrightness);
    break;

  default:
    ERROR("%s - SetBrigthness for indicator %d NOT SUPPORTED on this platform/n", __FUNCTION__, eIndicator);
    ret = FPD_API_RESULT_INVALID_PARAM;
    break;
  }
  return ret;
}

FPD_API_RESULT FPD_GetBrightness(FPD_INDICATOR eIndicator, FPD_BRIGHTNESS *eBrightness)
{
  FPD_API_RESULT ret = FPD_API_RESULT_SUCCESS;
  uint32_t intensity = 0;
  LEDfunction_t led;

  switch (eIndicator)
  {
  case FPD_INDICATOR_POWER:
    led = POWER;
    break;
  case FPD_INDICATOR_NETWORK:
    led = LED_A_RED;
    break;
  case FPD_INDICATOR_WIFI:
    led = LED_B_RED;
    break;
  default:
    led = LED_FUNCTION_UNKNOWN;
    break;
  }

  ret = mfrfpd_Getintensity(led, &intensity);
  if (ret == FPD_API_RESULT_SUCCESS)
  {
    *eBrightness = (FPD_BRIGHTNESS)intensity;
  }
  return ret;
}


FPD_API_RESULT FPD_SetColor(FPD_INDICATOR eIndicator, FPD_COLOR eColor)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;
  uint32_t red = 0, green = 0, blue = 0;

  switch (eColor)
  {
  case FPD_COLOR_BLUE:
    blue = 100;
    break;
  case FPD_COLOR_GREEN:
    green = 100;
    break;
  case FPD_COLOR_RED:
    red = 100;
    break;
  case FPD_COLOR_YELLOW:
    red = 100;
    green = 100;
    break;
  case FPD_COLOR_ORANGE:
    red = 100;
    green = 25;
    break;
  case FPD_COLOR_WHITE:
    red = green = blue = 100;
    break;
  default:
    ERROR("%s - Invalid color [%d] requested\n", __FUNCTION__, eColor);
    break;
  }

  switch (eIndicator)
  {
  case FPD_INDICATOR_POWER:
    ret = mfrfpd_SetColor(POWER, red, green, blue);
    break;
  case FPD_INDICATOR_NETWORK:
    ret = mfrfpd_SetColor(LED_A_RED, red, green, blue);
    ret = mfrfpd_SetColor(LED_A_GREEN, red, green, blue);
    break;

  case FPD_INDICATOR_WIFI:
    ret = mfrfpd_SetColor(LED_B_RED, red, green, blue);
    ret = mfrfpd_SetColor(LED_B_GREEN, red, green, blue);
    break;
  }

  return ret;
}


FPD_API_RESULT FPD_SetState(FPD_INDICATOR eIndicator, bool state)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;

  mfrfpd_StopBlink(eIndicator, state);

  return mfrfpd_IndicatorSetState(eIndicator, state);
}


FPD_API_RESULT FPD_GetState(FPD_INDICATOR eIndicator, bool *state)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;

  if (eIndicator > FPD_INDICATOR_WIFI)
  {
    WARN("%s - GetState for indicator %d NOT SUPPORTED on this platform/n", __FUNCTION__, eIndicator);
    ret = FPD_API_RESULT_INVALID_PARAM;
  }

  *state = mfrfpd_indicatorState(eIndicator);

  return ret;
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

/*
 * Local implementation functions
 *
 */

static void mfrfpd_StopBlink(FPD_INDICATOR eIndicator, bool bOn)
{
  FPD_API_RESULT ret = FPD_API_RESULT_SUCCESS;

  if ( eIndicator > FPD_INDICATOR_WIFI)
    return;

  ERROR("Stop blinking of %d indicator, state change to %d\n", eIndicator, bOn);
  pthread_mutex_lock(&blink_mutex);
  switch (eIndicator)
  {
  case FPD_INDICATOR_POWER:
    blinking_status[0].last_state = bOn;
    blinking_status[0].interval   = 0;
    blinking_status[0].iterations = 0;
    blinking_status[0].duration   = 0;
    break;
  case FPD_INDICATOR_NETWORK:
    blinking_status[1].last_state = bOn;
    blinking_status[1].interval   = 0;
    blinking_status[1].iterations = 0;
    blinking_status[1].duration   = 0;
    break;
  case FPD_INDICATOR_WIFI:
    blinking_status[2].last_state = bOn;
    blinking_status[2].interval   = 0;
    blinking_status[2].iterations = 0;
    blinking_status[2].duration   = 0;
    break;
  }
  pthread_mutex_unlock(&blink_mutex);
}


static FPD_API_RESULT mfrfpd_IndicatorSetState(FPD_INDICATOR eIndicator, bool state)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;

  switch (eIndicator)
  {
  case FPD_INDICATOR_POWER:
    ret = mfrfpd_LedSet(POWER, state);
    break;

  case FPD_INDICATOR_NETWORK:
      ret = mfrfpd_LedSet(LED_A_RED, state);
      ret = mfrfpd_LedSet(LED_A_GREEN, state);
    break;

  case FPD_INDICATOR_WIFI:
      ret = mfrfpd_LedSet(LED_B_RED, state);
      ret = mfrfpd_LedSet(LED_B_GREEN, state);
    break;

  default:
    WARN("%s - SetState for indicator %d NOT SUPPORTED on this platform/n", __FUNCTION__, eIndicator);
    ret = FPD_API_RESULT_INVALID_PARAM;
    break;
  }

  return ret;
}


static bool mfrfpd_indicatorState(FPD_INDICATOR eIndicator)
{
  bool state;

  pthread_mutex_lock(&mutex);
  switch (eIndicator)
  {
  case FPD_INDICATOR_POWER:
    state = currentParams[POWER].On;
    break;

  case FPD_INDICATOR_NETWORK:
    state = (currentParams[LED_A_RED].On || currentParams[LED_A_GREEN].On);
    break;

  case FPD_INDICATOR_WIFI:
    state = (currentParams[LED_B_RED].On || currentParams[LED_B_GREEN].On);
    break;
  }
  pthread_mutex_unlock(&mutex);
  return state;
}


void *mfrfpd_blinking_thread(void *param)
{
  (void)param;
  int i;
  bool state;

  memset(blinking_status, 0, sizeof(blinking_status));
  blinking_status[0].indicator = FPD_INDICATOR_POWER;
  blinking_status[1].indicator = FPD_INDICATOR_NETWORK;
  blinking_status[2].indicator = FPD_INDICATOR_WIFI;
  blink_thread_active = 1;
  sem_init(&blink_change_semaphore, 0, 1);

  DEBUG("%s - thread started\n", __FUNCTION__);

  while (blink_thread_active)
  {
    /* Blink as long as there is indicator with active blinking values */
    while((blinking_status[0].iterations > 0) ||
          (blinking_status[1].iterations > 0) ||
          (blinking_status[2].iterations > 0))
    {
      pthread_mutex_lock(&blink_mutex);
      for (i=0; i < FPD_NUM_INDICATORS; i++)
      {
        if (blinking_status[i].iterations > 0)
        {
          blinking_status[i].duration -= BLINK_CHECK_MS;
          if ((blinking_status[i].duration % blinking_status[i].interval) == 0)
          {
            blinking_status[i].iterations--;
            if (blinking_status[i].iterations > 0)
            {
              state = !mfrfpd_indicatorState(blinking_status[i].indicator);
            }
            else
            {
              state = blinking_status[i].last_state;
            }
            mfrfpd_IndicatorSetState(blinking_status[i].indicator, state);
          }
        }
      }
      pthread_mutex_unlock(&blink_mutex);
      usleep(BLINK_CHECK_US);
    }
    /* wait for new blinking orders */
    DEBUG("%s - wait for new blinking orders\n", __FUNCTION__);
    sem_wait(&blink_change_semaphore);
  }

  return NULL;
}

/**
 * @brief
 *
 * This function will ...
 *
 * @param [in] LED          led function on the device
 * @param [in]
 * @return FPD_API_RESULT   Error code.
 */

static FPD_API_RESULT mfrfpd_SetColor(LEDfunction_t LED, uint32_t red, uint32_t green, uint32_t blue)
{
  FPD_API_RESULT ret = FPD_API_RESULT_SUCCESS;
  uint32_t redold, greenold, blueold;

  if (LED >= LED_FUNCTION_MAX)
  {
    return FPD_API_RESULT_INVALID_PARAM;
  }
  pthread_mutex_lock(&mutex);

  redold = currentParams[LED].ColorPercentage[0];
  greenold = currentParams[LED].ColorPercentage[1];
  blueold = currentParams[LED].ColorPercentage[2];

  currentParams[LED].ColorPercentage[0] = red;
  currentParams[LED].ColorPercentage[1] = green;
  currentParams[LED].ColorPercentage[2] = blue;

  pthread_mutex_unlock(&mutex);

  ret = mfrfpd_LedSet(LED, currentParams[LED].On);
  if (ret != FPD_API_RESULT_SUCCESS)
  {
    pthread_mutex_lock(&mutex);
    currentParams[LED].ColorPercentage[0] = redold;
    currentParams[LED].ColorPercentage[1] = greenold;
    currentParams[LED].ColorPercentage[2] = blueold;
    pthread_mutex_unlock(&mutex);
  }

  return ret;
}

static FPD_API_RESULT mfrfpd_Setintensity(LEDfunction_t LED, uint32_t intensity)
{
  FPD_API_RESULT ret = FPD_API_RESULT_FAILED;

  if (intensity > FPD_BRIGHTNESS_MAX)
  {
    ERROR("%s - ERORR invalid param intensity > %d\n", __FUNCTION__, FPD_BRIGHTNESS_MAX);
    return FPD_API_RESULT_INVALID_PARAM;
  }

  switch (LED)
  {
  case POWER:
    pthread_mutex_lock(&mutex);
    currentParams[POWER].Intensity = intensity;
    pthread_mutex_unlock(&mutex);
    ret = mfrfpd_LedSet(LED, currentParams[LED].On);
    break;
  case LED_A_RED:
  case LED_A_GREEN:
  case LED_B_RED:
  case LED_B_GREEN:
    pthread_mutex_lock(&mutex);
    currentParams[LED_A_RED].Intensity = intensity;
    currentParams[LED_A_GREEN].Intensity = intensity;
    currentParams[LED_B_RED].Intensity = intensity;
    currentParams[LED_B_GREEN].Intensity = intensity;
    pthread_mutex_unlock(&mutex);
    ret = mfrfpd_LedSet(LED, currentParams[LED].On);
    break;
  default:
    ERROR("%s - SetBrigthness for LED %d NOT SUPPORTED on this platform/n", __FUNCTION__, LED);
    ret = FPD_API_RESULT_INVALID_PARAM;
    break;
  }
  return ret;
}


/**
 * @brief
 *
 * This function read indicator intensity for internal use
 *
 * @param [in]  LED         led function on the device
 * @param [out] *intensity  pointer to intensity variable
 * @return FPD_API_RESULT   Error code.
 */
static FPD_API_RESULT mfrfpd_Getintensity(LEDfunction_t LED, uint32_t *intensity)
{
  FPD_API_RESULT ret = FPD_API_RESULT_SUCCESS;

  if (LED >= LED_FUNCTION_MAX)
  {
    ERROR("%s - GetBrigthness for LED %d NOT SUPPORTED on this platform/n", __FUNCTION__, LED);
    ret = FPD_API_RESULT_INVALID_PARAM;
  }
  else
  {
    pthread_mutex_lock(&mutex);
    *intensity = currentParams[LED].Intensity;
    pthread_mutex_unlock(&mutex);
  }
  return ret;
}

/**
 * @brief Set Led On or Off at currently set intensity
 *
 * This function will turn particular LED on or off with previously set intensity
 *
 * @param [in] LED          led function on the device
 * @param [in] bOn          on/off state
 * @return FPD_API_RESULT   Error code.
 */
static FPD_API_RESULT mfrfpd_LedSet(LEDfunction_t LED, bool bOn)
{
  FPD_API_RESULT ret = FPD_API_RESULT_SUCCESS;
  float       fIntensity;

  DEBUG("%s - for LED %d, will be %s\n", __FUNCTION__, LED, bOn ? "ON" : "OFF");

  pthread_mutex_lock(&mutex);
  switch(LED)
  {
  case LED_A_RED:
    if(bOn && currentParams[LED_A_RED].ColorPercentage[0])
    {
      gpioSettingsleda_r.value = NEXUS_GpioValue_eHigh;
    }
    else
    {
      gpioSettingsleda_r.value = NEXUS_GpioValue_eLow;
    }
    NEXUS_Gpio_SetSettings(leda_r, &gpioSettingsleda_r);
    break;

  case LED_A_GREEN:
    if (bOn && currentParams[LED_A_GREEN].ColorPercentage[1])
    {
      gpioSettingsleda_g.value = NEXUS_GpioValue_eHigh;
    }
    else
    {
      gpioSettingsleda_g.value = NEXUS_GpioValue_eLow;
    }
    NEXUS_Gpio_SetSettings(leda_g, &gpioSettingsleda_g);
    break;

  case LED_B_RED:
    if(bOn && currentParams[LED_B_RED].ColorPercentage[0])
    {
      gpioSettingsledb_r.value = NEXUS_GpioValue_eHigh;
    }
    else
    {
      gpioSettingsledb_r.value = NEXUS_GpioValue_eLow;
    }
    NEXUS_Gpio_SetSettings(ledb_r, &gpioSettingsledb_r);
    break;

  case LED_B_GREEN:
    if (bOn  && currentParams[LED_B_GREEN].ColorPercentage[1])
    {
      gpioSettingsledb_g.value = NEXUS_GpioValue_eHigh;
    }
    else
    {
      gpioSettingsledb_g.value = NEXUS_GpioValue_eLow;
    }
    NEXUS_Gpio_SetSettings(ledb_g, &gpioSettingsledb_g);
    break;

  case POWER:
    if (bOn)
    {
      /* This driver will configure the RGB component LED to a GPIO if duty cycle is either 100% full off or 0% full on,
         otherwise it will be configured as a PWM. When entering s2, low power mode the PWM block is disabled.  This results
         in full on for each component configured as a PWM. */
      fIntensity = (currentParams[POWER].ColorPercentage[0] * currentParams[POWER].Intensity)/100;
      fIntensity = (LED_MAX_PWM_VALUE*(fIntensity/100));
      PowerLEDRed.Intensity = fIntensity;

      fIntensity = (currentParams[POWER].ColorPercentage[1] * currentParams[POWER].Intensity)/100;
      fIntensity = (LED_MAX_PWM_VALUE*(fIntensity/100));
      PowerLEDGreen.Intensity = fIntensity;

      fIntensity = (currentParams[POWER].ColorPercentage[2] * currentParams[POWER].Intensity)/100;
      fIntensity = (LED_MAX_PWM_VALUE*(fIntensity/100));
      PowerLEDBlue.Intensity = fIntensity;
      mfrfpd_Configure_PWM();
    }
    else
    {
      PowerLEDRed.Intensity = LED_MIN_PWM_VALUE;
      PowerLEDGreen.Intensity = LED_MIN_PWM_VALUE;
      PowerLEDBlue.Intensity = LED_MIN_PWM_VALUE;
      mfrfpd_Configure_PWM();
    }
    break;

  default:
    ERROR("%s - Unsupported LED %d\n", __FUNCTION__, LED);
    ret = FPD_API_RESULT_INVALID_PARAM;
    break;
  }

  switch (LED)
  {
  case LED_A_RED:
  case LED_A_GREEN:
  case LED_B_RED:
  case LED_B_GREEN:
    if (bOn)
    {
      fIntensity = (LED_MAX_PWM_VALUE * currentParams[LED].Intensity)/100;
      AB_LEDs.Intensity = fIntensity;
      NEXUS_Pwm_Stop(hPwmChannel);
      NEXUS_Pwm_SetOnInterval(hPwmChannel, (LED_MAX_PWM_VALUE - AB_LEDs.Intensity));
      NEXUS_Pwm_Start(hPwmChannel);
    }
    break;

  default:
    break;
  }

  if (ret == FPD_API_RESULT_SUCCESS)
  {
    currentParams[LED].On = bOn;
  }
  pthread_mutex_unlock(&mutex);

  return ret;
}


/**
 * @brief Helper function for communication with LP5562 device used for POWER LED
 * control.
 *
 * This function will send data to device sub address
 *
 * @param [in] I2csubAddr   8-bit internal address in LP5562 device
 * @param [in] I2CData      one byte of data to write
 * @return FPD_API_RESULT   Error code.
 */
static FPD_API_RESULT mfrfpd_Write_I2c(uint8_t I2csubAddr, uint8_t I2CData)
{
  FPD_API_RESULT Rtn = FPD_API_RESULT_SUCCESS;
  BERR_Code  error = BERR_SUCCESS;
  uint8_t    data[PWM_DATA_LENGTH];

  data[0] = I2CData;
  for(uint32_t retry_count = I2C_RETRY_COUNT; retry_count !=0; retry_count--)
  {
    error = NEXUS_I2c_Write (hI2cHandle, LP5562_CHIP_ADDRESS, I2csubAddr, &data[0], PWM_DATA_LENGTH);
    if (error == BERR_SUCCESS)
    {
      Rtn = FPD_API_RESULT_SUCCESS;
      break;
    }
    else
    {
      lp5562_wait_opmode_done();
      ERROR("%s - I2C Write Error Address = %d Data = %x\n", __FUNCTION__, I2csubAddr, data[0]);
    }
  }
  return Rtn;
}

static void mfrfpd_Configure_PWM(void)
{
  FPD_API_RESULT Rtn = FPD_API_RESULT_SUCCESS;

  Rtn = mfrfpd_Write_I2c(LP5562_REG_B_PWM, (uint8_t)PowerLEDBlue.Intensity);
  if(Rtn != FPD_API_RESULT_SUCCESS)
  {
    ERROR("%s - NexusTI Configure_PWM Write Blue PWM I2C Write Error\n", __FUNCTION__);
  }
  Rtn = mfrfpd_Write_I2c(LP5562_REG_G_PWM, (uint8_t)PowerLEDGreen.Intensity);
  if(Rtn != FPD_API_RESULT_SUCCESS)
  {
    ERROR("%s - NexusTI Configure_PWM Write Green PWM I2C Write Error\n", __FUNCTION__);
  }
  Rtn = mfrfpd_Write_I2c(LP5562_REG_R_PWM, (uint8_t)PowerLEDRed.Intensity);
  if(Rtn != FPD_API_RESULT_SUCCESS)
  {
    ERROR("%s - NexusTI Configure_PWM Write Red PWM I2C Write Error\n", __FUNCTION__);
  }
}
