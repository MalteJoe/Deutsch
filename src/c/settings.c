#include "settings.h"

//Default key values

bool       key_indicator_fuzzy      = true;
Visibility key_indicator_bluetooth  = WARN;
bool       key_indicator_vibe       = true;
Visibility key_indicator_batt_img   = WARN;
bool       key_indicator_text_nrw   = false;
bool       key_indicator_text_wien  = false;
bool       key_indicator_date       = true;
uint8_t    key_indicator_theme      = 0;
uint8_t    key_indicator_text_align = 2;

static void log_settings() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Settings:");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "fuzzy=%d",      key_indicator_fuzzy);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "bluetooth=%d",  key_indicator_bluetooth);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "vibe=%d",       key_indicator_vibe);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "batt_img=%d",   key_indicator_batt_img);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "text_nrw=%d",   key_indicator_text_nrw);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "text_wien=%d",  key_indicator_text_wien);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "date=%d",       key_indicator_date);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "theme=%d",      key_indicator_theme);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "text_align=%d", key_indicator_text_align);
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
  persist_write_int(  KEY_BATT_IMG,   key_indicator_batt_img);
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
  key_indicator_batt_img    = persist_exists(KEY_BATT_IMG)    ? persist_read_int (KEY_BATT_IMG)   : key_indicator_batt_img;
  key_indicator_text_nrw    = persist_exists(KEY_TEXT_NRW)    ? persist_read_bool(KEY_TEXT_NRW)   : key_indicator_text_nrw;
  key_indicator_text_wien   = persist_exists(KEY_TEXT_WIEN)   ? persist_read_bool(KEY_TEXT_WIEN)  : key_indicator_text_wien;
  key_indicator_date        = persist_exists(KEY_DATE)        ? persist_read_bool(KEY_DATE)       : key_indicator_date;
  key_indicator_theme       = persist_exists(KEY_THEME)       ? persist_read_int (KEY_THEME)      : key_indicator_theme;
  key_indicator_text_align  = persist_exists(KEY_TEXT_ALIGN)  ? persist_read_int (KEY_TEXT_ALIGN) : key_indicator_text_align;
  log_settings();
}

void settings_process_update(DictionaryIterator *iter) {
  Tuple *t;

  for (t = dict_read_first(iter); t != NULL; t = dict_read_next(iter)) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received key=%d of type=%d with size=%d", t->key, t->type, t->length);
    switch (t->type)
    {
    case 1: // string
      APP_LOG(APP_LOG_LEVEL_DEBUG, "cstring=%s", t->value->cstring);
      break;
    case 3: // int
      switch (t->length)
      {
      case 1:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "int8=%d", t->value->int8);
        break;
      case 2:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "int16=%d", t->value->int16);
        break;
      case 4:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "int32=%d", t->value->int32);
        break;
      
      default:
        break;
      }
    default:
      break;
    }
  }
  
  t = dict_find(iter, MESSAGE_KEY_Fuzzy);
  if (t != NULL) {
    key_indicator_fuzzy = t->value->int32;
  }

  t = dict_find(iter, MESSAGE_KEY_BluetoothIcon);
  if (t != NULL) {
    // TODO with the next release of clay:
    // key_indicator_bluetooth = t->value->int32;
    key_indicator_bluetooth = atoi(t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_VibeOnDisconnect);
  if (t != NULL) {
    key_indicator_vibe = t->value->int32;
  }

  t = dict_find(iter, MESSAGE_KEY_BatteryIcon);
  if (t != NULL) {
    // TODO with the next release of clay:
    // key_indicator_batt_img = t->value->int32;
    key_indicator_batt_img = atoi(t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_TextNRW);
  if (t != NULL) {
    // TODO with the next release of clay:
    // key_indicator_text_nrw = t->value->int32;
    key_indicator_text_nrw = atoi(t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_TextWien);
  if (t != NULL) {
    // TODO with the next release of clay:
    // key_indicator_text_wien = t->value->int32;
    key_indicator_text_wien = atoi(t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_Date);
  if (t != NULL) {
    key_indicator_date = t->value->int32;
  }

  t = dict_find(iter, MESSAGE_KEY_ColorTheme);
  if (t != NULL) {
    // TODO with the next release of clay:
    // key_indicator_theme = t->value->int32;
    key_indicator_theme = atoi(t->value->cstring);
  }

  t = dict_find(iter, MESSAGE_KEY_TextAlignment);
  if (t != NULL) {
    // TODO with the next release of clay:
    // key_indicator_text_align = t->value->int32;
    key_indicator_text_align = atoi(t->value->cstring);
  }
  
  log_settings();
}
