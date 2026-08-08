#pragma once

#include <pebble.h>

#include "battery.h"
#include "display_time.h"
#include "settings.h"

void load_text_layers();

void set_theme();

void layout_layers();

void update_date_visibility();

void update_time_text_3_minute_lines(const char* minutes, const char* hours, const char* date);
void update_time_text_2_long_lines(const char* minutes, const char* hours, const char* date);
void update_time_text_2_big_lines(const char* minutes, const char* hours, const char* date);

void toggle_bluetooth_icon(bool connected);

void layout_init();
void layout_deinit();
