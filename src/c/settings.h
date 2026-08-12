#pragma once

#include <pebble.h>

typedef enum {
  NEVER  = 0,
  WARN   = 1,
  ALWAYS = 2,
} Visibility;

extern bool       key_indicator_fuzzy;        // true = don't be too exact about the time
extern Visibility key_indicator_bluetooth;    // icon visibility - WARN = only when disconnected
extern bool       key_indicator_vibe;         // true = vibe on bluetooth disconnect
extern Visibility key_indicator_batt_img;     // icon visibility - WARN = Only when below threshold or charging
extern bool       key_indicator_text_nrw;     // true = say "viertel x+1" at xx:45
extern bool       key_indicator_text_wien;    // true = say "viertel x+1" at xx:15
extern bool       key_indicator_date;         // true = show date
extern uint8_t    key_indicator_theme;        // Color Theme
extern uint8_t    key_indicator_text_align;   // Text alignment - 0 = left, 1 = center, 2 = right

//Save keys to persistent storage
void persist_settings();

//Load value from storage, if storage is empty load default value
void load_persisted_settings();

void settings_process_update(DictionaryIterator *iter);
