#include <pebble.h>

#include "globals.h"
#include "combat.h"

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