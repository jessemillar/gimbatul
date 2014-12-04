#include <pebble.h>

// Including as .h instead of .c allows for the above #include <pebble.h> to apply to all files and avoids some ugly errors
#include <functions.h>
#include <player.h>

Window *g_window_main;
Window *g_window_menu;
Window *g_window_clock;

BitmapLayer *g_image_layer;
GBitmap *g_image;
Layer *g_health_bars_layer;
GFont *g_font_press_start;
TextLayer *g_level_layer;
TextLayer *g_exp_layer;

int g_clock_life = 2500; // Lifetime in milliseconds
BitmapLayer *g_clock_image_layer;
GBitmap *g_clock_image;
GFont *g_font_alagard;
TextLayer *g_clock_layer;

int g_menu_size = 3;
SimpleMenuLayer *g_menu;
SimpleMenuItem *g_menu_items;

int g_image_count = 4; // I'm too lazy to figure out a better way to do this
uint32_t g_images[4] = {
	RESOURCE_ID_CORRIDOR_CONTINUE,
	RESOURCE_ID_CORRIDOR_DEAD,
	RESOURCE_ID_CORRIDOR_LEFT,
	RESOURCE_ID_CORRIDOR_RIGHT,
	// RESOURCE_ID_ENEMY_SKELLY,
	// RESOURCE_ID_ENEMY_SLUG,
	// RESOURCE_ID_ENEMY_SPIDER,
	// RESOURCE_ID_ENEMY_TROLL,
	// RESOURCE_ID_OTHER_CHEST
};

void draw_health(Layer *me, GContext *ctx)
{
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(22, 145, (int16_t)(99 * (g_player_current_health / g_player_max_health)), 4), 0, 0);
}

void heal_player(int amount)
{
	if (g_player_current_health < g_player_max_health)
	{
		g_player_current_health += amount;

		if (g_player_current_health > g_player_max_health)
		{
			g_player_current_health = g_player_max_health;
		}

		layer_mark_dirty(g_health_bars_layer); // Mark dirty to force a redraw the health bars
	}
}

void hurt_player(int amount)
{
	if (g_player_current_health > 0)
	{
		g_player_current_health -= amount;

		if (g_player_current_health < 0)
		{
			g_player_current_health = 0;
		}

		layer_mark_dirty(g_health_bars_layer); // Mark dirty to force a redraw the health bars
	}
}

void take_step(struct tm *tick_time, TimeUnits units_changed)
{
	gbitmap_destroy(g_image); // Destroy the image before loading a different one to save RAM
	g_image = gbitmap_create_with_resource(g_images[random(g_image_count)]); // Select a random image from the array
	bitmap_layer_set_bitmap(g_image_layer, g_image);

	layer_mark_dirty(bitmap_layer_get_layer(g_image_layer)); // Mark dirty to force a redraw of the image
}

void button_up_handler(ClickRecognizerRef recognizer, void *context)
{
	heal_player(3);
}

void button_down_handler(ClickRecognizerRef recognizer, void *context)
{
	hurt_player(3);
}

void click_config_provider(Window *window)
{	
	window_single_click_subscribe(BUTTON_ID_UP, button_up_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, button_down_handler);
}

void window_main_load(Window *window)
{
	g_image_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	g_image = gbitmap_create_with_resource(RESOURCE_ID_CORRIDOR_CONTINUE); // Initially set the image so we have something to destroy in the step function
	bitmap_layer_set_bitmap(g_image_layer, g_image);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer));

	g_health_bars_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(g_health_bars_layer, draw_health); // Set the drawing context for health bars
	layer_add_child(window_get_root_layer(window), g_health_bars_layer);

	g_font_press_start = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PRESS_START_8));

	g_level_layer = text_layer_create(GRect(20, 155, 45, 10));
	text_layer_set_background_color(g_level_layer, GColorClear);
	text_layer_set_text_color(g_level_layer, GColorBlack);
	text_layer_set_font(g_level_layer, g_font_press_start);
	text_layer_set_text_alignment(g_level_layer, GTextAlignmentLeft);
	text_layer_set_text(g_level_layer, "L7");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_level_layer));

	g_exp_layer = text_layer_create(GRect(33, 155, 90, 10));
	text_layer_set_background_color(g_exp_layer, GColorClear);
	text_layer_set_text_color(g_exp_layer, GColorBlack);
	text_layer_set_font(g_exp_layer, g_font_press_start);
	text_layer_set_text_alignment(g_exp_layer, GTextAlignmentRight);
	text_layer_set_text(g_exp_layer, "EXP 5/52");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_exp_layer));
}

void window_main_unload(Window *window)
{	
	bitmap_layer_destroy(g_image_layer);
	gbitmap_destroy(g_image);
	layer_destroy(g_health_bars_layer);
}

void window_menu_load(Window *window)
{
	// menu_icon_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_1);

	// Although we already defined NUM_FIRST_MENU_ITEMS, you can define an int as such to easily change the order of menu items later
	int num_a_items = 0;

	g_menu_items[num_a_items++] = (SimpleMenuItem)
	{
		// You should give each menu item a title and callback
		.title = "First Item",
		// .callback = menu_select_callback,
	};

	g_menu_items[num_a_items++] = (SimpleMenuItem)
	{
		.title = "Second Item",
		.subtitle = "Here's a subtitle",
		// .callback = menu_select_callback,
	};

	g_menu_items[num_a_items++] = (SimpleMenuItem)
	{
		.title = "Third Item",
		.subtitle = "This has an icon",
		// .callback = menu_select_callback,
		// .icon = menu_icon_image
	};
}

void window_menu_unload(Window *window)
{	
	simple_menu_layer_destroy(g_menu);
}

void populate_clock(struct tm *tick_time, TimeUnits units_changed) // Initially populate the clock so the face doesn't start blank
{
	// Populate the clock
	static char buffer[] = "00:00xxx"; // Allocate "long-lived" storage (required by TextLayer)
	strftime(buffer, sizeof(buffer), "%H:%M", tick_time); // Write the time to the buffer in a safe manner
	clock_copy_time_string(buffer, sizeof(buffer)); // Reformat the time to the user's preference
	text_layer_set_text(g_clock_layer, buffer); // Display the time in the text time layer
}

void hide_clock()
{
	if (window_stack_contains_window(g_window_clock))
	{
		window_stack_remove(g_window_clock, true);
	}
}

void window_clock_load(Window *window)
{
	g_clock_image_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	g_clock_image = gbitmap_create_with_resource(RESOURCE_ID_OTHER_PARCHMENT); // Initially set the image so we have something to destroy in the step function
	bitmap_layer_set_bitmap(g_clock_image_layer, g_clock_image);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_clock_image_layer));

	g_font_alagard = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALAGARD_26));
	g_clock_layer = text_layer_create(GRect(0, 67, 144, 36));
	text_layer_set_background_color(g_clock_layer, GColorClear);
	text_layer_set_text_color(g_clock_layer, GColorBlack);
	text_layer_set_font(g_clock_layer, g_font_alagard);
	text_layer_set_text_alignment(g_clock_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_clock_layer));

	time_t temp = time(NULL);
	populate_clock(localtime(&temp), MINUTE_UNIT); // Manually call the function with a "fake" time

	app_timer_register(g_clock_life, hide_clock, NULL); // Hide the clock after a certain amount of time
}

void window_clock_unload(Window *window)
{	
	text_layer_destroy(g_clock_layer);
	fonts_unload_custom_font(g_font_alagard);
	bitmap_layer_destroy(g_clock_image_layer);
	gbitmap_destroy(g_clock_image);
}

static void tap_handler(AccelAxisType axis, int32_t direction)
{
	// Do I need to check for and prevent data gathered from vibrations...?
	window_stack_push(g_window_clock, true);
}

void init()
{
	g_window_main = window_create();
	window_set_window_handlers(g_window_main, (WindowHandlers) {
		.load = window_main_load,
		.unload = window_main_unload,
	});

	g_window_menu = window_create();
	window_set_window_handlers(g_window_menu, (WindowHandlers) {
		.load = window_menu_load,
		.unload = window_menu_unload,
	});

	g_window_clock = window_create();
	window_set_window_handlers(g_window_clock, (WindowHandlers) {
		.load = window_clock_load,
		.unload = window_clock_unload,
	});

	window_set_click_config_provider(g_window_main, (ClickConfigProvider) click_config_provider);
	tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler)take_step);

	accel_tap_service_subscribe(tap_handler); // Subscribe to taps

	srand(time(NULL)); // Set the random seed to the current time

	// Load persistent values
	g_player_current_health = persist_exists(NUM_PLAYER_CURRENT_HEALTH) ? persist_read_int(NUM_PLAYER_CURRENT_HEALTH) : 100;

	window_set_fullscreen(g_window_main, true); // Make the app fullscreen
	window_stack_push(g_window_main, true);

	window_set_fullscreen(g_window_menu, true); // Make the menu fullscreen

	window_set_fullscreen(g_window_clock, true); // Make the clock fullscreen
}

void deinit()
{
	// Save persistent values
	persist_write_int(NUM_PLAYER_CURRENT_HEALTH, g_player_current_health);

	window_destroy(g_window_main);
}

int main(void)
{
	init();
	app_event_loop(); // This is apparently necessary even though I don't have a function called this
	deinit();
}