#include <pebble.h>

#define SECONDS_IN_DAY (86400)

static Window *mainWindow;
static TextLayer *timeLayer;

static void MainWindowLoad(Window *window) {
	timeLayer = text_layer_create(GRect(0, 55, 144, 100));
	text_layer_set_background_color(timeLayer, GColorClear);
	text_layer_set_text_color(timeLayer, GColorWhite);

	text_layer_set_font(timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(timeLayer, GTextAlignmentCenter);

	layer_add_child(window_get_root_layer(window), text_layer_get_layer(timeLayer));
}

static void MainWindowUnload(Window *window) {
	text_layer_destroy(timeLayer);
}

static void Tick(struct tm *time, TimeUnits delta) {
	static char buffer[] = "00000000\n00000000";
	unsigned int seconds = time->tm_sec + time->tm_min * 60 + time->tm_hour * 60 * 60;
	float tickles = seconds * 65536.0f / SECONDS_IN_DAY;
	unsigned short currentTickle = tickles;
	float toNext = currentTickle + 1 - tickles;

	if(toNext > 0.0f) {
		psleep(toNext * 1000.0f);
		++currentTickle;
	}

	for(unsigned char i = 0; i < 16; ++i) {
		buffer[i + i / 8] = currentTickle / (1 << (15 - i)) % 2
		                  ? '1' : '0';
	}
	text_layer_set_text(timeLayer, buffer);
}

static void Init() {
	mainWindow = window_create();

	window_set_window_handlers(mainWindow, (WindowHandlers) {
		.load   = MainWindowLoad,
		.unload = MainWindowUnload
	});

	window_stack_push(mainWindow, true);

	window_set_background_color(mainWindow, GColorBlack);

	tick_timer_service_subscribe(SECOND_UNIT, Tick);
}

static void Destroy() {
	window_destroy(mainWindow);
}

int main(void) {
	Init();
	app_event_loop();
	Destroy();
}
