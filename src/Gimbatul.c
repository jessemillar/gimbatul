#include <pebble.h>

Window *g_window;

double g_player_max_health = 100;
double g_player_current_health = 75;

BitmapLayer *g_image_layer;
GBitmap *g_image;
Layer *g_health_bars_layer;

TextLayer *g_clock_layer;
GFont g_font_sailor_stitch;

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

void draw_health(Layer *me, GContext *ctx) // Currently draws two lines instead of one square for thickness (ghetto)
{
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_line(ctx, GPoint(0, 166), GPoint(144, 166)); // Player health bar background
	graphics_draw_line(ctx, GPoint(0, 167), GPoint(144, 167)); // Player health bar background

	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_draw_line(ctx, GPoint(0, 166), GPoint((int16_t)(144 * (g_player_current_health / g_player_max_health)), 166)); // Player health bar
	graphics_draw_line(ctx, GPoint(0, 167), GPoint((int16_t)(144 * (g_player_current_health / g_player_max_health)), 167)); // Player health bar
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
		g_player_current_health += amount;

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

	g_image = gbitmap_create_with_resource(g_images[rand() % g_image_count]); // Select a random image from the array
	bitmap_layer_set_bitmap(g_image_layer, g_image);

	layer_mark_dirty(bitmap_layer_get_layer(g_image_layer)); // Mark dirty to force a redraw of the image
}

void window_load(Window *window)
{
	g_image_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	g_image = gbitmap_create_with_resource(RESOURCE_ID_CORRIDOR_CONTINUE);
	bitmap_layer_set_bitmap(g_image_layer, g_image);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer));

	g_health_bars_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(g_health_bars_layer, draw_health); // Set the drawing context for health bars
	layer_add_child(window_get_root_layer(g_window), g_health_bars_layer);

	g_font_sailor_stitch = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALAGARD_15));
	g_clock_layer = text_layer_create(GRect(0, 142, 144, 30));
	text_layer_set_background_color(g_clock_layer, GColorClear);
	text_layer_set_text_color(g_clock_layer, GColorBlack);
	text_layer_set_font(g_clock_layer, g_font_sailor_stitch);
	text_layer_set_text_alignment(g_clock_layer, GTextAlignmentCenter);
	text_layer_set_text(g_clock_layer, "12:24");
	layer_add_child(window_get_root_layer(g_window), text_layer_get_layer(g_clock_layer));
}

void window_unload(Window *window)
{	
	bitmap_layer_destroy(g_image_layer);
	gbitmap_destroy(g_image);
	layer_destroy(g_health_bars_layer);
	text_layer_destroy(g_clock_layer);
	fonts_unload_custom_font(g_font_sailor_stitch);
}

void init()
{
	g_window = window_create();
	window_set_window_handlers(g_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	tick_timer_service_subscribe(SECOND_UNIT, (TickHandler)take_step);

	srand(time(NULL)); // Set the random seed to the current time

	window_set_fullscreen(g_window, true); // Make the app fullscreen
	window_stack_push(g_window, true);
}

void deinit()
{
	window_destroy(g_window);
}

int main(void)
{
	init();
	app_event_loop(); // This is apparently necessary even though I don't have a function called this
	deinit();
}