#include "bluetooth.h"

#include "settings.h"

// Bluetooth
static GBitmap *bluetooth_connected_image, *bluetooth_disconnected_image; 
static BitmapLayer *bluetooth_layer; 
static const bool key_indicator_bt_offonly    = false; // show Bluetooth icon only if offline

//Bluetooth
void toggle_bluetooth_icon(bool connected) {
  if (connected) {
    if (key_indicator_bt_offonly) {
      layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), true);
    } else {
      bitmap_layer_set_bitmap(bluetooth_layer, bluetooth_connected_image);
      layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), false);
  	}
  } else {
    bitmap_layer_set_bitmap(bluetooth_layer, bluetooth_disconnected_image);
    layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), false);
  }
}

Layer *bluetooth_init() {
  bluetooth_connected_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_CONNECTED);
  bluetooth_disconnected_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_DISCONNECTED);
  bluetooth_layer = bitmap_layer_create(gbitmap_get_bounds(bluetooth_connected_image));
  if (key_indicator_bluetooth) {
    bluetooth_connection_service_subscribe(toggle_bluetooth_icon);
    toggle_bluetooth_icon(bluetooth_connection_service_peek());
    layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), false);
  } else {
    layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), true);
  }
  return bitmap_layer_get_layer(bluetooth_layer);
}

void bluetooth_deinit() {
  bitmap_layer_destroy(bluetooth_layer);
  gbitmap_destroy(bluetooth_connected_image);
  gbitmap_destroy(bluetooth_disconnected_image);
}