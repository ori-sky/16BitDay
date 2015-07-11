#include <pebble.h>

static Window *mainWindow;
static TextLayer *timeLayer;

static void Tick(struct tm *time, TimeUnits delta) {
	
}

static void MainWindowLoad(Window *window) {
	timeLayer = text_layer_create(GRect(0, 55, 144, 50));
	text_layer_set_background_color(timeLayer, GColorClear);
	text_layer_set_text_color(timeLayer, GColorBlack);
	text_layer_set_text(timeLayer, "00:00");

	text_layer_set_font(timeLayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(timeLayer, GTextAlignmentCenter);

	layer_add_child(window_get_root_layer(window), text_layer_get_layer(timeLayer));
}

static void MainWindowUnload(Window *window) {
	text_layer_destroy(timeLayer);
}

static void Init() {
	mainWindow = window_create();

	window_set_window_handlers(mainWindow, (WindowHandlers) {
		.load   = MainWindowLoad,
		.unload = MainWindowUnload
	});

	window_stack_push(mainWindow, true);

	tick_timer_service_subscribe(MINUTE_UNIT, Tick);
}

static void Destroy() {
	window_destroy(mainWindow);
}

int main(void) {
	Init();
	app_event_loop();
	Destroy();
}
