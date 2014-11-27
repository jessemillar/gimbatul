#include <pebble.h>

Window *g_window;

int g_player_max_health = 100;
int g_player_current_health = 100;

BitmapLayer *g_image_layer;
GBitmap *g_image;

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

void draw_health()
{
	graphics_context_set_stroke_color(g_image, GColorBlack);
	graphics_draw_line(g_image, GPoint(0, 160), GPoint(144, 160));
}

void step(struct tm *tick_time, TimeUnits units_changed)
{
	// Clear the current image by destroying the layer and bitmap and readding to the window
	bitmap_layer_destroy(g_image_layer);
	gbitmap_destroy(g_image);

	// Similar but not the same as in the window loader
	g_image_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	g_image = gbitmap_create_with_resource(g_images[rand() % g_image_count]);

	bitmap_layer_set_bitmap(g_image_layer, g_image);

	layer_add_child(window_get_root_layer(g_window), bitmap_layer_get_layer(g_image_layer));
}

void window_load(Window *window)
{
	g_image_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	g_image = gbitmap_create_with_resource(RESOURCE_ID_CORRIDOR_CONTINUE);

	bitmap_layer_set_bitmap(g_image_layer, g_image);

	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer));
}

void window_unload(Window *window)
{	
	bitmap_layer_destroy(g_image_layer);
	gbitmap_destroy(g_image);
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