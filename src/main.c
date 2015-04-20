#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_week_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  unsigned int itemp=temp;
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "2147483648";
  static char dbuffer[] = "                                    ";
  static char wbuffer[] = "                ";

  snprintf(buffer, sizeof(buffer), "%u", itemp);
  strftime(dbuffer, sizeof(dbuffer), "%d %B %Y", tick_time);
  strftime(wbuffer, sizeof(wbuffer), "%A", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_date_layer, dbuffer);
  text_layer_set_text(s_week_layer, wbuffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 120, 144, 30));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorClear);

  s_date_layer = text_layer_create(GRect(0, 147, 144, 21));
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorClear);

  s_week_layer = text_layer_create(GRect(0, 0, 144, 21));
  text_layer_set_background_color(s_week_layer, GColorBlack);
  text_layer_set_text_color(s_week_layer, GColorClear);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);

  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);

  text_layer_set_font(s_week_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_week_layer, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_week_layer));
  
  // Locale
  char *sys_locale = setlocale(LC_ALL, "");
  //text_layer_set_text(s_week_layer, sys_locale);
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_week_layer);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  window_set_background_color(s_main_window, GColorBlack);  
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
