#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "mini-printf.h"
	
#define MY_UUID { 0xE5, 0xE0, 0x5B, 0xC0, 0x12, 0x00, 0x41, 0x68, 0x82, 0x7E, 0x94, 0x04, 0x23, 0x5C, 0xCC, 0x7B }
PBL_APP_INFO_SIMPLE(MY_UUID, "Fudge Roller", "JCD", 1 /* App version */);


Window window;
long seed;
TextLayer textLayer;
int numberOfRolls;
int totalRollValue;

long get_seconds() 
{
    PblTm t;
    get_time(&t);

	// Convert time to seconds since epoch.
   	return t.tm_sec						// start seconds 
		+ t.tm_min*60  					// add minutes 
		+ t.tm_hour*3600  				// add hours 
		+ t.tm_yday*86400  				// add days 
		+ (t.tm_year-70)*31536000  		// add years since 1970 
		+ ((t.tm_year-69)/4)*86400  	// add a day after leap years, starting in 1973 
		- ((t.tm_year-1)/100)*86400 	// remove a leap day every 100 years, starting in 2001 
		+ ((t.tm_year+299)/400)*86400; 	// add a leap day back every 400 years, starting in 2001 
}

char *itoa(int num)
{
	static char buff[20];
	int i = 0, temp_num = num, length = 0;
	char *string = buff;

	if(num >= 0) {
		while(temp_num) {
			temp_num /= 10;
			length++;
		}

		for(i = 0; i < length; i++) {
		 	buff[(length-1)-i] = '0' + (num % 10);
			num /= 10;
		}
		buff[i] = '\0';
	}
	else
		return "Unsupported Number";

	return string;
}

int random(int max) 
{
	seed = (((seed * 214013L + 2531011L) >> 16) & 32767);

	return ((seed % max) + 1);
}

// Modify these common button handlers

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

}

void formatResults(char *string, int results[]) 
{	
	char *t = "Roll#%d\n%d %d %d %d\n\t= %d\n\n(%d)";
	int total = results[0] + results[1] + results[2] + results[3];
	
	numberOfRolls++;
	totalRollValue += total;

	mini_snprintf(string, 50, t, numberOfRolls, results[0], results[1], results[2], results[3], total, totalRollValue); 
}

void getResults(int results[])
{
	int i;
	for(i = 0; i < 4; i++) {
		results[i] = random(3) - 2;
	}
	
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  	char *text = "";
	int results[4];
	
	getResults(results);
	formatResults(text, results);
  text_layer_set_text(&textLayer, text);
}


void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

}


// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}


// Standard app initialisation

void handle_init(AppContextRef ctx) {
  (void)ctx;

 // srand(time(NULL));
	
  window_init(&window, "Button App");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&textLayer, window.layer.frame);
  text_layer_set_text(&textLayer, "Fudge\nRoller");
  text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
  layer_add_child(&window.layer, &textLayer.layer);

  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
