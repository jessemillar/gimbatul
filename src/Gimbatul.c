#include <toph.h>

Window *t_window;

TextLayer *g_text_time_layer;

BitmapLayer *g_corridor_layer;
GBitmap *g_corridor;

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
	static char buffer[] = "00:00"; // Allocate "long-lived" storage (required by TextLayer)
	// static char buffer[] = "00:00xx"; // The "xx" is required if we want am/pm to display
	strftime(buffer, sizeof(buffer), "%H:%M", tick_time); // Write the time to the buffer in a safe manner
	clock_copy_time_string(buffer, sizeof(buffer)); // Reformat the time to the user's preference
	text_layer_set_text(g_text_time_layer, buffer); // Display the time in the text time layer
}

void populate_clock() // Initially populate the clock so the face doesn't start blank
{
	// Get a time structure so that the face doesn't start blank
	struct tm *t;
	time_t temp;
	temp = time(NULL);
	t = localtime(&temp);

	// Manually call the tick handler when the window is loading
	tick_handler(t, MINUTE_UNIT);
}

void window_load(Window *window)
{
	// Time styling
	g_text_time_layer = text_layer_create(GRect(0, 112, 144, 30));
	text_layer_set_background_color(g_text_time_layer, GColorClear);
	text_layer_set_text_color(g_text_time_layer, GColorBlack);
	text_layer_set_font(g_text_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	text_layer_set_text_alignment(g_text_time_layer, GTextAlignmentCenter);

	// Bitmap styling
	g_corridor = gbitmap_create_with_resource(RESOURCE_ID_CORRIDOR_CONTINUE);
	g_corridor_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(g_corridor_layer, g_corridor);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_corridor_layer));

	// Add text last
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_time_layer)); // Load the text layer

	populate_clock();
}

void window_unload(Window *window)
{
	gbitmap_destroy(g_corridor);
	bitmap_layer_destroy(g_corridor_layer);

	text_layer_destroy(g_text_time_layer);
}

void init()
{
	t_window = window_create();
	window_set_window_handlers(t_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	tick_timer_service_subscribe(SECOND_UNIT, (TickHandler)tick_handler); // Ask for an update every second

	window_set_fullscreen(t_window, true);
	window_stack_push(t_window, true);
}

void deinit()
{
	window_destroy(t_window);
}

int main(void)
{
	init();
	app_event_loop(); // This is apparently necessary even though I don't have a function called this
	deinit();
}