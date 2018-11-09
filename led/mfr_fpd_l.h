/*
 * mfr_fpd_lgi.h
 *
 *  Created on: Dec 6, 2016
 *      Author: L.Plewa
 */

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


typedef int FPD_BRIGHTNESS;
#define FPD_BRIGHTNESS_MAX 100

typedef enum _FPD_COLOR
{
  FPD_COLOR_BLUE    = (0x01<<0),  /* 1 */
  FPD_COLOR_GREEN   = (0x01<<1),  /* 2 */
  FPD_COLOR_RED     = (0x01<<2),  /* 4 */
  FPD_COLOR_YELLOW  = (0x01<<3),  /* 8 */
  FPD_COLOR_ORANGE  = (0x01<<4),  /* 16 */
  FPD_COLOR_WHITE   = (0x01<<5)   /* 32 */
}FPD_COLOR;

typedef enum _FPD_KEY
{
    FPD_SOFT_KEY_1 = 0x1,
    FPD_SOFT_KEY_2 = 0x2,
    FPD_POWER_KEY = 0x4,
}FPD_KEY;

#define FPD_SUPPORTED_COLORS  (FPD_COLOR_BLUE | \
                               FPD_COLOR_GREEN | \
                               FPD_COLOR_ORANGE | \
                               FPD_COLOR_RED | \
                               FPD_COLOR_YELLOW )

#define FPD_NUM_INDICATORS               3

typedef enum _FPD_INDICATOR {
  FPD_INDICATOR_POWER                    = (0x01<<0),
  FPD_INDICATOR_NETWORK                  = (0x01<<1),
  FPD_INDICATOR_WIFI                     = (0x01<<2)
} FPD_INDICATOR;

typedef enum _FPD_INDICATOR_SUPPORTED_COLORS
{
  FPD_INDICATOR_SUPPORTED_COLORS_NETWORK  = FPD_SUPPORTED_COLORS,
  FPD_INDICATOR_SUPPORTED_COLORS_POWER    = FPD_SUPPORTED_COLORS,
  FPD_INDICATOR_SUPPORTED_COLORS_WIFI     = FPD_SUPPORTED_COLORS

}FPD_INDICATOR_SUPPORTED_COLORS;

typedef enum _FPD_INDICATOR_MAX_BRIGHTNESS
{
  FPD_INDICATOR_MAX_BRIGHTNESS_NETWORK    = FPD_BRIGHTNESS_MAX,
  FPD_INDICATOR_MAX_BRIGHTNESS_POWER      = FPD_BRIGHTNESS_MAX,
  FPD_INDICATOR_MAX_BRIGHTNESS_WIFI       = FPD_BRIGHTNESS_MAX
}FPD_INDICATOR_MAX_BRIGHTNESS;

#define FPD_INDICATOR_MIN_BLINK_DURATION_MS   100

typedef enum _FPD_TIME
{
  FPD_TIME_12_HOUR = 0,
  FPD_TIME_24_HOUR = 1,
}FPD_TIME;


#define FPD_MAX_HORZ_ITERATIONS  5     // the maximum number of times per minute characters can scroll right to left across the display with zero hold time set.
#define FPD_MAX_VERT_ITERATIONS  20    // the maximum number of times per minute characters can scroll bottom to top across the display with zero hold time set.
#define FPD_MAX_VERT_ROWS        1     // the maximum number of times per minute characters can scroll right to left across the display with zero hold time set.
#define FPD_MAX_HORZ_COLS        4     // the maximum number of times per minute characters can scroll bottom to top across the display with zero hold time set.
#define FPD_INDICATOR_TOTAL_COUNT 5

typedef char FPD_STR[256];
#define FPD_SUPPORTED_CHARACTERS  " AbCdEFgHiJLnoPrStUy0123456789-"
typedef int (*FPD_CALLBACK) (FPD_KEY eKeyPressed, char * pCookie);

FPD_API_RESULT FPD_Init(void);
FPD_API_RESULT FPD_DeInit(void);
FPD_API_RESULT FPD_SetBlink(FPD_INDICATOR eIndicator, int nBlinkDuration, int nBlinkIterations);
FPD_API_RESULT FPD_SetBrightness(FPD_INDICATOR eIndicator, FPD_BRIGHTNESS eBrightness);
FPD_API_RESULT FPD_GetBrightness(FPD_INDICATOR eIndicator, FPD_BRIGHTNESS *eBrightness);
FPD_API_RESULT FPD_SetColor(FPD_INDICATOR eIndicator, FPD_COLOR eColor);
FPD_API_RESULT FPD_SetState(FPD_INDICATOR eIndicator, bool state);
FPD_API_RESULT FPD_GetState(FPD_INDICATOR eIndicator, bool *state);
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


#endif /* MFR_FPD_LGI_H_ */
