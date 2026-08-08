#pragma once

#include <pebble.h>

void toggle_bluetooth_icon(bool connected);

Layer *bluetooth_init();

void bluetooth_deinit();