#include <pebble.h>

Window *g_window;

MenuLayer *g_menu_layer;

BitmapLayer *g_image_layer;

int g_player_state = 0; // 0 is exploring and 1 is fighting
int g_current_image = 0;
int g_image_count = 4;

GBitmap *g_image_corridor_continue;
GBitmap *g_image_corridor_dead;
GBitmap *g_image_corridor_left;
GBitmap *g_image_corridor_right;

GBitmap *g_image_enemy_skelly;
GBitmap *g_image_enemy_slug;
GBitmap *g_image_enemy_spider;
GBitmap *g_image_enemy_troll;

void step(struct tm *tick_time, TimeUnits units_changed)
{
	g_player_state = rand() % 2;
	g_current_image = rand() % g_image_count;

	if (g_player_state == 0)
	{
		if (g_current_image == 0)
		{
			bitmap_layer_set_bitmap(g_image_layer, g_image_corridor_continue);
		}
		else if (g_current_image == 1)
		{
			bitmap_layer_set_bitmap(g_image_layer, g_image_corridor_dead);
		}
		else if (g_current_image == 2)
		{
			bitmap_layer_set_bitmap(g_image_layer, g_image_corridor_left);
		}
		else if (g_current_image == 3)
		{
			bitmap_layer_set_bitmap(g_image_layer, g_image_corridor_right);
		}
	}
	else
	{
		if (g_current_image == 0)
		{
			bitmap_layer_set_bitmap(g_image_layer, g_image_enemy_skelly);
		}
		else if (g_current_image == 1)
		{
			bitmap_layer_set_bitmap(g_image_layer, g_image_enemy_slug);
		}
		else if (g_current_image == 2)
		{
			bitmap_layer_set_bitmap(g_image_layer, g_image_enemy_spider);
		}
		else if (g_current_image == 3)
		{
			bitmap_layer_set_bitmap(g_image_layer, g_image_enemy_troll);
		}
	}
}

void window_load(Window *window)
{
	g_image_corridor_continue = gbitmap_create_with_resource(RESOURCE_ID_CORRIDOR_CONTINUE);
	g_image_corridor_dead = gbitmap_create_with_resource(RESOURCE_ID_CORRIDOR_DEAD);
	g_image_corridor_left = gbitmap_create_with_resource(RESOURCE_ID_CORRIDOR_LEFT);
	g_image_corridor_right = gbitmap_create_with_resource(RESOURCE_ID_CORRIDOR_RIGHT);

	g_image_enemy_skelly = gbitmap_create_with_resource(RESOURCE_ID_ENEMY_SKELLY);
	g_image_enemy_slug = gbitmap_create_with_resource(RESOURCE_ID_ENEMY_SLUG);
	g_image_enemy_spider = gbitmap_create_with_resource(RESOURCE_ID_ENEMY_SPIDER);
	g_image_enemy_troll = gbitmap_create_with_resource(RESOURCE_ID_ENEMY_TROLL);

	g_image_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(g_image_layer, g_image_corridor_continue);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_image_layer));
}

void window_unload(Window *window)
{	
	menu_layer_destroy(g_menu_layer);

	bitmap_layer_destroy(g_image_layer);

	gbitmap_destroy(g_image_corridor_continue);
	gbitmap_destroy(g_image_corridor_dead);
	gbitmap_destroy(g_image_corridor_left);
	gbitmap_destroy(g_image_corridor_right);

	gbitmap_destroy(g_image_enemy_skelly);
	gbitmap_destroy(g_image_enemy_slug);
	gbitmap_destroy(g_image_enemy_spider);
	gbitmap_destroy(g_image_enemy_troll);
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