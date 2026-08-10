#include "bluetooth.h"

#include "settings.h"

static GBitmap *bluetooth_connected_image, *bluetooth_disconnected_image; 
static BitmapLayer *bluetooth_layer; 

static void toggle_bluetooth_icon(bool connected) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Bluetooth connected: %d", connected);
  if (key_indicator_bluetooth == NEVER) {
    layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), true);
  } else {
    bitmap_layer_set_bitmap(bluetooth_layer, connected ? bluetooth_connected_image : bluetooth_disconnected_image);
    layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), key_indicator_bluetooth == WARN && connected);
  }
  if (!connected && key_indicator_vibe) {
    vibes_long_pulse();
  }
}

void bluetooth_settings_changed() {
  toggle_bluetooth_icon(connection_service_peek_pebble_app_connection());
  if (key_indicator_bluetooth > NEVER) {
    connection_service_subscribe((ConnectionHandlers) {
      .pebble_app_connection_handler = toggle_bluetooth_icon
    });
  } else {
    bluetooth_unsubscribe();
  }
}

void bluetooth_unsubscribe() {
  connection_service_unsubscribe();
}

Layer *bluetooth_layer_create() {
  bluetooth_connected_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_CONNECTED);
  bluetooth_disconnected_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_DISCONNECTED);
  bluetooth_layer = bitmap_layer_create(gbitmap_get_bounds(bluetooth_connected_image));

  return bitmap_layer_get_layer(bluetooth_layer);
}

void bluetooth_layer_destroy() {
  bitmap_layer_destroy(bluetooth_layer);
  gbitmap_destroy(bluetooth_connected_image);
  gbitmap_destroy(bluetooth_disconnected_image);
}