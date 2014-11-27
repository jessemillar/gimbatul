#include <pebble.h> // Nab the Pebble libraries

// GBitmap *test_bitmap

/*
void t_make_text_layer(int x, int y, int width, int height, GColor color, GColor background, const char *res_id, GTextAlignment alignment)
{
	TextLayer temp = text_layer_create(GRect(x, y, width, height)); // Break into its own function

	text_layer_set_text_color(temp, color);
	text_layer_set_background_color(temp, background);
	text_layer_set_font(temp, fonts_get_system_font(res_id));
	text_layer_set_text_alignment(temp, alignment);

	return temp;
}
*/

void t_set_text(TextLayer *text_layer, const char *text)
{
	text_layer_set_text(text_layer, text);
}

void t_add_text_layer(TextLayer *text_layer, Window *window_layer)
{
	layer_add_child(window_get_root_layer(window_layer), text_layer_get_layer(text_layer));
}

/*
GBitmap t_make_bitmap()
{
	GBitmap temp = // stuff

	return temp;
}
*/