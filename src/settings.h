#pragma once

#include <pebble.h>

extern bool key_indicator_fuzzy;        // true = don't be too exact about the time
extern bool key_indicator_bluetooth;    // true = bluetooth icon on
extern bool key_indicator_vibe;         // true = vibe on bluetooth disconnect
extern bool key_indicator_batt_img;     // true = show batt usage image
extern bool key_indicator_text_nrw;     // true = say "viertel x+1" at xx:45
extern bool key_indicator_text_wien;    // true = say "viertel x+1" at xx:15
extern bool key_indicator_date;         // true = show date
extern int  key_indicator_theme;        // Color Theme
extern int  key_indicator_text_align;   // Text alignment - 0 = left, 1 = center, 2 = right

//Set key IDs
enum {
  KEY_FUZZY       = 0,
  KEY_BLUETOOTH   = 1,
  KEY_VIBE        = 2,
  KEY_BATT_IMG    = 3,
  KEY_TEXT_NRW    = 4,
  KEY_TEXT_WIEN   = 5,
  KEY_DATE        = 6,
  KEY_THEME       = 7,
  KEY_TEXT_ALIGN  = 8,
};

//Save keys to persistent storage
void persist_settings();

//Load value from storage, if storage is empty load default value
void load_persisted_settings();
