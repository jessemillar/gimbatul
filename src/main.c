#include <pebble.h>

#include "globals.h"
#include "combat.h"
#include "tools.h"
#include "main.h"

// I need to do something better with generating these values (and they'll need to be saves on app exit in case you leave halfway through a fight)
int g_enemy_level = 2;
int g_enemy_max_health = 20;
int g_enemy_current_health = 20;
int g_enemy_attack = 2;

int g_fight_probability_cap = 10;
int g_fight_probability = 1; // ...out of g_fight_probability_cap is a fight
bool g_in_fight = false;

int g_clock_life = 2500; // Lifetime in milliseconds

int g_image_count_corridor = 4; // I'm too lazy to figure out a better way to do this
uint32_t g_images_corridor[4] = {
	RESOURCE_ID_CORRIDOR_CONTINUE,
	RESOURCE_ID_CORRIDOR_DEAD,
	RESOURCE_ID_CORRIDOR_LEFT,
	RESOURCE_ID_CORRIDOR_RIGHT
};

int g_image_count_enemies = 4; // I'm too lazy to figure out a better way to do this
uint32_t g_images_enemies[4] = {
	RESOURCE_ID_ENEMY_SKELLY,
	RESOURCE_ID_ENEMY_SLUG,
	RESOURCE_ID_ENEMY_SPIDER,
	RESOURCE_ID_ENEMY_TROLL
};

void draw_health(Layer *me, GContext *ctx)
{
	if (g_player_current_health > 0)
	{
		double temp_player_max = (double)g_player_max_health;
		double temp_player_current = (double)g_player_current_health;

		graphics_context_set_fill_color(ctx, GColorBlack);
		graphics_fill_rect(ctx, GRect(22, 145, (int16_t)(99 * (temp_player_current / temp_player_max)), 4), 0, 0);
	}

	if (g_in_fight) // Draw the monster's health
	{
		double temp_enemy_max = (double)g_enemy_max_health;
		double temp_enemy_current = (double)g_enemy_current_health;

		graphics_context_set_fill_color(ctx, GColorBlack);
		graphics_fill_rect(ctx, GRect(6, 7, (int16_t)(132 * (temp_enemy_current / temp_enemy_max)), 4), 0, 0);
	}
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

		layer_mark_dirty(g_layer_health_bars); // Mark dirty to force a redraw the health bars
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

		if (g_player_current_health == 0) // Kill the player
		{
			gbitmap_destroy(g_image_main_background); // Destroy the image before loading a different one to save RAM
			g_image_main_background = gbitmap_create_with_resource(RESOURCE_ID_OTHER_DEATH);
			bitmap_layer_set_bitmap(g_image_layer_main, g_image_main_background);
		}
		else
		{
			layer_mark_dirty(g_layer_health_bars); // Mark dirty to force a redraw the health bars
		}
	}
}

void hurt_enemy(int amount)
{
	if (g_enemy_current_health > 0)
	{
		g_enemy_current_health -= amount;

		if (g_enemy_current_health < 0)
		{
			g_enemy_current_health = 0;
		}

		layer_mark_dirty(g_layer_health_bars); // Mark dirty to force a redraw the health bars
	}
}

void step()
{
	if (!g_in_fight) // Don't take steps when in a fight
	{
		if (random(g_fight_probability_cap) <= g_fight_probability) // Start a fight
		{
			g_in_fight = true; // End the fight only when the monster dies (or when the player does)

			gbitmap_destroy(g_image_main_background); // Destroy the image before loading a different one to save RAM
			g_image_main_background = gbitmap_create_with_resource(g_images_enemies[random(g_image_count_enemies)]); // Select a random image from the array
			bitmap_layer_set_bitmap(g_image_layer_main, g_image_main_background);

			layer_mark_dirty(bitmap_layer_get_layer(g_image_layer_main)); // Mark dirty to force a redraw of the image

			layer_add_child(window_get_root_layer(g_window_main), bitmap_layer_get_layer(g_image_layer_help_attack));
			layer_add_child(window_get_root_layer(g_window_main), bitmap_layer_get_layer(g_image_layer_help_menu));
			layer_add_child(window_get_root_layer(g_window_main), bitmap_layer_get_layer(g_image_layer_help_run));
		}
		else // Not a fight
		{
			gbitmap_destroy(g_image_main_background); // Destroy the image before loading a different one to save RAM
			g_image_main_background = gbitmap_create_with_resource(g_images_corridor[random(g_image_count_corridor)]); // Select a random image from the array
			bitmap_layer_set_bitmap(g_image_layer_main, g_image_main_background);

			layer_mark_dirty(bitmap_layer_get_layer(g_image_layer_main)); // Mark dirty to force a redraw of the image

			layer_remove_from_parent(bitmap_layer_get_layer(g_image_layer_help_attack));
			layer_add_child(window_get_root_layer(g_window_main), bitmap_layer_get_layer(g_image_layer_help_menu));
			layer_remove_from_parent(bitmap_layer_get_layer(g_image_layer_help_run));
		}
	}
}

void take_step(struct tm *tick_time, TimeUnits units_changed) // I guess the SDK needs these variables
{
	heal_player(g_player_heal_rate); // Heal the player a bit each step even if we're in a fight
	
	step();
}

void show_stats()
{
	// We need space for at least "L99" and the end character provided by snprintf
	char lvl_buffer[4]; // We need "static" so the buffer persists...?
	snprintf(lvl_buffer, sizeof(lvl_buffer), "L%d", g_player_level);
	text_layer_set_text(g_text_layer_lvl, lvl_buffer);

	// We need space for at least "EXP 999/999" and the end character provided by snprintf
	char exp_buffer[12]; // We need "static" so the buffer persists...?
	snprintf(exp_buffer, sizeof(exp_buffer), "EXP %d/%d", g_player_current_exp, g_player_max_exp);
	text_layer_set_text(g_text_layer_exp, exp_buffer);
}

void button_up_handler(ClickRecognizerRef recognizer, void *context)
{
	if (g_in_fight)
	{
		hurt_enemy(g_player_attack);

		if (g_enemy_current_health > 0)
		{
			hurt_player(g_enemy_attack);
		}
		else
		{
			g_in_fight = false; // End the fight

			g_enemy_current_health = g_enemy_max_health; // Reset the enemy

			g_player_current_exp += g_enemy_level;

			if (g_player_current_exp >= g_player_max_exp)
			{
				g_player_current_exp = g_player_current_exp - g_player_max_exp;
				g_player_level++;
			}

			show_stats();

			step();
		}
	}
}

void button_down_handler(ClickRecognizerRef recognizer, void *context)
{
	if (g_in_fight)
	{
		if (random(3) == 1)
		{
			g_in_fight = false; // Run away from the fight

			g_enemy_current_health = g_enemy_max_health; // Reset the enemy

			step();
		}
		else
		{
			hurt_player(g_enemy_attack / 2);
		}
	}
}

void click_config_provider(Window *window)
{	
	window_single_click_subscribe(BUTTON_ID_UP, button_up_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, button_down_handler);
}

void window_main_load(Window *window)
{
	g_image_layer_main = bitmap_layer_create(GRect(0, 0, 144, 168));
	g_image_main_background = gbitmap_create_with_resource(RESOURCE_ID_CORRIDOR_CONTINUE); // Initially set the image so we have something to destroy in the step function
	bitmap_layer_set_bitmap(g_image_layer_main, g_image_main_background);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer_main));

	g_image_layer_help_attack = bitmap_layer_create(GRect(132, 36, 12, 21));
	g_image_help_attack = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HELP_ATTACK);
	bitmap_layer_set_bitmap(g_image_layer_help_attack, g_image_help_attack);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer_help_attack));
	g_image_layer_help_menu = bitmap_layer_create(GRect(132, 79, 12, 20));
	g_image_help_menu = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HELP_MENU);
	bitmap_layer_set_bitmap(g_image_layer_help_menu, g_image_help_menu);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer_help_menu));
	g_image_layer_help_run = bitmap_layer_create(GRect(132, 121, 12, 21));
	g_image_help_run = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HELP_RUN);
	bitmap_layer_set_bitmap(g_image_layer_help_run, g_image_help_run);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer_help_run));

	g_layer_health_bars = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(g_layer_health_bars, draw_health); // Set the drawing context for health bars
	layer_add_child(window_get_root_layer(window), g_layer_health_bars);

	g_font_press_start = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PRESS_START_8));

	g_text_layer_lvl = text_layer_create(GRect(20, 155, 45, 10));
	text_layer_set_background_color(g_text_layer_lvl, GColorClear);
	text_layer_set_text_color(g_text_layer_lvl, GColorBlack);
	text_layer_set_font(g_text_layer_lvl, g_font_press_start);
	text_layer_set_text_alignment(g_text_layer_lvl, GTextAlignmentLeft);	

	g_text_layer_exp = text_layer_create(GRect(33, 155, 90, 10));
	text_layer_set_background_color(g_text_layer_exp, GColorClear);
	text_layer_set_text_color(g_text_layer_exp, GColorBlack);
	text_layer_set_font(g_text_layer_exp, g_font_press_start);
	text_layer_set_text_alignment(g_text_layer_exp, GTextAlignmentRight);

	show_stats();

	layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_layer_lvl));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_layer_exp));
}

void window_main_unload(Window *window)
{	
	bitmap_layer_destroy(g_image_layer_main);
	gbitmap_destroy(g_image_main_background);
	layer_destroy(g_layer_health_bars);
	fonts_unload_custom_font(g_font_press_start);
	text_layer_destroy(g_text_layer_lvl);
	text_layer_destroy(g_text_layer_exp);
	bitmap_layer_destroy(g_image_layer_help_attack);
	gbitmap_destroy(g_image_help_attack);
	bitmap_layer_destroy(g_image_layer_help_menu);
	gbitmap_destroy(g_image_help_menu);
	bitmap_layer_destroy(g_image_layer_help_run);
	gbitmap_destroy(g_image_help_run);
}

void window_menu_load(Window *window)
{
	// menu_icon_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_1);

	// Although we already defined G_PLAYER_FIRST_MENU_ITEMS, you can define an int as such to easily change the order of menu items later
	int num_a_items = 0;

	g_layer_menu_items[num_a_items++] = (SimpleMenuItem)
	{
		// You should give each menu item a title and callback
		.title = "First Item",
		// .callback = menu_select_callback,
	};

	g_layer_menu_items[num_a_items++] = (SimpleMenuItem)
	{
		.title = "Second Item",
		.subtitle = "Here's a subtitle",
		// .callback = menu_select_callback,
	};

	g_layer_menu_items[num_a_items++] = (SimpleMenuItem)
	{
		.title = "Third Item",
		.subtitle = "This has an icon",
		// .callback = menu_select_callback,
		// .icon = menu_icon_image
	};
}

void window_menu_unload(Window *window)
{	
	simple_menu_layer_destroy(g_layer_menu);
}

void tap_handler(AccelAxisType axis, int32_t direction)
{
	if (!window_stack_contains_window(g_window_clock)) // Don't make duplicates of the clock window
	{
		window_stack_push(g_window_clock, true);
	}
}

void populate_clock(struct tm *tick_time, TimeUnits units_changed) // Initially populate the clock so the face doesn't start blank
{
	// We need space for at least "00:00xxx" and a trailing space
	char time_buffer[9];
	strftime(time_buffer, sizeof(time_buffer), "%H:%M", tick_time);
	clock_copy_time_string(time_buffer, sizeof(time_buffer)); // Reformat the time to the user's preference
	text_layer_set_text(g_text_layer_time, time_buffer);

	// We need space for at least "Wed, Dec 25" and a trailing space
	char date_buffer[12];
	strftime(date_buffer, sizeof(date_buffer), "%a, %b %e", tick_time);
	text_layer_set_text(g_text_layer_date, date_buffer);
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
	g_image_layer_clock = bitmap_layer_create(GRect(0, 0, 144, 168));
	g_image_clock_background = gbitmap_create_with_resource(RESOURCE_ID_OTHER_PARCHMENT); // Initially set the image so we have something to destroy in the step function
	bitmap_layer_set_bitmap(g_image_layer_clock, g_image_clock_background);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer_clock));

	g_image_layer_clock_bluetooth = bitmap_layer_create(GRect(6, 11, 11, 13));
	if (bluetooth_connection_service_peek()) // Change the image depending on if we have a Bluetooth connection of not
	{
		g_image_clock_bluetooth = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_ON);
	}
	else
	{
		g_image_clock_bluetooth = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_OFF);
	}
	bitmap_layer_set_bitmap(g_image_layer_clock_bluetooth, g_image_clock_bluetooth);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer_clock_bluetooth));

	g_image_layer_clock_battery = bitmap_layer_create(GRect(118, 11, 16, 9));
	BatteryChargeState battery_temp = battery_state_service_peek();
	if (battery_temp.is_charging) // Change the image depending on the battery status
	{
		g_image_clock_battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGING);
	}
	else if (battery_temp.is_plugged)
	{
		g_image_clock_battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_PLUGGED);
	}
	else if (battery_temp.charge_percent >= 80)
	{
		g_image_clock_battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_4);
	}
	else if (battery_temp.charge_percent >= 60)
	{
		g_image_clock_battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_3);
	}
	else if (battery_temp.charge_percent >= 40)
	{
		g_image_clock_battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_2);
	}
	else if (battery_temp.charge_percent >= 20)
	{
		g_image_clock_battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_1);
	}
	else if (battery_temp.charge_percent < 20)
	{
		g_image_clock_battery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_0);
	}
	bitmap_layer_set_bitmap(g_image_layer_clock_battery, g_image_clock_battery);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer_clock_battery));

	g_font_alagard_32 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALAGARD_32));
	g_text_layer_time = text_layer_create(GRect(0, 56, 144, 36));
	text_layer_set_background_color(g_text_layer_time, GColorClear);
	text_layer_set_text_color(g_text_layer_time, GColorBlack);
	text_layer_set_font(g_text_layer_time, g_font_alagard_32);
	text_layer_set_text_alignment(g_text_layer_time, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_layer_time));

	g_font_alagard_16 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALAGARD_16));
	g_text_layer_date = text_layer_create(GRect(0, 89, 144, 20));
	text_layer_set_background_color(g_text_layer_date, GColorClear);
	text_layer_set_text_color(g_text_layer_date, GColorBlack);
	text_layer_set_font(g_text_layer_date, g_font_alagard_16);
	text_layer_set_text_alignment(g_text_layer_date, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_layer_date));

	time_t time_temp = time(NULL);
	populate_clock(localtime(&time_temp), MINUTE_UNIT); // Manually call the function with a "fake" time

	app_timer_register(g_clock_life, hide_clock, NULL); // Hide the clock after a certain amount of time
}

void window_clock_unload(Window *window)
{	
	text_layer_destroy(g_text_layer_time);
	text_layer_destroy(g_text_layer_date);
	fonts_unload_custom_font(g_font_alagard_32);
	fonts_unload_custom_font(g_font_alagard_16);
	bitmap_layer_destroy(g_image_layer_clock);
	gbitmap_destroy(g_image_clock_background);
	bitmap_layer_destroy(g_image_layer_clock_bluetooth);
	gbitmap_destroy(g_image_clock_bluetooth);
	bitmap_layer_destroy(g_image_layer_clock_battery);
	gbitmap_destroy(g_image_clock_battery);
}

void init()
{
	// Load persistent values before doing anything else
	g_player_level = persist_exists(G_PLAYER_CURRENT_LEVEL_PKEY) ? persist_read_int(G_PLAYER_CURRENT_LEVEL_PKEY) : G_PLAYER_CURRENT_LEVEL_DEFAULT;
	g_player_max_health = persist_exists(G_PLAYER_MAX_HEALTH_PKEY) ? persist_read_int(G_PLAYER_MAX_HEALTH_PKEY) : G_PLAYER_MAX_HEALTH_DEFAULT;
	g_player_current_health = persist_exists(G_PLAYER_CURRENT_HEALTH_PKEY) ? persist_read_int(G_PLAYER_CURRENT_HEALTH_PKEY) : G_PLAYER_CURRENT_HEALTH_DEFAULT;
	g_player_max_exp = persist_exists(G_PLAYER_MAX_EXP_PKEY) ? persist_read_int(G_PLAYER_MAX_EXP_PKEY) : G_PLAYER_MAX_EXP_DEFAULT;
	g_player_current_exp = persist_exists(G_PLAYER_CURRENT_EXP_PKEY) ? persist_read_int(G_PLAYER_CURRENT_EXP_PKEY) : G_PLAYER_CURRENT_EXP_DEFAULT;
	g_player_heal_rate = persist_exists(G_PLAYER_HEAL_RATE_PKEY) ? persist_read_int(G_PLAYER_HEAL_RATE_PKEY) : G_PLAYER_HEAL_RATE_DEFAULT;
	g_player_attack = persist_exists(G_PLAYER_ATTACK_PKEY) ? persist_read_int(G_PLAYER_ATTACK_PKEY) : G_PLAYER_ATTACK_DEFAULT;

	// log_int(g_player_max_health);

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

	window_set_fullscreen(g_window_main, true); // Make the app fullscreen
	window_stack_push(g_window_main, true);
	window_set_fullscreen(g_window_menu, true); // Make the menu fullscreen
	window_set_fullscreen(g_window_clock, true); // Make the clock fullscreen
}

void deinit()
{
	// Save persistent values
	persist_write_int(G_PLAYER_CURRENT_LEVEL_PKEY, g_player_level);
	persist_write_int(G_PLAYER_MAX_HEALTH_PKEY, g_player_max_health);
	persist_write_int(G_PLAYER_CURRENT_HEALTH_PKEY, g_player_current_health);
	persist_write_int(G_PLAYER_MAX_EXP_PKEY, g_player_max_exp);
	persist_write_int(G_PLAYER_CURRENT_EXP_PKEY, g_player_current_exp);
	persist_write_int(G_PLAYER_HEAL_RATE_PKEY, g_player_current_exp);
	persist_write_int(G_PLAYER_ATTACK_PKEY, g_player_attack);

	window_destroy(g_window_main);
	window_destroy(g_window_menu);
	window_destroy(g_window_clock);
}

int main(void)
{
	init();
	app_event_loop(); // This is apparently necessary even though I don't have a function called this
	deinit();
}