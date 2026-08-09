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
  APP_LOG(APP_LOG_LEVEL_INFO, "Received inbox message with size %d", dict_size(iter));
  settings_process_update(iter);
  bluetooth_settings_changed();
  battery_settings_changed();

  set_theme();
  layout_layers();
  update_date_visibility();
  const time_t now = time(NULL);
  display_time(localtime(&now));
}

static void init(void) {
  load_persisted_settings();
  layout_init();
  bluetooth_settings_changed();
  battery_settings_changed();
  app_message_register_inbox_received(in_received_handler);
  app_message_open(128, 0);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
  app_message_deregister_callbacks();
  tick_timer_service_unsubscribe();
  unobstructed_area_service_unsubscribe();
  bluetooth_unsubscribe();
  battery_unsubscribe();

  persist_settings();
  layout_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}