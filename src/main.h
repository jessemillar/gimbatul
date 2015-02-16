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

static Window *g_window_main;
static Window *g_window_menu;
static Window *g_window_clock;

static BitmapLayer *g_image_layer_main;
static GBitmap *g_image_main_background;
static Layer *g_layer_health_bars;
static GFont *g_font_press_start;
static TextLayer *g_text_layer_lvl;
static TextLayer *g_text_layer_exp;
static BitmapLayer *g_image_layer_help_attack;
static GBitmap *g_image_help_attack;
static BitmapLayer *g_image_layer_help_menu;
static GBitmap *g_image_help_menu;
static BitmapLayer *g_image_layer_help_run;
static GBitmap *g_image_help_run;

static SimpleMenuLayer *g_layer_menu;
static SimpleMenuItem *g_layer_menu_items;

static BitmapLayer *g_image_layer_clock;
static GBitmap *g_image_clock_background;
static GFont *g_font_alagard_16;
static GFont *g_font_alagard_32;
static TextLayer *g_text_layer_time;
static TextLayer *g_text_layer_date;
static BitmapLayer *g_image_layer_clock_bluetooth;
static GBitmap *g_image_clock_bluetooth;
static BitmapLayer *g_image_layer_clock_battery;
static GBitmap *g_image_clock_battery;

static int random(int cap);

static void draw_health(Layer *me, GContext *ctx);

static void heal_player(int amount);

static void hurt_player(int amount);

static void hurt_enemy(int amount);

static void step();

static void take_step(struct tm *tick_time, TimeUnits units_changed);

static void show_stats();

static void button_up_handler(ClickRecognizerRef recognizer, void *context);

static void button_down_handler(ClickRecognizerRef recognizer, void *context);

static void click_config_provider(Window *window);

static void window_main_load(Window *window);

static void window_main_unload(Window *window);

static void window_menu_load(Window *window);

static void window_menu_unload(Window *window);

static void tap_handler(AccelAxisType axis, int32_t direction);

static void populate_clock(struct tm *tick_time, TimeUnits units_changed);

static void hide_clock();

static void window_clock_load(Window *window);

static void window_clock_unload(Window *window);

static void init();

static void deinit();

int main(void);