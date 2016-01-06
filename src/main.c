// Styling Notes
// Made by Ben Chapman-Kish from 2016-01-03 to 2016-01-05
// This watchapp is designed to contain notes that the user
// can set in the configuration page on their phone

#include <pebble.h>

// From font browser, adjust later

#define NUM_FONTS 16
#define NUM_MESSAGES 3

typedef struct {
	char *name;
	char *variant;
	char *res;
} PebbleFont;

static PebbleFont pebble_fonts[] = {
 { .name = "Gothic", .variant = "14", .res = FONT_KEY_GOTHIC_14 },
 { .name = "Gothic", .variant = "14 Bold", .res = FONT_KEY_GOTHIC_14_BOLD },
 { .name = "Gothic", .variant = "18", .res = FONT_KEY_GOTHIC_18 },
 { .name = "Gothic", .variant = "18 Bold", .res = FONT_KEY_GOTHIC_18_BOLD },
 { .name = "Gothic", .variant = "24", .res = FONT_KEY_GOTHIC_24 },
 { .name = "Gothic", .variant = "24 Bold", .res = FONT_KEY_GOTHIC_24_BOLD },
 { .name = "Gothic", .variant = "28", .res = FONT_KEY_GOTHIC_28 },
 { .name = "Gothic", .variant = "28 Bold", .res = FONT_KEY_GOTHIC_28_BOLD },

 { .name = "Bitham", .variant = "30 Black", .res = FONT_KEY_BITHAM_30_BLACK },
 { .name = "Bitham", .variant = "42 Bold", .res = FONT_KEY_BITHAM_42_BOLD },
 { .name = "Bitham", .variant = "42 Light", .res = FONT_KEY_BITHAM_42_LIGHT },

 { .name = "Bitham", .variant = "34 Medium Numbers", .res = FONT_KEY_BITHAM_34_MEDIUM_NUMBERS },
 { .name = "Bitham", .variant = "42 Medium Numbers", .res = FONT_KEY_BITHAM_42_MEDIUM_NUMBERS },

 { .name = "Roboto", .variant = "21 Condensed", .res = FONT_KEY_ROBOTO_CONDENSED_21 },
 { .name = "Roboto", .variant = "49 Bold Subset", .res = FONT_KEY_ROBOTO_BOLD_SUBSET_49 },
 { .name = "Droid",  .variant = "28 Bold", .res = FONT_KEY_DROID_SERIF_28_BOLD }
};

static char *s_messages[] = {
  "0123456789",
  "abcdef ABCDEF",
  "09:42"
};



typedef struct {
	uint32_t id;
	char *title;
	uint32_t *pars;
} Note;

typedef struct {
	uint8_t font;
	char *text;
} Par;

static Par *pars;

static Window *s_help_window;
static TextLayer *s_help_title_layer, *s_help_layer;
static TextLayer *s_paragraph_layers[2];
static ScrollLayer *s_scroll_layer;


static Window *s_main_window, *s_font_window;
static MenuLayer *s_menu_layer;
static TextLayer *s_text_layer, *s_font_name_layer, *s_font_variant_layer, *s_font_size_layer;

/* Store the index of the currently selected font and text message. */
static int s_current_font;
static int s_current_message;

/* Text buffer for the size required to display font and message */
static char s_size_text[256] = "size";

static uint16_t get_num_rows(struct MenuLayer* menu_layer, uint16_t section_index, void *callback_context) {
  return NUM_FONTS;
}

static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  PebbleFont *font_array = (PebbleFont*) callback_context;
  PebbleFont *this_font = &font_array[cell_index->row];

  menu_cell_basic_draw(ctx, cell_layer, this_font->name, this_font->variant, NULL);
}

static void select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  s_current_font = cell_index->row;

  window_stack_push(s_font_window, true);
}

static void main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);

  s_menu_layer = menu_layer_create(layer_get_bounds(window_layer));
  menu_layer_set_callbacks(s_menu_layer, pebble_fonts, (MenuLayerCallbacks) {
    .get_num_rows = get_num_rows,
    .draw_row = draw_row,
    .select_click = select_click
  });
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void main_window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
}

static void show_selected_font_and_message() {
  PebbleFont *font = &pebble_fonts[s_current_font];

  // Update the font and text for the demo message
  text_layer_set_font(s_text_layer, fonts_get_system_font(font->res));
  text_layer_set_text(s_text_layer, s_messages[s_current_message]);

  // Update the font name and font variant at the bottom of the screen
  text_layer_set_text(s_font_name_layer, font->name);
  text_layer_set_text(s_font_variant_layer, font->variant);

  // Update Font Size Layer
  GSize textSize = text_layer_get_content_size(s_text_layer);
  snprintf(s_size_text, sizeof(s_size_text), "H: %d W: %d", textSize.h, textSize.w);

  text_layer_set_text(s_font_size_layer, s_size_text);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_current_message++;
  if ((unsigned)s_current_message >= NUM_MESSAGES) {
    s_current_message = 0;
  }

  show_selected_font_and_message();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_current_font--;
  if (s_current_font < 0) {
    s_current_font = NUM_FONTS - 1;
  }

  MenuIndex idx = menu_layer_get_selected_index(s_menu_layer);
  idx.row = s_current_font;
  menu_layer_set_selected_index(s_menu_layer, idx, MenuRowAlignCenter, false);
  
  show_selected_font_and_message();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_current_font++;
  if ((unsigned)s_current_font >= NUM_FONTS) {
    s_current_font = 0;
  }

  MenuIndex idx = menu_layer_get_selected_index(s_menu_layer);
  idx.row = s_current_font;
  menu_layer_set_selected_index(s_menu_layer, idx, MenuRowAlignCenter, false);
  
  show_selected_font_and_message();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler)up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler)down_click_handler);
}



static void close_help_handler(ClickRecognizerRef recognizer, void *context) {
	window_stack_remove(s_help_window, true);
}

static void help_click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, close_help_handler);
};

static void help_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

	// Initialize the scroll layer
  s_scroll_layer = scroll_layer_create(layer_get_frame(window_layer));
  // This binds the scroll layer to the window so that up and down map to scrolling
  // You may use scroll_layer_set_callbacks to add or override interactivity
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
	// Got this to work thanks to http://redd.it/26t34c
	scroll_layer_set_callbacks(s_scroll_layer, (ScrollLayerCallbacks){
    .click_config_provider = help_click_config_provider
  });
	
	// Initialize the text layer
	s_help_title_layer = text_layer_create(GRect(4, 0, 144-8, 100));
	text_layer_set_text(s_help_title_layer, "Welcome to Cold War Simulator 2K15!");
	text_layer_set_font(s_help_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	s_help_layer = text_layer_create(GRect(4, 40, 144-8, 2000));
	text_layer_set_text(s_help_layer, "Your objective is to defeat your rival faction, \
	referred to in this game as \"them\", either through force or peacefully.\n\n\
	To defeat them by force, nuke them when your power and smarts is at least thrice \
	their power and smarts. The conflict will also end peacefully when the tensions reach zero.\n\n\
	From the main screen, you can press select to view your actions and options. \
	Every action has outcomes, both good and bad, that are based on luck as well \
	as the \"stats\": the values visible on the main screen. \n\n\
	From the main screen, you can also press up to view the turn history \
	and press down to repeat the last action you took. \
	In the history screen, press the select button to switch between \
	viewing the outcome of an action and what turn it happened on.\n\n\
	Good luck and have fun!\n\n\
	© Ben Chapman-Kish 2015");
	text_layer_set_font(s_help_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
	// Trim text layer and scroll content to fit text box
	GSize max_size_1 = text_layer_get_content_size(s_help_layer);
	GSize max_size_2 = text_layer_get_content_size(s_help_title_layer);
  text_layer_set_size(s_help_layer, GSize(max_size_1.w, max_size_1.h + 4));
	text_layer_set_size(s_help_title_layer, GSize(max_size_2.w, max_size_2.h + 4));
  scroll_layer_set_content_size(s_scroll_layer, GSize(144, max_size_1.h + max_size_2.h + 18));
	// Add the layers for display
	scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_help_title_layer));
	scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_help_layer));

  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));
}

static void help_window_unload(Window *window) {
	text_layer_destroy(s_help_layer);
	text_layer_destroy(s_help_title_layer);
  scroll_layer_destroy(s_scroll_layer);
}



static void font_window_load(Window *window) {
  // Define some event handlers for clicks
  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);

  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Arrange the three text layers on top of each other
  window_bounds.size.h -= 40;
  s_text_layer = text_layer_create(window_bounds);

  window_bounds.origin.x = 2;
  window_bounds.size.w -= 4;

  window_bounds.origin.y += window_bounds.size.h;
  window_bounds.size.h = 18;

  s_font_name_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_font_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  window_bounds.origin.y += window_bounds.size.h;
  s_font_variant_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_font_variant_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  window_bounds.origin.y -= 2.0 * window_bounds.size.h;
  s_font_size_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_font_size_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  // Add the child layer to the current window (s_font_window)
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_font_name_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_font_variant_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_font_size_layer));

  // Finally, update the text and font in the layers
  show_selected_font_and_message();
}

static void font_window_unload(Window *window) {
  layer_remove_child_layers(window_get_root_layer(window));

  text_layer_destroy(s_text_layer);
  text_layer_destroy(s_font_name_layer);
  text_layer_destroy(s_font_variant_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  s_font_window = window_create();
  window_set_window_handlers(s_font_window, (WindowHandlers) {
    .load = font_window_load,
    .unload = font_window_unload
  });
	
	int num_pars = 2;
	
	pars = malloc(sizeof(Par) * num_pars);
	pars[0] = (Par) {
		.font = 5,
		.text = "Welcome to my notes app!"
	};
	pars[1] = (Par) {
		.font = 2,
		.text = "It is very cool and useful.\nJust check it out!"
	};
	
	
	Note my_note = {
		.title = "This is my note"
	};
	my_note.pars = (uint32_t*)malloc(sizeof(uint32_t) * num_pars);
	my_note.pars[0] = 0;
	my_note.pars[1] = 1;
	
	Par this_par;
	
	for (int i=0; i<num_pars; i++) {
		this_par = pars[my_note.pars[i]];
		APP_LOG(APP_LOG_LEVEL_DEBUG, "%d %s", this_par.font, this_par.text);
	}
	free(my_note.pars);
	free(pars);

  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_font_window);
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}