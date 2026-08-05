#include <pebble.h>

//Window
static Window *window;

//Bluetooth
static GBitmap *bluetooth_connected_image, *bluetooth_disconnected_image; //Bluetooth images
static BitmapLayer *bluetooth_layer; //Bluetooth layer

//Battery
static uint8_t batteryPercent; //for calculating fill state
static GBitmap *battery_image;
static BitmapLayer *battery_image_layer, *battery_fill_layer; //battery icon, show fill status

//Text Lines
static TextLayer *minuteLayer_2longlines, *minuteLayer_3lines, *minuteLayer_2biglines, *hourLayer, *dateLayer;
static GFont robotoLightFont, robotoLightLargeFont;

//Set key IDs
enum {
  KEY_FUZZY     = 0,
  KEY_BLUETOOTH = 1,
  KEY_VIBE      = 2,
  KEY_BATT_IMG  = 3,
  KEY_TEXT_NRW  = 4,
  KEY_TEXT_WIEN = 5,
  KEY_DATE      = 6,
  KEY_THEME     = 7,
};

//Default key values
static bool key_indicator_fuzzy 	= true;	    //true = don't be too exact about the time
static bool key_indicator_bluetooth	= true;		//true = bluetooth icon on
static bool key_indicator_vibe 		= true;		//true = vibe on bluetooth disconnect
static bool key_indicator_batt_img	= true;		//true = show batt usage image
static bool key_indicator_text_nrw	= false;	//true = say "viertel x+1" at xx:45
static bool key_indicator_text_wien	= false;	//true = say "viertel x+1" at xx:15
static bool key_indicator_date		= true;		//true = show date
static int  key_indicator_theme     = 0;        //Color Theme

// The following are not yet configurable, but let's pretend they are:
static const bool key_indicator_batt_redonly = true;	// true = show battery icon only if red
static const bool key_indicator_bt_offonly = true;		// true = show Bluetooth icon only if offline
static const bool key_indicator_rightalign = true;		// true = right aligned text, false=left aligned

//Reference resolution the original layout was designed for (Pebble Classic / Aplite).
//All layout coordinates below are expressed relative to this size and then
//scaled to whatever the actual watch's display resolution is, so the face
//looks right on every generation (Aplite 144x168, Basalt/Diorite/Flint 144x168,
//Chalk 180x180 round, Emery 200x228, ...) instead of only filling the
//top-left 144x168 corner of larger screens.
#define BASE_W 144
#define BASE_H 168

//Scale a horizontal / vertical coordinate (or length) given in "base" pixels
//to the equivalent value on the watch's actual screen.
static int16_t scale_x(int16_t v, GRect bounds) {
  return (int16_t)((int32_t)v * bounds.size.w / BASE_W);
}
static int16_t scale_y(int16_t v, GRect bounds) {
  return (int16_t)((int32_t)v * bounds.size.h / BASE_H);
}

//Battery icon will be red if charge is <= this percentage
//(could be configurable in the future)
static const int red_percent = 10;

/*
  ##################################
  ######## Custom Functions ########
  ##################################
*/

static void set_theme() {
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

//Battery - set image if charging, or set empty battery image if not charging
static void change_battery_icon(bool charging) {
  gbitmap_destroy(battery_image);
  if(charging) {
    battery_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGE);
  } else {
    battery_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY);
  }  
  bitmap_layer_set_bitmap(battery_image_layer, battery_image);
  layer_mark_dirty(bitmap_layer_get_layer(battery_image_layer));
}

//Update battery icon or hide it
static void update_battery(BatteryChargeState charge_state) {
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
  const GColor color = batteryPercent <= red_percent ? GColorRed : key_indicator_theme==4 ? GColorBlack : GColorWhite;
  //Antialiasing is on by default where the platform supports it, but we set
  //it explicitly here since this is one of the few places we draw manually.
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_rect(ctx, GRect(2, 2, batteryPercent/100.0*11.0, 5), 0, GCornerNone);
}

static void load_battery_layers() {
  battery_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY);
  //Actual position/size is applied by layout_layers() once all resources
  //exist; GRectZero here is just a valid placeholder for layer_create().
  battery_fill_layer = bitmap_layer_create(GRectZero);
  battery_image_layer = bitmap_layer_create(GRectZero);
  bitmap_layer_set_bitmap(battery_image_layer, battery_image);
  layer_set_update_proc(bitmap_layer_get_layer(battery_fill_layer), battery_layer_update_callback);
	
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(battery_image_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(battery_fill_layer));
  if (key_indicator_batt_img) {
    battery_state_service_subscribe(&update_battery);
  }
  update_battery(battery_state_service_peek());
}

//Bluetooth
static void toggle_bluetooth_icon(bool connected) { // Toggle bluetooth
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
  if (!connected && key_indicator_vibe) {
    vibes_long_pulse();
  }
}

static void bluetooth_connection_callback(bool connected) {  //Bluetooth handler
  toggle_bluetooth_icon(connected);
}

static void load_bluetooth_layers() {
  bluetooth_connected_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_CONNECTED);
  bluetooth_disconnected_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_DISCONNECTED);
  //Actual position/size is applied by layout_layers() once all resources
  //exist; GRectZero here is just a valid placeholder for layer_create().
  bluetooth_layer = bitmap_layer_create(GRectZero);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bluetooth_layer));
  if (key_indicator_bluetooth) {
    bluetooth_connection_service_subscribe(bluetooth_connection_callback);
    bluetooth_connection_callback(bluetooth_connection_service_peek());
    layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), false);
  } else {
    layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), true);
  }
}

//If a Key is changing, do following:
static void process_tuple(const Tuple *t) {
  APP_LOG(APP_LOG_LEVEL_INFO,"[Deutsch] Received setting: key %lu is %s",t->key,t->value->cstring);

  switch(t->key) {
    //Fuzzy mode
    case KEY_FUZZY: {
      key_indicator_fuzzy = !strcmp(t->value->cstring,"on"); // easiest way to convert a on/off string into a boolean
      break;
    }
    case KEY_BLUETOOTH: {
	  key_indicator_bluetooth = !strcmp(t->value->cstring,"on");
      layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), !key_indicator_bluetooth);
      if (key_indicator_bluetooth) {
        bluetooth_connection_service_subscribe(bluetooth_connection_callback);
        bluetooth_connection_callback(bluetooth_connection_service_peek());
      } else {
        bluetooth_connection_service_unsubscribe();
      }
      break;
    }
    case KEY_VIBE: {
		  key_indicator_vibe = !strcmp(t->value->cstring,"on");
      break;
    }
    case KEY_BATT_IMG: {
		  key_indicator_batt_img = !strcmp(t->value->cstring,"on");
      update_battery(battery_state_service_peek());
      
      if (key_indicator_batt_img) {
        battery_state_service_subscribe(&update_battery);
      }
      else {
        battery_state_service_unsubscribe();
      }
      break;
    }
    case KEY_TEXT_NRW: {
		  key_indicator_text_nrw = !strcmp(t->value->cstring,"on");
      break;
    }
    case KEY_TEXT_WIEN: {
		  key_indicator_text_wien = !strcmp(t->value->cstring,"on");
      break;
    }
    case KEY_DATE: {
      key_indicator_date = !strcmp(t->value->cstring,"on");
      layer_set_hidden(text_layer_get_layer(dateLayer), !key_indicator_date);
      break;
    }
    case KEY_THEME: {
      key_indicator_theme = atoi(t->value->cstring);
      break;
    }
  }
}

static void load_text_layers() {
  //Load Fonts
  GFont bitham 			= fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
  GFont bithamBold 		= fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  GFont dateFont		= fonts_get_system_font(FONT_KEY_GOTHIC_18);
  ResHandle robotoLight	= resource_get_handle(RESOURCE_ID_FONT_ROBOTO_LIGHT_34);
  ResHandle robotoLightLarge = resource_get_handle(RESOURCE_ID_FONT_ROBOTO_LIGHT_42);
  robotoLightFont = fonts_load_custom_font(robotoLight);
  robotoLightLargeFont = fonts_load_custom_font(robotoLightLarge);

  //Actual position/size/alignment is applied by layout_layers(); GRectZero
  //here is just a valid placeholder for text_layer_create().
  minuteLayer_3lines = text_layer_create(GRectZero);
  text_layer_set_background_color(minuteLayer_3lines, GColorClear);
  text_layer_set_font(minuteLayer_3lines, robotoLightFont);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minuteLayer_3lines));

  minuteLayer_2longlines = text_layer_create(GRectZero);
  text_layer_set_background_color(minuteLayer_2longlines, GColorClear);
  text_layer_set_font(minuteLayer_2longlines, robotoLightFont);
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

  // Configure DateLayer
  dateLayer = text_layer_create(GRectZero);
  text_layer_set_background_color(dateLayer, GColorClear);
  text_layer_set_font(dateLayer, dateFont);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(dateLayer));
  layer_set_hidden(text_layer_get_layer(dateLayer), !key_indicator_date);
}

//Compute and apply this watchface's layout (position, size, alignment) for
//the given unobstructed screen bounds. Called once at startup, and again
//whenever the unobstructed area changes - e.g. Timeline Quick View sliding
//in/out - so the face always adapts to the currently available space.
//Must be called only after load_text_layers()/load_battery_layers()/
//load_bluetooth_layers() have created their layers and images.
static void layout_layers(GRect bounds) {
  //Get alignment. On round displays (Chalk) right/left aligned text can run
  //into the curved bezel, so text is centered there instead.
#if defined(PBL_ROUND)
  const GTextAlignment align = GTextAlignmentCenter;
  (void)key_indicator_rightalign; // not applicable on round displays
#else
  const GTextAlignment align = key_indicator_rightalign ? GTextAlignmentRight : GTextAlignmentLeft;
#endif

  //Extra side margin so text doesn't get clipped by a round bezel. 0 on
  //rectangular displays, where the layout matches the original design.
  const int16_t round_margin = PBL_IF_ROUND_ELSE(10, 0);

  //Keep the time text column wider than the original only moderately, so it
  //still wraps close to the original Pebble layout without stretching all the
  //way to the full screen width on large watch faces. The previous full-width
  //approach was too wide; this preserves the intended wrapping behavior while
  //giving a little extra breathing room on larger screens.
  const int16_t extra_w = bounds.size.w > BASE_W ? (int16_t)(bounds.size.w - BASE_W) : 0;
  const int16_t allow_wide = extra_w > 0 ? (extra_w > 18 ? 18 : extra_w) : 0;
  const int16_t w = BASE_W - 3 - 2 * round_margin + allow_wide;
  const int16_t x0 = bounds.origin.x + (bounds.size.w - w) / 2 + round_margin;

  const bool use_large_fonts = bounds.size.w >= 180 || bounds.size.h >= 220;
  if (use_large_fonts) {
    text_layer_set_font(minuteLayer_3lines, robotoLightLargeFont);
    text_layer_set_font(minuteLayer_2longlines, robotoLightLargeFont);
    text_layer_set_font(minuteLayer_2biglines, robotoLightLargeFont);
  } else {
    text_layer_set_font(minuteLayer_3lines, robotoLightFont);
    text_layer_set_font(minuteLayer_2longlines, robotoLightFont);
    text_layer_set_font(minuteLayer_2biglines, robotoLightFont);
  }

  //The fonts are fixed-size bitmap fonts - they don't get bigger on a taller
  //screen - so the vertical *gap* between the minutes and the hour must stay
  //fixed too. Scaling it by the screen height (like the horizontal layout
  //scales width) just stretches empty space in between. Instead, keep the
  //original pixel spacing and use any extra screen height as a single shared
  //offset that nudges the whole time+date block down, roughly centering it
  //and giving the date some breathing room at the top on taller screens.
  const int16_t extra_h = bounds.size.h > BASE_H ? (int16_t)(bounds.size.h - BASE_H) : 0;
  const int16_t y = bounds.origin.y + extra_h / 2;
  const int16_t screen_bottom = bounds.origin.y + bounds.size.h;

  // Minute Layers
  text_layer_set_text_alignment(minuteLayer_3lines, align);
  layer_set_frame(text_layer_get_layer(minuteLayer_3lines), (GRect) {
    .origin = { x0, y + 10 },
    .size   = { w, screen_bottom - (y + 10) }
  });

  text_layer_set_text_alignment(minuteLayer_2longlines, align);
  layer_set_frame(text_layer_get_layer(minuteLayer_2longlines), (GRect) {
    .origin = { x0, y + 44 },
    .size   = { w, screen_bottom - (y + 44) }
  });

  text_layer_set_text_alignment(minuteLayer_2biglines, align);
  layer_set_frame(text_layer_get_layer(minuteLayer_2biglines), (GRect) {
    .origin = { x0, y + 23 },
    .size   = { w, screen_bottom - (y + 23) }
  });

  // Hour Layer
  text_layer_set_text_alignment(hourLayer, align);
  layer_set_frame(text_layer_get_layer(hourLayer), (GRect) {
    .origin = { x0, y + 109 },
    .size   = { w, screen_bottom - (y + 109) }
  });

  // Battery icon - anchored proportionally to the top-left corner, native icon size.
  const GRect battery_frame = (GRect) {
    .origin = { bounds.origin.x + scale_x(3, bounds), bounds.origin.y + scale_y(2, bounds) },
    .size = gbitmap_get_bounds(battery_image).size
  };
  layer_set_frame(bitmap_layer_get_layer(battery_image_layer), battery_frame);
  layer_set_frame(bitmap_layer_get_layer(battery_fill_layer), battery_frame);

  // Bluetooth icon - anchored proportionally to the top-right corner, native icon size.
  const GSize bt_icon_size = gbitmap_get_bounds(bluetooth_connected_image).size;
  layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), (GRect) {
    .origin = { bounds.origin.x + bounds.size.w - bt_icon_size.w - scale_x(2, bounds), bounds.origin.y + scale_y(2, bounds) },
    .size = bt_icon_size
  });

  // Date Layer - genuinely centered horizontally (inset just enough to clear
  // the battery/bluetooth icons on either side, so it stays centered even
  // though only one side actually has an icon). Uses its own small top
  // margin (independent of the minute/hour block's vertical offset above),
  // since it doesn't need to grow nearly as much on taller screens.
  const int16_t date_icon_clearance = (battery_frame.size.w > bt_icon_size.w ? battery_frame.size.w : bt_icon_size.w)
                                       + scale_x(6, bounds);
  layer_set_frame(text_layer_get_layer(dateLayer), (GRect) {
    .origin = { bounds.origin.x + date_icon_clearance, bounds.origin.y + scale_y(6, bounds) },
    .size   = { bounds.size.w - 2 * date_icon_clearance, scale_y(20, bounds) }
  });
  text_layer_set_text_alignment(dateLayer, GTextAlignmentCenter);
}

//Called whenever the unobstructed screen area finishes changing (e.g. after
//Timeline Quick View has fully appeared or disappeared), so the layout can
//be recalculated for the new available space.
static void unobstructed_change_handler(void *context) {
  layout_layers(layer_get_unobstructed_bounds(window_get_root_layer(window)));
}

//Display Time
static void display_time(const struct tm *time) {
  //Hour Texts
  static const char *const hour_string[] = {
	"zwölf", "eins","zwei", "drei", "vier", "fünf", "sechs", "sieben", "acht", "neun", "zehn", "elf"
   };

  //Minute Texts
  static const char *const minute_string[] = {
    "\npunkt", "eins\nnach", "zwei\nnach", "drei\nnach", "vier\nnach", "fünf\nnach",
    "sechs\nnach", "sieben\nnach", "acht\nnach", "neun\nnach", "zehn\nnach",
    "elf\nnach", "zwölf\nnach", "dreizehn nach", "vierzehn nach", "viertel nach",
    "sechzehn nach", "siebzehn nach", "achtzehn nach", "neunzehn nach", "\nzwanzig nach",
    "neun\nvor\nhalb", "acht\nvor\nhalb", "sieben\nvor\nhalb", "sechs\nvor\nhalb", "\nfünf vor halb",
    "vier\nvor\nhalb", "drei\nvor\nhalb", "zwei\nvor\nhalb", "eins\nvor\nhalb", "\nhalb",
    "eins\nnach\nhalb", "zwei\nnach\nhalb", "drei\nnach\nhalb", "vier\nnach\nhalb", "\nfünf nach halb",
    "sechs\nnach\nhalb", "sieben\nnach\nhalb", "acht\nnach\nhalb", "neun\nnach\nhalb", "\nzwanzig vor",
    "neunzehn vor", "achtzehn vor", "siebzehn vor", "sechzehn vor", "drei-\nviertel",
    "vierzehn vor", "dreizehn vor", "zwölf\nvor", "elf\nvor", "zehn\nvor",
    "neun\nvor", "acht\nvor", "sieben\nvor", "sechs\nvor", "fünf\nvor",
    "vier\nvor", "drei\nvor", "zwei\nvor", "eins\nvor", "kurz vor"
  };

  //Day of week texts
  static const char *const day_string[] = {
    "so", "mo", "di", "mi", "do", "fr", "sa"
  };
  
  // Set Time
  const int hour	= time->tm_hour;
  int       min		= time->tm_min;
  const int mday	= time->tm_mday; //day of the month
  const int wday	= time->tm_wday; //day of week (0=sunday, 1=monday, etc.)

  //Fuzzy mode, e. g. say "fünf nach drei" when it's actually already 15:07.
  if (key_indicator_fuzzy) {
	static const int delta[] = {
		0,		// 0    5
		-1,		// 1    6
		-2,		// 2    7
		2,		// 3    8
		1,		// 4    9
	};
	min += delta[min%5];
  }
  
  // Configure the minute layers
  layer_set_hidden(text_layer_get_layer(minuteLayer_3lines), true);
  layer_set_hidden(text_layer_get_layer(minuteLayer_2longlines), true);
  layer_set_hidden(text_layer_get_layer(minuteLayer_2biglines), true);  
  
  if ((20 <= min && min <= 29) ||
      (31 <= min && min <= 40)) {
        layer_set_hidden(text_layer_get_layer(minuteLayer_3lines), false);
  } else if (min == 13 ||
             min == 14 ||
             (16 <= min && min <= 19) ||
             (41 <= min && min <= 44) ||
             min == 46 ||
             min == 47) {
      layer_set_hidden(text_layer_get_layer(minuteLayer_2longlines), false);
  } else if ((0 <= min && min <= 12) ||
             min == 15 ||
             min == 30 ||
             min == 45 ||
             (48 <= min && min <= 60)) {
      layer_set_hidden(text_layer_get_layer(minuteLayer_2biglines), false);
  }
  
  static char staticTimeText[20+1] = ""; // Needs to be static because it's used by the system later.
  staticTimeText[0] = '\0';
  strcat(staticTimeText , minute_string[min]);
  
  //Override with Special minute texts
  if (key_indicator_text_nrw && min == 45) {
    strcpy(staticTimeText , "viertel vor");
  }
  if (key_indicator_text_wien && min == 15) {
    strcpy(staticTimeText , "\nviertel"); //HINT: also update hour +1!
  }
  
  text_layer_set_text(minuteLayer_3lines, staticTimeText);
  text_layer_set_text(minuteLayer_2longlines, staticTimeText);
  text_layer_set_text(minuteLayer_2biglines, staticTimeText);
  
  // Hour Text
  static char staticHourText[10+1] = ""; // Needs to be static because it's used by the system later.
  if (min <= 20) {
    if (min == 15 && key_indicator_text_wien) { //Override with Special minute texts
      strcpy(staticHourText, hour_string[(hour + 1) % 12]);
    } else {
      strcpy(staticHourText , hour_string[hour % 12]);
    }
  } else {
    strcpy(staticHourText , hour_string[(hour + 1) % 12]);
  }
  
  text_layer_set_text(hourLayer, staticHourText);
  
  // Weekday
  static char staticDateText[5+1];
  snprintf(staticDateText, sizeof(staticDateText), "%s %i", day_string[wday], mday);
  text_layer_set_text(dateLayer, staticDateText);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  display_time(tick_time);
}

/*
  ###################################
  ######## Generic Functions ########
  ###################################
*/

//If a Key is changing, call process_tuple
static void in_received_handler(DictionaryIterator *iter, void *context) {
  for(Tuple *t=dict_read_first(iter); t!=NULL; t=dict_read_next(iter)) {
      process_tuple(t);
  }

  set_theme();
  const time_t now = time(NULL);
  display_time(localtime(&now));
}

static void window_load(Window *window) {
  //Key
  app_message_register_inbox_received(in_received_handler); //register key receiving
  app_message_open(512, 512); //Key buffer in- and outbound
  
  //Load value from storage, if storage is empty load default value
  key_indicator_fuzzy =	    persist_exists(KEY_FUZZY) 	    ? persist_read_bool(KEY_FUZZY) 	    : key_indicator_fuzzy;
  key_indicator_bluetooth =	persist_exists(KEY_BLUETOOTH)	? persist_read_bool(KEY_BLUETOOTH) 	: key_indicator_bluetooth;
  key_indicator_vibe =		persist_exists(KEY_VIBE) 		? persist_read_bool(KEY_VIBE) 		: key_indicator_vibe;
  key_indicator_batt_img =	persist_exists(KEY_BATT_IMG) 	? persist_read_bool(KEY_BATT_IMG) 	: key_indicator_batt_img;
  key_indicator_text_nrw =	persist_exists(KEY_TEXT_NRW) 	? persist_read_bool(KEY_TEXT_NRW) 	: key_indicator_text_nrw;
  key_indicator_text_wien =	persist_exists(KEY_TEXT_WIEN) 	? persist_read_bool(KEY_TEXT_WIEN)	: key_indicator_text_wien;
  key_indicator_date =		persist_exists(KEY_DATE) 		? persist_read_bool(KEY_DATE) 		: key_indicator_date;
  key_indicator_theme =	    persist_exists(KEY_THEME) 		? persist_read_bool(KEY_THEME) 		: key_indicator_theme;
  
  //Get the actual usable bounds of this watch's screen (varies by platform:
  //144x168 on Aplite/Basalt/Diorite/Flint, 180x180 round on Chalk, 200x228 on
  //Emery, ...) instead of assuming the old fixed 144x168 Aplite resolution.
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_unobstructed_bounds(window_layer);

  //Create all layers, then position them for the current bounds.
  load_text_layers();
  load_battery_layers();
  load_bluetooth_layers();
  layout_layers(bounds);

  //Re-run the layout whenever the unobstructed area changes, e.g. Timeline
  //Quick View sliding in/out, so the face adapts to the available space.
  UnobstructedAreaHandlers unobstructed_handlers = {
    .did_change = unobstructed_change_handler
  };
  unobstructed_area_service_subscribe(unobstructed_handlers, NULL);

  set_theme();
  const time_t now = time(NULL);
  display_time(localtime(&now));
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void window_unload(Window *window) {
  text_layer_destroy(minuteLayer_3lines);
  text_layer_destroy(minuteLayer_2longlines);
  text_layer_destroy(minuteLayer_2biglines);
  text_layer_destroy(hourLayer);
  text_layer_destroy(dateLayer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true); //Push to Display
}

static void deinit(void) {
  window_destroy(window);
  tick_timer_service_unsubscribe();
  unobstructed_area_service_unsubscribe();
  
  //Bluetooth
  bluetooth_connection_service_unsubscribe();
  layer_remove_from_parent(bitmap_layer_get_layer(bluetooth_layer));
  bitmap_layer_destroy(bluetooth_layer);
  gbitmap_destroy(bluetooth_connected_image);
  gbitmap_destroy(bluetooth_disconnected_image);
  
  //Battery
  battery_state_service_unsubscribe();
  layer_remove_from_parent(bitmap_layer_get_layer(battery_fill_layer));
  bitmap_layer_destroy(battery_fill_layer);
  gbitmap_destroy(battery_image);
  layer_remove_from_parent(bitmap_layer_get_layer(battery_image_layer));
  bitmap_layer_destroy(battery_image_layer);
    
  //Save keys to persistent storage
  persist_write_bool(KEY_FUZZY,     key_indicator_fuzzy);
  persist_write_bool(KEY_BLUETOOTH, key_indicator_bluetooth);
  persist_write_bool(KEY_VIBE,      key_indicator_vibe);
  persist_write_bool(KEY_BATT_IMG,  key_indicator_batt_img);
  persist_write_bool(KEY_TEXT_NRW,  key_indicator_text_nrw);
  persist_write_bool(KEY_TEXT_WIEN, key_indicator_text_wien);
  persist_write_bool(KEY_DATE,      key_indicator_date);
  persist_write_int (KEY_THEME,     key_indicator_theme);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}