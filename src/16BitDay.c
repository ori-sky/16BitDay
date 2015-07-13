#include <pebble.h>

#define SECONDS_IN_DAY (86400)

static Window *mainWindow;
static BitmapLayer *backgroundLayer;
static GBitmap *backgroundBitmap;
static TextLayer *timeLayer;

static void MainWindowLoad(Window *window) {
	backgroundBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
	backgroundLayer = bitmap_layer_create(GRect(18, 56, 144, 168));
	bitmap_layer_set_bitmap(backgroundLayer, backgroundBitmap);

	timeLayer = text_layer_create(GRect(0, 0, 144, 100));
	text_layer_set_background_color(timeLayer, GColorClear);
	text_layer_set_text_color(timeLayer, GColorBlack);
	text_layer_set_font(timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(timeLayer, GTextAlignmentCenter);

	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(backgroundLayer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(timeLayer));
}

static void MainWindowUnload(Window *window) {
	text_layer_destroy(timeLayer);
	bitmap_layer_destroy(backgroundLayer);
	gbitmap_destroy(backgroundBitmap);
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
