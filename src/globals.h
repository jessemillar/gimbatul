extern int g_player_level;
extern int g_player_max_health;
extern int g_player_current_health;
extern int g_player_max_exp;
extern int g_player_current_exp;
extern int g_player_heal_rate;
extern int g_player_attack;

extern int g_enemy_level;
extern int g_enemy_max_health;
extern int g_enemy_current_health;
extern int g_enemy_attack;

extern int g_fight_probability_cap;
extern int g_fight_probability;
extern bool g_in_fight;

extern int g_clock_life;

extern Window *g_window_main;
extern Window *g_window_menu;
extern Window *g_window_clock;

extern BitmapLayer *g_image_layer_main;
extern GBitmap *g_image_main_background;
extern Layer *g_layer_health_bars;
extern GFont *g_font_press_start;
extern TextLayer *g_text_layer_lvl;
extern TextLayer *g_text_layer_exp;
extern BitmapLayer *g_image_layer_help_attack;
extern GBitmap *g_image_help_attack;
extern BitmapLayer *g_image_layer_help_menu;
extern GBitmap *g_image_help_menu;
extern BitmapLayer *g_image_layer_help_run;
extern GBitmap *g_image_help_run;

extern SimpleMenuLayer *g_layer_menu;
extern SimpleMenuItem *g_layer_menu_items;

extern BitmapLayer *g_image_layer_clock;
extern GBitmap *g_image_clock_background;
extern GFont *g_font_alagard_16;
extern GFont *g_font_alagard_32;
extern TextLayer *g_text_layer_time;
extern TextLayer *g_text_layer_date;
extern BitmapLayer *g_image_layer_clock_bluetooth;
extern GBitmap *g_image_clock_bluetooth;
extern BitmapLayer *g_image_layer_clock_battery;
extern GBitmap *g_image_clock_battery;