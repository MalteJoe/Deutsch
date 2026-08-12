#pragma once

#include <pebble.h>

void bluetooth_settings_changed();
void bluetooth_unsubscribe();

Layer *bluetooth_layer_create();
void bluetooth_layer_destroy();