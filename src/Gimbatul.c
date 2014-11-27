#include <pebble.h>

Window *g_window;

MenuLayer *g_menu_layer;

BitmapLayer *g_image_layer;
GBitmap *g_image;

void step(struct tm *tick_time, TimeUnits units_changed)
{
	// Do random step generation stuff
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

	tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler)step); // Ask for an update every minute

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