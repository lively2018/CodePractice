
#ifndef DSFPDSETTINGS_H_
#define DSFPDSETTINGS_H_

#include "dsTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//namespace {

#ifdef dsFPD_INDICATOR_MESSAGE
  #undef dsFPD_INDICATOR_MESSAGE
  #undef dsFPD_INDICATOR_POWER
  #undef dsFPD_INDICATOR_RECORD
  #undef dsFPD_INDICATOR_REMOTE
  #undef dsFPD_INDICATOR_RFBYPASS
  #undef dsFPD_INDICATOR_MAX
#endif

enum dsFPD_LED_INDICATOR {
  dsFPD_INDICATOR_POWER = 0,
  dsFPD_INDICATOR_NETWORK,
  dsFPD_INDICATOR_WIFI,
  dsFPD_INDICATOR_MAX
};

/** Structure to store color enumerator value and corresponding string name defined in dsTypes.h*/
typedef struct _dsFPDColors_t_
{
  dsFPDColor_t  color;
  const char   *name;
}dsFPDColors_t;

static const dsFPDColors_t arrayAllColors[] = {
  {dsFPD_COLOR_BLUE,   "Blue"},
  {dsFPD_COLOR_GREEN,  "Green"},
  {dsFPD_COLOR_RED,    "Red"},
  {dsFPD_COLOR_YELLOW, "Yellow"},
  {dsFPD_COLOR_ORANGE, "Orange"},
  {dsFPD_COLOR_WHITE,  "White"}
};

#define _MAX_BRIGHTNESS 100
#define _MIN_BRIGHTNESS 0
#define _DEFAULT_LEVELS 10
#define _MAX_CYCLERATE  2
#define _MAX_HORZ_COLS  0
#define _MAX_VERT_ROWS  0
#define _MAX_HORZ_ITER  0
#define _MAX_VERT_ITER  0
#define _DEFAULT_COLOR_MODE 0


typedef struct _dsFPDColorConfig_t {
  int id;
  dsFPDColor_t color;
}dsFPDColorConfig_t;


typedef struct _dsFPDIndicatorConfig_t {
  dsFPDIndicator_t     id;
  unsigned int         numSupportedColors;
  dsFPDColorConfig_t * supportedColors;
  int                  maxBrightness;
  int                  maxCycleRate;
  int                  minBrightness;
  int                  levels;
int                    colorMode;
} dsFPDIndicatorConfig_t;

typedef struct _dsFPDTextDisplayConfig_t {
  dsFPDTextDisplay_t    id;
  const char *          name;
  dsFPDColorConfig_t *  supportedColors;
  int                   maxBrightness;
  int                   maxCycleRate;
  const char *          supportedCharacters;
  int                   columns;
  int                   rows;
  int                   maxHorizontalIterations;
  int                   maxVerticalIterations;
  int                   levels;
  int                   colorMode;
} dsFPDTextDisplayConfig_t;

static dsFPDColorConfig_t  kPowerIndicatorColors[dsFPD_COLOR_MAX] = {
  {
    /*.Id = */ 0,
    /*.color = */ dsFPD_COLOR_BLUE,
  },
  {
    /*.Id = */ 1,
    /*.color = */ dsFPD_COLOR_GREEN,
  },
  {
    /*.Id = */ 2,
    /*.color = */ dsFPD_COLOR_RED,
  },
  {
    /*.Id = */ 3,
    /*.color = */ dsFPD_COLOR_YELLOW,
  },
  {
    /*.Id = */ 4,
    /*.color = */ dsFPD_COLOR_ORANGE,
  },
  {
    /*.Id = */ 5,
    /*.color = */ dsFPD_COLOR_WHITE,
  }
};

static dsFPDColorConfig_t  kOtherIndicatorColors[4] = {
  {
    /*.Id = */ 0,
    /*.color = */ dsFPD_COLOR_GREEN,
  },
  {
    /*.Id = */ 1,
    /*.color = */ dsFPD_COLOR_RED,
  },
  {
    /*.Id = */ 2,
    /*.color = */ dsFPD_COLOR_YELLOW,
  },
  {
    /*.Id = */ 3,
    /*.color = */ dsFPD_COLOR_ORANGE,
  },

};

static const dsFPDIndicatorConfig_t kIndicators[dsFPD_INDICATOR_MAX] = {
  {
    /*.id              = */ dsFPD_INDICATOR_POWER,
    /*.numSupportedColors*/ (sizeof(kPowerIndicatorColors)/sizeof(kPowerIndicatorColors[0])),
    /*.supportedColors = */ kPowerIndicatorColors,
    /*.maxBrightness   = */ _MAX_BRIGHTNESS,
    /*.maxCycleRate    = */ _MAX_CYCLERATE,
    /*.minBrightness   = */ _MIN_BRIGHTNESS,
    /*.levels          = */ _DEFAULT_LEVELS,
    /*.colorMode       = */ _DEFAULT_COLOR_MODE,
  },
  {
    /*.id              = */ dsFPD_INDICATOR_NETWORK,
    /*.numSupportedColors*/ (sizeof(kOtherIndicatorColors)/sizeof(kOtherIndicatorColors[0])),
    /*.supportedColors = */ kOtherIndicatorColors,
    /*.maxBrightness   = */ _MAX_BRIGHTNESS,
    /*.maxCycleRate    = */  _MAX_CYCLERATE,
    /*.minBrightness   = */ _MIN_BRIGHTNESS,
    /*.levels          = */ _DEFAULT_LEVELS,
    /*.colorMode       = */ _DEFAULT_COLOR_MODE,
  },
  {
    /*.id              = */ dsFPD_INDICATOR_WIFI,
    /*.numSupportedColors*/ (sizeof(kOtherIndicatorColors)/sizeof(kOtherIndicatorColors[0])),
    /*.supportedColors = */ kOtherIndicatorColors,
    /*.maxBrightness   = */ _MAX_BRIGHTNESS,
    /*.maxCycleRate    = */  _MAX_CYCLERATE,
    /*.minBrightness   = */ _MIN_BRIGHTNESS,
    /*.levels          = */ _DEFAULT_LEVELS,
    /*.colorMode       = */ _DEFAULT_COLOR_MODE,
  },
};

#ifndef HAS_CLOCK_DISPLAY
static const dsFPDTextDisplayConfig_t * kTextDisplays = NULL;
#else
static const dsFPDTextDisplayConfig_t  kTextDisplays[dsFPD_TEXTDISP_MAX] = {
  {
    /*.id                    = */ dsFPD_TEXTDISP_TEXT,
    /*.name                  = */ "Text",
    /*.supportedColors       = */ kIndicatorColors,
    /*.maxBrightness         = */ _MAX_BRIGHTNESS,
    /*.maxCycleRate          = */ _MAX_CYCLERATE,
    /*.supportedCharacters   = */ "ABCDEFG",
    /*.columns               = */ _MAX_HORZ_COLS,
    /*.rows                  = */ _MAX_VERT_ROWS,
    /*.horizontal iterations = */ _MAX_HORZ_ITER,
    /*.vertical iterations   = */ _MAX_VERT_ITER,
    /*.levels                = */ _DEFAULT_LEVELS,
    /*.colorMode             = */ _DEFAULT_COLOR_MODE
  },
};
#endif

inline const bool isIndicatorValid(int id) {
  return (id < dsFPD_INDICATOR_MAX);
}

}
#ifdef __cplusplus
//}
#endif


#endif /* DSFPDSETTINGS_H_ */
