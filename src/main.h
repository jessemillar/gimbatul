// These aren't values, they're "keys"
#define G_PLAYER_CURRENT_LEVEL_PKEY 9999
#define G_PLAYER_MAX_HEALTH_PKEY 9998
#define G_PLAYER_CURRENT_HEALTH_PKEY 9997
#define G_PLAYER_MAX_EXP_PKEY 9996
#define G_PLAYER_CURRENT_EXP_PKEY 9995
#define G_PLAYER_HEAL_RATE_PKEY 9994
#define G_PLAYER_ATTACK_PKEY 9993

// These are the default values
#define G_PLAYER_CURRENT_LEVEL_DEFAULT 1
#define G_PLAYER_MAX_HEALTH_DEFAULT 100
#define G_PLAYER_CURRENT_HEALTH_DEFAULT 100
#define G_PLAYER_MAX_EXP_DEFAULT 5
#define G_PLAYER_CURRENT_EXP_DEFAULT 0
#define G_PLAYER_HEAL_RATE_DEFAULT 1
#define G_PLAYER_ATTACK_DEFAULT 3

// Window *g_window_main;
// Window *g_window_menu;
// Window *g_window_clock;

// BitmapLayer *g_image_layer_main;
// GBitmap *g_image_main_background;
// Layer *g_layer_health_bars;
// GFont *g_font_press_start;
// TextLayer *g_text_layer_lvl;
// TextLayer *g_text_layer_exp;
// BitmapLayer *g_image_layer_help_attack;
// GBitmap *g_image_help_attack;
// BitmapLayer *g_image_layer_help_menu;
// GBitmap *g_image_help_menu;
// BitmapLayer *g_image_layer_help_run;
// GBitmap *g_image_help_run;

// SimpleMenuLayer *g_layer_menu;
// SimpleMenuItem *g_layer_menu_items;

// BitmapLayer *g_image_layer_clock;
// GBitmap *g_image_clock_background;
// GFont *g_font_alagard_16;
// GFont *g_font_alagard_32;
// TextLayer *g_text_layer_time;
// TextLayer *g_text_layer_date;
// BitmapLayer *g_image_layer_clock_bluetooth;
// GBitmap *g_image_clock_bluetooth;
// BitmapLayer *g_image_layer_clock_battery;
// GBitmap *g_image_clock_battery;

void draw_health(Layer *me, GContext *ctx);

void heal_player(int amount);

void hurt_player(int amount);

void hurt_enemy(int amount);

void step();

void take_step(struct tm *tick_time, TimeUnits units_changed);

void show_stats();

void button_up_handler(ClickRecognizerRef recognizer, void *context);

void button_down_handler(ClickRecognizerRef recognizer, void *context);

void click_config_provider(Window *window);

void window_main_load(Window *window);

void window_main_unload(Window *window);

void window_menu_load(Window *window);

void window_menu_unload(Window *window);

void tap_handler(AccelAxisType axis, int32_t direction);

void populate_clock(struct tm *tick_time, TimeUnits units_changed);

void hide_clock();

void window_clock_load(Window *window);

void window_clock_unload(Window *window);

void init();

void deinit();

int main(void);