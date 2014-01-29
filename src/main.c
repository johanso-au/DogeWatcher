// Works covered by CC BY-NC-SA 4.0

#include <pebble.h>
 
Window* window;
TextLayer *time_layer;
TextLayer *doge_price_layer;
TextLayer *usdk_price_layer;

static GBitmap *background_img;
static BitmapLayer* bg_layer;

char buffer[] = "00:00";

enum {
  PRICE_DOGE_KEY,     
  PRICE_USDK_KEY,      
};

static void out_sent_handler(DictionaryIterator *sent, void *context) {
	// outgoing message was delivered
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to send AppMessage to Pebble");
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
	Tuple *price_doge_tuple = dict_find(iter, PRICE_DOGE_KEY);
	Tuple *price_usdk_tuple = dict_find(iter, PRICE_USDK_KEY);

	if (price_doge_tuple) {
		text_layer_set_text(doge_price_layer, price_doge_tuple->value->cstring);
	}
	if (price_usdk_tuple) {
		text_layer_set_text(usdk_price_layer, price_usdk_tuple->value->cstring);
	}
}

void in_dropped_handler(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "incoming message from Pebble dropped");
}

void update() {
	app_message_outbox_send();	
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
        //Format the buffer string using tick_time as the time source
        strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
        
        //Change the TextLayer text to show the new time!
        text_layer_set_text(time_layer, buffer);
	
		psleep(500); // 500ms sleep to wait for JS to be ready.
		update();	
}


void window_load(Window *window)
{
        //Background Layer
		background_img = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG);
        bg_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
        bitmap_layer_set_background_color(bg_layer, GColorBlack);
        bitmap_layer_set_bitmap(bg_layer, background_img);
        layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bg_layer));
	
		//Time layer
        time_layer = text_layer_create(GRect(0, 0, 144, 62));
        text_layer_set_background_color(time_layer, GColorClear);
        text_layer_set_text_color(time_layer, GColorWhite);
        text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
        text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
        layer_add_child(window_get_root_layer(window), (Layer*) time_layer);
			
		// doge_price_layer
		doge_price_layer = text_layer_create(GRect(0, 65, 144, 50));
		text_layer_set_text_color(doge_price_layer, GColorWhite);
		text_layer_set_background_color(doge_price_layer, GColorClear);
		text_layer_set_font(doge_price_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
		text_layer_set_text_alignment(doge_price_layer, GTextAlignmentCenter);
		
		// usdk_price_layer
		usdk_price_layer = text_layer_create(GRect(0, 120, 144, 50));
		text_layer_set_text_color(usdk_price_layer, GColorWhite);
		text_layer_set_background_color(usdk_price_layer, GColorClear);
		text_layer_set_font(usdk_price_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
		text_layer_set_text_alignment(usdk_price_layer, GTextAlignmentCenter);
        
        layer_add_child(window_get_root_layer(window), (Layer*) time_layer);
		layer_add_child(window_get_root_layer(window), (Layer*) doge_price_layer);
		layer_add_child(window_get_root_layer(window), (Layer*) usdk_price_layer);

	
	//Get a time structure so that the face doesn't start blank
        struct tm *t;
        time_t temp;        
        temp = time(NULL);        
        t = localtime(&temp);        
        
        //Manually call the tick handler when the window is loading
        tick_handler(t, MINUTE_UNIT);
		
}
 
void window_unload(Window *window)
{
        //We will safely destroy the Window's elements here!
        text_layer_destroy(time_layer);
        text_layer_destroy(doge_price_layer);
		text_layer_destroy(usdk_price_layer);
        bitmap_layer_destroy(bg_layer);
}
 
static void app_message_init(void) {
	app_message_open(128 /* inbound_size */, 2 /* outbound_size */);
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
}

void init()
{
        app_message_init();
	
		//Initialize the app elements here!
        window = window_create();
        window_set_window_handlers(window, (WindowHandlers) {
                .load = window_load,
                .unload = window_unload,
        });
        
        tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
        
		window_stack_push(window, true);
}
 
void deinit()
{
        //De-initialize elements here to save memory!
        tick_timer_service_unsubscribe();
        
        window_destroy(window);
}
 
int main(void)
{
        init();
        app_event_loop();
        deinit();
}