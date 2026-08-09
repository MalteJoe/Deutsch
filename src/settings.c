#include "settings.h"

//Default key values

bool       key_indicator_fuzzy      = true;
Visibility key_indicator_bluetooth  = ALWAYS;
bool       key_indicator_vibe       = true;
bool       key_indicator_batt_img   = true;
bool       key_indicator_text_nrw   = false;
bool       key_indicator_text_wien  = false;
bool       key_indicator_date       = true;
uint8_t    key_indicator_theme      = 0;
uint8_t    key_indicator_text_align = 2;

static void log_settings() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Settings: fuzzy=%d, bluetooth=%d, vibe=%d, batt_img=%d, text_nrw=%d, text_wien=%d, date=%d, theme=%d, text_align=%d",
    key_indicator_fuzzy,
    key_indicator_bluetooth,
    key_indicator_vibe,
    key_indicator_batt_img,
    key_indicator_text_nrw,
    key_indicator_text_wien,
    key_indicator_date,
    key_indicator_theme,
    key_indicator_text_align
  );
} 

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


void persist_settings() {
  APP_LOG(APP_LOG_LEVEL_INFO, "Persist settings");
  log_settings();
  persist_write_bool( KEY_FUZZY,      key_indicator_fuzzy);
  persist_write_int(  KEY_BLUETOOTH,  key_indicator_bluetooth);
  persist_write_bool( KEY_VIBE,       key_indicator_vibe);
  persist_write_bool( KEY_BATT_IMG,   key_indicator_batt_img);
  persist_write_bool( KEY_TEXT_NRW,   key_indicator_text_nrw);
  persist_write_bool( KEY_TEXT_WIEN,  key_indicator_text_wien);
  persist_write_bool( KEY_DATE,       key_indicator_date);
  persist_write_int(  KEY_THEME,      key_indicator_theme);
  persist_write_int(  KEY_TEXT_ALIGN, key_indicator_text_align);
}

//Load value from storage, if storage is empty load default value
void load_persisted_settings() {
  APP_LOG(APP_LOG_LEVEL_INFO, "Load settings");
  key_indicator_fuzzy       = persist_exists(KEY_FUZZY)       ? persist_read_bool(KEY_FUZZY)      : key_indicator_fuzzy;
  key_indicator_bluetooth   = persist_exists(KEY_BLUETOOTH)   ? persist_read_int (KEY_BLUETOOTH)  : key_indicator_bluetooth;
  key_indicator_vibe        = persist_exists(KEY_VIBE)        ? persist_read_bool(KEY_VIBE)       : key_indicator_vibe;
  key_indicator_batt_img    = persist_exists(KEY_BATT_IMG)    ? persist_read_bool(KEY_BATT_IMG)   : key_indicator_batt_img;
  key_indicator_text_nrw    = persist_exists(KEY_TEXT_NRW)    ? persist_read_bool(KEY_TEXT_NRW)   : key_indicator_text_nrw;
  key_indicator_text_wien   = persist_exists(KEY_TEXT_WIEN)   ? persist_read_bool(KEY_TEXT_WIEN)  : key_indicator_text_wien;
  key_indicator_date        = persist_exists(KEY_DATE)        ? persist_read_bool(KEY_DATE)       : key_indicator_date;
  key_indicator_theme       = persist_exists(KEY_THEME)       ? persist_read_int (KEY_THEME)      : key_indicator_theme;
  key_indicator_text_align  = persist_exists(KEY_TEXT_ALIGN)  ? persist_read_int (KEY_TEXT_ALIGN) : key_indicator_text_align;
  log_settings();
}

static void process_tuple(const Tuple *t) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Received setting: key=%d, type=%d, length=%d", t->key, t->type, t->length);

  switch(t->key) {
    case KEY_FUZZY: {
      key_indicator_fuzzy = t->value->int16;
      break;
    }
    case KEY_BLUETOOTH: {
      key_indicator_bluetooth = t->value->int32;
      break;
    }
    case KEY_VIBE: {
      key_indicator_vibe = t->value->int16;
      break;
    }
    case KEY_BATT_IMG: {
      key_indicator_batt_img = t->value->int16;
      break;
    }
    case KEY_TEXT_NRW: {
      key_indicator_text_nrw = t->value->int16;
      break;
    }
    case KEY_TEXT_WIEN: {
      key_indicator_text_wien = t->value->int16;
      break;
    }
    case KEY_DATE: {
      key_indicator_date = t->value->int16;
      break;
    }
    case KEY_THEME: {
      key_indicator_theme = t->value->int32;
      break;
    }
    case KEY_TEXT_ALIGN: {
      key_indicator_text_align = t->value->int32;
      break;
    }
  }
}

void settings_process_update(DictionaryIterator *iter) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating Settings from inbox with %d entries", dict_size(iter));
  for(Tuple *t = dict_read_first(iter); t != NULL; t = dict_read_next(iter)) {
    process_tuple(t);
  }
}
