#include "display_time.h"

#include "layout.h"
#include "settings.h"

void display_time(const struct tm *time) {
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

  void(*update_time)(const char* minutes, const char* hours, const char* date);

  switch (min) {
  default:
    update_time = update_time_text_2_big_lines;
    break;
  case 20:
  case 21:
  case 22:
  case 23:
  case 24:
  case 25:
  case 26:
  case 27:
  case 28:
  case 29:
  case 31:
  case 32:
  case 33:
  case 34:
  case 35:
  case 36:
  case 37:
  case 38:
  case 39:
  case 40:
    update_time = update_time_text_3_minute_lines;
    break;
  case 13:
  case 14:
  case 16:
  case 17:
  case 18:
  case 19:
  case 41:
  case 42:
  case 43:
  case 44:
  case 46:
  case 47:
    update_time = update_time_text_2_long_lines;
    break;
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
  
  // Weekday
  static char staticDateText[5+1];
  snprintf(staticDateText, sizeof(staticDateText), "%s %i", day_string[wday], mday);

  update_time(staticTimeText, staticHourText, staticDateText);
}
