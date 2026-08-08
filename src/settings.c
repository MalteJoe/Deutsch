#include "settings.h"

//Default key values
bool key_indicator_fuzzy     = true;   // true = don't be too exact about the time
bool key_indicator_bluetooth = true;   // true = bluetooth icon on
bool key_indicator_vibe      = true;   // true = vibe on bluetooth disconnect
bool key_indicator_batt_img  = true;   // true = show batt usage image
bool key_indicator_text_nrw  = false;  // true = say "viertel x+1" at xx:45
bool key_indicator_text_wien = false;  // true = say "viertel x+1" at xx:15
bool key_indicator_date      = true;   // true = show date
int key_indicator_theme      = 0;      // Color Theme
int key_indicator_text_align = 2;      // Text alignment - 0 = left, 1 = center, 2 = right

void persist_settings() {
  persist_write_bool(KEY_FUZZY, key_indicator_fuzzy);
  persist_write_bool(KEY_BLUETOOTH, key_indicator_bluetooth);
  persist_write_bool(KEY_VIBE, key_indicator_vibe);
  persist_write_bool(KEY_BATT_IMG, key_indicator_batt_img);
  persist_write_bool(KEY_TEXT_NRW, key_indicator_text_nrw);
  persist_write_bool(KEY_TEXT_WIEN, key_indicator_text_wien);
  persist_write_bool(KEY_DATE, key_indicator_date);
  persist_write_int(KEY_THEME, key_indicator_theme);
  persist_write_int(KEY_TEXT_ALIGN, key_indicator_text_align);
}

//Load value from storage, if storage is empty load default value
void load_persisted_settings() {
  key_indicator_fuzzy       = persist_exists(KEY_FUZZY)       ? persist_read_bool(KEY_FUZZY)      : key_indicator_fuzzy;
  key_indicator_bluetooth   = persist_exists(KEY_BLUETOOTH)   ? persist_read_bool(KEY_BLUETOOTH)  : key_indicator_bluetooth;
  key_indicator_vibe        = persist_exists(KEY_VIBE)        ? persist_read_bool(KEY_VIBE)       : key_indicator_vibe;
  key_indicator_batt_img    = persist_exists(KEY_BATT_IMG)    ? persist_read_bool(KEY_BATT_IMG)   : key_indicator_batt_img;
  key_indicator_text_nrw    = persist_exists(KEY_TEXT_NRW)    ? persist_read_bool(KEY_TEXT_NRW)   : key_indicator_text_nrw;
  key_indicator_text_wien   = persist_exists(KEY_TEXT_WIEN)   ? persist_read_bool(KEY_TEXT_WIEN)  : key_indicator_text_wien;
  key_indicator_date        = persist_exists(KEY_DATE)        ? persist_read_bool(KEY_DATE)       : key_indicator_date;
  key_indicator_theme       = persist_exists(KEY_THEME)       ? persist_read_int(KEY_THEME)       : key_indicator_theme;
  key_indicator_text_align  = persist_exists(KEY_TEXT_ALIGN)  ? persist_read_int(KEY_TEXT_ALIGN)  : key_indicator_text_align;
}