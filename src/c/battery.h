#pragma once

#include <pebble.h>

void battery_settings_changed();
void battery_unsubscribe();

Layer *battery_layer_create();
void battery_layer_destroy();