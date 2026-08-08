# include "battery.h"

static BitmapLayer *battery_image_layer, *battery_fill_layer;
static GBitmap *battery_image;

//Battery icon will be red if charge is <= this percentage
//(could be configurable in the future)
static const int red_percent = 10;
static const bool key_indicator_batt_redonly = false; // show battery icon only if red
static uint8_t batteryPercent; //for calculating fill state

//Battery - set image if charging, or set empty battery image if not charging
void change_battery_icon(bool charging) {
  gbitmap_destroy(battery_image);
  if (charging) {
    battery_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGE);
  } else {
    battery_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY);
  }  
  bitmap_layer_set_bitmap(battery_image_layer, battery_image);
}

//Update battery icon or hide it
void update_battery(BatteryChargeState charge_state) {
  const bool show = key_indicator_batt_img
  ? (key_indicator_batt_redonly ? charge_state.charge_percent <= red_percent : true)
  : false;

  if (show) {
    batteryPercent = charge_state.charge_percent;
    layer_set_hidden(bitmap_layer_get_layer(battery_image_layer), false);
    if(batteryPercent==100) {
      change_battery_icon(false);
      layer_set_hidden(bitmap_layer_get_layer(battery_fill_layer), false);
    }
    layer_set_hidden(bitmap_layer_get_layer(battery_fill_layer), charge_state.is_charging);
    change_battery_icon(charge_state.is_charging);
  } else {
    layer_set_hidden(bitmap_layer_get_layer(battery_fill_layer), true);
    layer_set_hidden(bitmap_layer_get_layer(battery_image_layer), true);
  }
}

//draw the remaining battery percentage
static void battery_layer_update_callback(Layer *me, GContext* ctx) {
  const GColor color = batteryPercent <= red_percent ? GColorRed : GColorWhite;
  //Antialiasing is on by default where the platform supports it, but we set
  //it explicitly here since this is one of the few places we draw manually.
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_rect(ctx, GRect(2, 2, batteryPercent/100.0*11.0, 5), 0, GCornerNone);
}

Layer *battery_init() {
  battery_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY);
  const GRect bounds = gbitmap_get_bounds(battery_image);
  Layer *layer = layer_create(bounds);
  battery_fill_layer = bitmap_layer_create(bounds);
  battery_image_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(battery_image_layer, battery_image);
  layer_set_update_proc(bitmap_layer_get_layer(battery_fill_layer), battery_layer_update_callback);

  layer_add_child(layer, bitmap_layer_get_layer(battery_image_layer));
  layer_add_child(layer, bitmap_layer_get_layer(battery_fill_layer));
  if (key_indicator_batt_img) {
    battery_state_service_subscribe(&update_battery);
  }
  update_battery(battery_state_service_peek());
  return layer;
}

void battery_deinit() {
  battery_state_service_unsubscribe();

  layer_remove_from_parent(bitmap_layer_get_layer(battery_fill_layer));
  bitmap_layer_destroy(battery_fill_layer);
  layer_remove_from_parent(bitmap_layer_get_layer(battery_image_layer));
  bitmap_layer_destroy(battery_image_layer);
  gbitmap_destroy(battery_image);
}