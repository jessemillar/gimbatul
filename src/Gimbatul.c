#include <pebble.h>

Window *g_window;

double g_player_max_health = 100;
double g_player_current_health = 66;
double g_player_attack = 10;
double g_player_defence = 10;

BitmapLayer *g_image_layer;
GBitmap *g_image;
Layer *g_health_bars_layer;

int g_image_count = 8;
uint32_t g_images[8] = {
	RESOURCE_ID_CORRIDOR_CONTINUE,
	RESOURCE_ID_CORRIDOR_DEAD,
	RESOURCE_ID_CORRIDOR_LEFT,
	RESOURCE_ID_CORRIDOR_RIGHT,
	RESOURCE_ID_ENEMY_SKELLY,
	RESOURCE_ID_ENEMY_SLUG,
	RESOURCE_ID_ENEMY_SPIDER,
	RESOURCE_ID_ENEMY_TROLL
};

void draw_health(Layer *me, GContext *ctx)
{
	graphics_context_set_stroke_color(ctx, GColorWhite);
	// graphics_draw_line(ctx, GPoint(0, 0), GPoint(144, 0)); // Enemy health bar background
	graphics_draw_line(ctx, GPoint(0, 167), GPoint(144, 167)); // Player health bar background

	graphics_context_set_stroke_color(ctx, GColorBlack);
	// graphics_draw_line(ctx, GPoint(0, 0), GPoint(144, 0)); // Enemy health bar background
	graphics_draw_line(ctx, GPoint(0, 167), GPoint((int16_t)(144 * (g_player_current_health / g_player_max_health)), 167)); // Player health bar background
}

void step(struct tm *tick_time, TimeUnits units_changed)
{
	// Clear the current image by destroying the layer and bitmap and readding to the window
	// bitmap_layer_destroy(g_image_layer);
	gbitmap_destroy(g_image);

	// Similar but not the same as in the window loader
	g_image_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	g_image = gbitmap_create_with_resource(g_images[rand() % g_image_count]);

	bitmap_layer_set_bitmap(g_image_layer, g_image);

	// layer_add_child(window_get_root_layer(g_window), bitmap_layer_get_layer(g_image_layer));

	layer_mark_dirty(g_health_bars_layer); // Mark dirty to force a redraw using the function we've defined for drawing
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
}

void window_unload(Window *window)
{	
	bitmap_layer_destroy(g_image_layer);
	gbitmap_destroy(g_image);
	layer_destroy(g_health_bars_layer);
}

void init()
{
	g_window = window_create();
	window_set_window_handlers(g_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	tick_timer_service_subscribe(SECOND_UNIT, (TickHandler)step);

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