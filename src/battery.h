#pragma once

#include <pebble.h>

//Battery - set image if charging, or set empty battery image if not charging
void change_battery_icon(bool charging);

//Update battery icon or hide it
void update_battery(BatteryChargeState charge_state);

Layer *battery_init();

void battery_deinit();