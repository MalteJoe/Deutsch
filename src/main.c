#include <pebble.h>

#include "battery.h"
#include "bluetooth.h"
#include "display_time.h"
#include "layout.h"
#include "settings.h"

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  display_time(tick_time);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  settings_process_update(iter);

  if (key_indicator_bluetooth) {
    toggle_bluetooth_icon(bluetooth_connection_service_peek());
    bluetooth_connection_service_subscribe(toggle_bluetooth_icon);
  } else {
    bluetooth_connection_service_unsubscribe();
  }

  update_battery(battery_state_service_peek());
  if (key_indicator_batt_img) {
    battery_state_service_subscribe(&update_battery);
  } else {
    battery_state_service_unsubscribe();
  }

  set_theme();
  layout_layers();
  update_date_visibility();
  const time_t now = time(NULL);
  display_time(localtime(&now));
}

static void init(void) {
  load_persisted_settings();
  layout_init();
  
  app_message_register_inbox_received(in_received_handler);
  app_message_open(512, 0);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
  app_message_deregister_callbacks();
  tick_timer_service_unsubscribe();
  unobstructed_area_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();

  persist_settings();
  layout_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}