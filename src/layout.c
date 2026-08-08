#include "layout.h"

//Reference resolution the original layout was designed for (Pebble Classic / Aplite).
//All layout coordinates below are expressed relative to this size and then
//scaled to whatever the actual watch's display resolution is, so the face
//looks right on every generation (Aplite 144x168, Basalt/Diorite/Flint 144x168,
//Chalk 180x180 round, Emery 200x228, ...) instead of only filling the
//top-left 144x168 corner of larger screens.
#define BASE_W 144
#define BASE_H 168

static Window *window;

static TextLayer 
  *minuteLayer_2longlines, 
  *minuteLayer_3lines, 
  *minuteLayer_2biglines,
  *hourLayer, 
  *dateLayer;

static Layer *battery_layer;

// Bluetooth
static GBitmap *bluetooth_connected_image, *bluetooth_disconnected_image; 
static BitmapLayer *bluetooth_layer; 
static const bool key_indicator_bt_offonly    = false; // show Bluetooth icon only if offline

void set_theme() {
  APP_LOG(APP_LOG_LEVEL_INFO,"[Deutsch] Setting colors according to theme %d",key_indicator_theme);

  GColor bkgnd, date, min, hr;
  switch(key_indicator_theme) {
    default:                              // B/W
      bkgnd   = GColorBlack;
      date    = GColorWhite;
      min     = GColorWhite;
      hr      = GColorWhite;
      break;

    case 1:                               // Blue
      bkgnd   = GColorOxfordBlue;
      date    = GColorWhite;
      min     = GColorCeleste;
      hr      = GColorPastelYellow;
      break;

    case 2:                               // Green
      bkgnd   = GColorMidnightGreen;
      date    = GColorWhite;
      min     = GColorMintGreen;
      hr      = GColorPastelYellow;
      break;

    case 3:                               // Red
      bkgnd   = GColorBulgarianRose;
      date    = GColorWhite;
      min     = GColorMelon;
      hr      = GColorPastelYellow;
      break;

    case 4:                               // Gray
      bkgnd   = GColorDarkGray;
      date    = GColorWhite;
      min     = GColorPastelYellow;
      hr      = GColorWhite;
      break;

    case 5:                               // White
      bkgnd   = GColorWhite;
      date    = GColorBlack;
      min     = GColorDarkGray;
      hr      = GColorBlack;
      break;
  }

  window_set_background_color(window, bkgnd);

  text_layer_set_text_color(dateLayer, date);

  text_layer_set_text_color(minuteLayer_3lines, min);
  text_layer_set_text_color(minuteLayer_2longlines, min);
  text_layer_set_text_color(minuteLayer_2biglines, min);

  text_layer_set_text_color(hourLayer, hr);
}

//Compute and apply this watchface's layout (position, size, alignment) for
//the given unobstructed screen bounds. Called once at startup, and again
//whenever the unobstructed area changes - e.g. Timeline Quick View sliding
//in/out - so the face always adapts to the currently available space.
//Must be called only after load_text_layers()/load_battery_layers()/
//load_bluetooth_layers() have created their layers and images.
void layout_layers() {
  const GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));
  //Get alignment. On round displays (Chalk) right/left aligned text can run
  //into the curved bezel, so text is centered there instead.
#ifdef PBL_ROUND
  const GTextAlignment text_align = GTextAlignmentCenter;
  const GAlign box_align = GAlignCenter;
#else
  const GTextAlignment text_align = key_indicator_text_align;
  GAlign box_align;
  switch (key_indicator_text_align) {
  case 0:
    box_align = GAlignBottomLeft;
    break;
  case 1:
    box_align = PBL_IF_RECT_ELSE(GAlignBottom, GAlignCenter);
    break;
  case 2:
  default:
    box_align = GAlignBottomRight;
    break;
  };
#endif

  // bounding box with margins depending on the model
#ifdef PBL_RECT
  const GEdgeInsets margin = GEdgeInsets(bounds.size.h > BASE_H + 10 ? 10 : 0, bounds.size.w > BASE_W + 10 ? 10 : 0);
  const GRect r_drawing_area = grect_inset(bounds, margin);
#else
  const GRect r_drawing_area = grect_inset(bounds, GEdgeInsets(0, 10));
#endif
  
  GRect r_text_area = (GRect) {
    .origin = GPointZero,
    .size = { BASE_W, BASE_H - 10 }
  };
  grect_align(&r_text_area, &r_drawing_area, box_align, false);

  // Minute Layers
  text_layer_set_text_alignment(minuteLayer_3lines, text_align);
  layer_set_frame(text_layer_get_layer(minuteLayer_3lines), grect_inset(r_text_area, GEdgeInsets(10, 0, 0, 0)));

  text_layer_set_text_alignment(minuteLayer_2longlines, text_align);
  layer_set_frame(text_layer_get_layer(minuteLayer_2longlines), grect_inset(r_text_area, GEdgeInsets(44, 0, 0, 0)));

  text_layer_set_text_alignment(minuteLayer_2biglines, text_align);
  layer_set_frame(text_layer_get_layer(minuteLayer_2biglines), grect_inset(r_text_area, GEdgeInsets(23, 0, 0, 0)));

  // Hour Layer
  text_layer_set_text_alignment(hourLayer, text_align);
  layer_set_frame(text_layer_get_layer(hourLayer), grect_inset(r_text_area, GEdgeInsets(109, 0, 0, 0)));

#ifdef PBL_ROUND
  // use the window layer so icons don't move on quick view
  const GRect window_layer = grect_inset(layer_get_bounds(window_get_root_layer(window)), GEdgeInsets(10));
#endif

  // Battery icon
  GRect battery_frame = layer_get_bounds(battery_layer);
#ifdef PBL_RECT
  grect_align(&battery_frame, &r_drawing_area, GAlignTopLeft, false);
#else
  grect_align(&battery_frame, &window_layer, GAlignLeft, false);
#endif
  layer_set_frame(battery_layer, battery_frame);

  // Bluetooth icon
  GRect bt_frame = (GRect) {
    .origin = GPointZero,
    .size = gbitmap_get_bounds(bluetooth_connected_image).size
  };
#ifdef PBL_RECT
  grect_align(&bt_frame, &r_drawing_area, GAlignTopRight, false);
#else
  grect_align(&bt_frame, &window_layer, GAlignRight, false);
#endif  
  layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), bt_frame);

  // Date
  GRect date_frame = (GRect) {
    .origin = GPointZero,
    .size = GSize(50, 18)
  };
  grect_align(&date_frame, &r_drawing_area, GAlignTop, false);
  date_frame.origin.y -= 5;
  layer_set_frame(text_layer_get_layer(dateLayer), date_frame);
}

void update_date_visibility() {
  layer_set_hidden(text_layer_get_layer(dateLayer), !key_indicator_date);
}

void load_text_layers() {
  //Load Fonts
  GFont bitham          = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
  GFont bithamBold      = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  GFont dateFont        = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  ResHandle robotoLight = resource_get_handle(RESOURCE_ID_FONT_ROBOTO_LIGHT_34);

  //Actual position/size/alignment is applied by layout_layers(); GRectZero
  //here is just a valid placeholder for text_layer_create().
  minuteLayer_3lines = text_layer_create(GRectZero);
  text_layer_set_background_color(minuteLayer_3lines, GColorClear);
  text_layer_set_font(minuteLayer_3lines, fonts_load_custom_font(robotoLight));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minuteLayer_3lines));

  minuteLayer_2longlines = text_layer_create(GRectZero);
  text_layer_set_background_color(minuteLayer_2longlines, GColorClear);
  text_layer_set_font(minuteLayer_2longlines, fonts_load_custom_font(robotoLight));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minuteLayer_2longlines));

  minuteLayer_2biglines = text_layer_create(GRectZero);
  text_layer_set_background_color(minuteLayer_2biglines, GColorClear);
  text_layer_set_font(minuteLayer_2biglines, bitham);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minuteLayer_2biglines));

  // Configure Hour Layer
  hourLayer = text_layer_create(GRectZero);
  text_layer_set_background_color(hourLayer, GColorClear);
  text_layer_set_font(hourLayer, bithamBold);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(hourLayer));
  layer_set_clips(text_layer_get_layer(hourLayer), true);

  // Configure DateLayer
  dateLayer = text_layer_create(GRectZero);
  text_layer_set_background_color(dateLayer, GColorClear);
  text_layer_set_font(dateLayer, dateFont);
  text_layer_set_text_alignment(dateLayer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(dateLayer));
  layer_set_hidden(text_layer_get_layer(dateLayer), !key_indicator_date);
}

void update_time_text_3_minute_lines(const char* minutes, const char* hours, const char* date) {
  layer_set_hidden(text_layer_get_layer(minuteLayer_3lines), false);
  layer_set_hidden(text_layer_get_layer(minuteLayer_2longlines), true);
  layer_set_hidden(text_layer_get_layer(minuteLayer_2biglines), true);
  text_layer_set_text(minuteLayer_3lines, minutes);
  text_layer_set_text(hourLayer, hours);
  text_layer_set_text(dateLayer, date);
}

void update_time_text_2_long_lines(const char* minutes, const char* hours, const char* date) {
  layer_set_hidden(text_layer_get_layer(minuteLayer_3lines), true);
  layer_set_hidden(text_layer_get_layer(minuteLayer_2longlines), false);
  layer_set_hidden(text_layer_get_layer(minuteLayer_2biglines), true);
  text_layer_set_text(minuteLayer_2longlines, minutes);
  text_layer_set_text(hourLayer, hours);
  text_layer_set_text(dateLayer, date);
}

void update_time_text_2_big_lines(const char* minutes, const char* hours, const char* date) {
  layer_set_hidden(text_layer_get_layer(minuteLayer_3lines), true);
  layer_set_hidden(text_layer_get_layer(minuteLayer_2longlines), true);
  layer_set_hidden(text_layer_get_layer(minuteLayer_2biglines), false);
  text_layer_set_text(minuteLayer_2biglines, minutes);
  text_layer_set_text(hourLayer, hours);
  text_layer_set_text(dateLayer, date);
}

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

static void load_bluetooth_layers() {
  bluetooth_connected_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_CONNECTED);
  bluetooth_disconnected_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_DISCONNECTED);
  //Actual position/size is applied by layout_layers() once all resources
  //exist; GRectZero here is just a valid placeholder for layer_create().
  bluetooth_layer = bitmap_layer_create(GRectZero);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bluetooth_layer));
  if (key_indicator_bluetooth) {
    bluetooth_connection_service_subscribe(toggle_bluetooth_icon);
    toggle_bluetooth_icon(bluetooth_connection_service_peek());
    layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), false);
  } else {
    layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), true);
  }
}

static void window_load(Window *window) {
  //Get the actual usable bounds of this watch's screen (varies by platform:
  //144x168 on Aplite/Basalt/Diorite/Flint, 180x180 round on Chalk, 200x228 on
  //Emery, ...) instead of assuming the old fixed 144x168 Aplite resolution.
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_unobstructed_bounds(window_layer);

  //Create all layers, then position them for the current bounds.
  load_text_layers();
  battery_layer = battery_init();
  layer_add_child(window_layer, battery_layer);
  load_bluetooth_layers();
  layout_layers(bounds);

  set_theme();
  const time_t now = time(NULL);
  display_time(localtime(&now));
}

static void window_unload(Window *window) {
  text_layer_destroy(minuteLayer_3lines);
  text_layer_destroy(minuteLayer_2longlines);
  text_layer_destroy(minuteLayer_2biglines);
  text_layer_destroy(hourLayer);
  text_layer_destroy(dateLayer);

  layer_remove_from_parent(bitmap_layer_get_layer(bluetooth_layer));
  bitmap_layer_destroy(bluetooth_layer);
  gbitmap_destroy(bluetooth_connected_image);
  gbitmap_destroy(bluetooth_disconnected_image);

  layer_remove_from_parent(battery_layer);
  battery_deinit();
}

void layout_init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

#ifndef PBL_PLATFORM_APLITE
  //Re-run the layout whenever the unobstructed area changes, e.g. Timeline
  //Quick View sliding in/out, so the face adapts to the available space.
  UnobstructedAreaHandlers unobstructed_handlers = {
    .did_change = layout_layers
  };
#endif
  unobstructed_area_service_subscribe(unobstructed_handlers, NULL);
}

void layout_deinit() {
  window_destroy(window);
}
