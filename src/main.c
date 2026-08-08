#include <pebble.h>

#include "battery.h"
#include "bluetooth.h"
#include "display_time.h"
#include "layout.h"
#include "settings.h"

//If a Key is changing, do following:
static void process_tuple(const Tuple *t) {
  APP_LOG(APP_LOG_LEVEL_INFO,"[Deutsch] Received setting: key %lu is %s",t->key,t->value->cstring);

  switch(t->key) {
    //Fuzzy mode
    case KEY_FUZZY: {
      key_indicator_fuzzy = !strcmp(t->value->cstring,"on"); // easiest way to convert a on/off string into a boolean
      break;
    }
    case KEY_BLUETOOTH: {
      key_indicator_bluetooth = !strcmp(t->value->cstring,"on");
      if (key_indicator_bluetooth) {
        bluetooth_connection_service_subscribe(toggle_bluetooth_icon);
        toggle_bluetooth_icon(bluetooth_connection_service_peek());
      } else {
        bluetooth_connection_service_unsubscribe();
      }
      break;
    }
    case KEY_VIBE: {
      key_indicator_vibe = !strcmp(t->value->cstring,"on");
      break;
    }
    case KEY_BATT_IMG: {
      key_indicator_batt_img = !strcmp(t->value->cstring, "on");
      update_battery(battery_state_service_peek());

      if (key_indicator_batt_img) {
        battery_state_service_subscribe(&update_battery);
      } else {
        battery_state_service_unsubscribe();
      }
      break;
    }
    case KEY_TEXT_NRW: {
      key_indicator_text_nrw = !strcmp(t->value->cstring, "on");
      break;
    }
    case KEY_TEXT_WIEN: {
      key_indicator_text_wien = !strcmp(t->value->cstring, "on");
      break;
    }
    case KEY_DATE: {
      key_indicator_date = !strcmp(t->value->cstring, "on");
      update_date_visibility();
      break;
    }
    case KEY_THEME: {
      key_indicator_theme = atoi(t->value->cstring);
      break;
    }
    case KEY_TEXT_ALIGN: {
      key_indicator_text_align = atoi(t->value->cstring);
      break;
    }
  }
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  display_time(tick_time);
}

/*
  ###################################
  ######## Generic Functions ########
  ###################################
*/

//If a Key is changing, call process_tuple
static void in_received_handler(DictionaryIterator *iter, void *context) {
  for(Tuple *t=dict_read_first(iter); t!=NULL; t=dict_read_next(iter)) {
    process_tuple(t);
  }

  set_theme();
  layout_layers();
  const time_t now = time(NULL);
  display_time(localtime(&now));
}

static void init(void) {
  //Key
  app_message_register_inbox_received(in_received_handler); //register key receiving
  app_message_open(512, 512); //Key buffer in- and outbound

  load_persisted_settings();

  layout_init();

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
  layout_deinit();
  tick_timer_service_unsubscribe();
  unobstructed_area_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();

  persist_settings();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}