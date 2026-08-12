# include "battery.h"

#include "settings.h"

static BitmapLayer *battery_image_layer, *battery_fill_layer;
static GBitmap *battery_frame;
static GBitmap *battery_charging;

//Battery icon will be red if charge is <= this percentage
//(could be configurable in the future)
static const int red_percent = 10;
static BatteryChargeState s_charge_state; //for rendering the fill state during the draw callback

//Battery - set image if charging, or set empty battery image if not charging
static void change_battery_icon(bool charging) {
  bitmap_layer_set_bitmap(battery_image_layer, charging ? battery_charging : battery_frame);
}

//Update battery icon or hide it
static void update_battery(BatteryChargeState charge_state) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Battery update: charge_percent=%d, is_charging=%d, is_plugged=%d", charge_state.charge_percent, charge_state.is_charging, charge_state.is_plugged);

  if (key_indicator_batt_img == NEVER) {
    layer_set_hidden(bitmap_layer_get_layer(battery_fill_layer), true);
    layer_set_hidden(bitmap_layer_get_layer(battery_image_layer), true);
  } else {
    change_battery_icon(charge_state.is_charging);
    s_charge_state = charge_state;
    const bool show = key_indicator_batt_img == ALWAYS 
        || charge_state.is_charging
        || charge_state.charge_percent <= red_percent;
    layer_set_hidden(bitmap_layer_get_layer(battery_image_layer), !show);
    layer_set_hidden(bitmap_layer_get_layer(battery_fill_layer), !show);
  }
}

//draw the remaining battery percentage
static void battery_layer_update_callback(Layer *me, GContext* ctx) {
  const GColor color = s_charge_state.is_charging ? GColorGreen 
      : s_charge_state.charge_percent <= red_percent ? GColorRed : GColorWhite;
  //Antialiasing is on by default where the platform supports it, but we set
  //it explicitly here since this is one of the few places we draw manually.
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_rect(ctx, GRect(2, 2, s_charge_state.charge_percent/100.0*11.0, 5), 0, GCornerNone);
}

void battery_settings_changed() {
  update_battery(battery_state_service_peek());
  if (key_indicator_batt_img > NEVER) {
    battery_state_service_subscribe(&update_battery);
  } else {
    battery_unsubscribe();
  }
}

void battery_unsubscribe() {
  battery_state_service_unsubscribe();
}

Layer *battery_layer_create() {
  battery_frame = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY);
  battery_charging = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGE);
  const GRect bounds = gbitmap_get_bounds(battery_frame);
  Layer *layer = layer_create(bounds);
  battery_fill_layer = bitmap_layer_create(bounds);
  battery_image_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(battery_image_layer, battery_frame);
  bitmap_layer_set_compositing_mode(battery_image_layer, PBL_IF_BW_ELSE(GCompOpAssignInverted, GCompOpSet));
  layer_set_update_proc(bitmap_layer_get_layer(battery_fill_layer), battery_layer_update_callback);

  layer_add_child(layer, bitmap_layer_get_layer(battery_fill_layer));
  layer_add_child(layer, bitmap_layer_get_layer(battery_image_layer));
  return layer;
}

void battery_layer_destroy() {
  battery_state_service_unsubscribe();

  layer_remove_from_parent(bitmap_layer_get_layer(battery_fill_layer));
  bitmap_layer_destroy(battery_fill_layer);
  layer_remove_from_parent(bitmap_layer_get_layer(battery_image_layer));
  bitmap_layer_destroy(battery_image_layer);
  gbitmap_destroy(battery_frame);
  gbitmap_destroy(battery_charging);
}