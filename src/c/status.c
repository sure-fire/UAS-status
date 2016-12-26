#include <pebble.h>
#include "status.h"

static Window *s_window;
static char state;
static TextLayer *s_txt_layer;

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}

static void prv_init(void) {
  prv_init_app_message();

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_stack_push(s_window, false);
}

static void prv_init_app_message() {
  // Initialize AppMessage
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_txt_layer = text_layer_create(GRect(0, (bounds.size.h/2)-45, bounds.size.w, 90));
  text_layer_set_background_color(s_txt_layer, GColorClear);
  text_layer_set_text_color(s_txt_layer, GColorBlack);
  text_layer_set_font(s_txt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_txt_layer, GTextAlignmentCenter);
  text_layer_set_text(s_txt_layer, "");
  layer_add_child(window_layer, text_layer_get_layer(s_txt_layer));
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_APP_READY);
  if (ready_tuple) {
    if(launch_reason() == APP_LAUNCH_USER || launch_reason() == APP_LAUNCH_QUICK_LAUNCH) {
      text_layer_set_text(s_txt_layer, "Checking...");
      prv_lockitron_toggle_state();
    } else {
      // Application was just installed, or configured
      // text_layer_set_text(s_txt_layer, "App Installed");
      prv_exit_application(NULL);
    }
    return;
  }

  Tuple *state_tuple = dict_find(iter, MESSAGE_KEY_STATE);
  if (state_tuple) {
    state = state_tuple->value->cstring[0];
    
    Tuple *since_tuple = dict_find(iter, MESSAGE_KEY_SINCE);
    char * since = since_tuple->value->cstring;
    APP_LOG(APP_LOG_LEVEL_INFO, "State: %c", state);
    APP_LOG(APP_LOG_LEVEL_INFO, "Since: %s", since);

    // Display the current lock state
    static char str[50];
    snprintf(str, sizeof(str), "The space has been\n%s since\n%s", prv_status_decode(state),since);
    text_layer_set_text(s_txt_layer, str);

    // Exit the application, after timeout
    prv_exit_delay();
  }
}

static void prv_exit_delay() {
  // Get the system timeout duration
  int timeout = preferred_result_display_duration() * 2;

  // After the timeout, exit the application
  app_timer_register(timeout, prv_exit_application, NULL);
}

static void prv_exit_application(void *data) {
  // App can exit to return directly to their default watchface
  exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);

  // Exit the application by unloading the only window
  window_stack_remove(s_window, false);
}

// Request a state change for the Lockitron (Unlock/Lock)
static void prv_lockitron_toggle_state() {
  DictionaryIterator *out;
  AppMessageResult result = app_message_outbox_begin(&out);
  if (result != APP_MSG_OK) {
    text_layer_set_text(s_txt_layer, "Outbox Failed");
  }

  result = app_message_outbox_send();
  if (result != APP_MSG_OK) {
    text_layer_set_text(s_txt_layer, "Message Failed");
  }
}

static void prv_window_unload(Window *window) {
  window_destroy(s_window);
}

static void prv_deinit(void) {
  // Before the application terminates, setup the AppGlance
  app_glance_reload(prv_update_app_glance, &state);
}

// Create the AppGlance displayed in the system launcher
static void prv_update_app_glance(AppGlanceReloadSession *session, size_t limit, void *context) {
  // Check we haven't exceeded system limit of AppGlance's
  if (limit < 1) return;

  // Retrieve the current Lockitron state from context
  char * old_state = context;

  // Generate a friendly message for the current Lockitron state
  char str[50];
  snprintf(str, sizeof(str), "%s", prv_status_decode(*old_state));
  APP_LOG(APP_LOG_LEVEL_INFO, "Glance string: %s", str);

  // Create the AppGlanceSlice (no icon, no expiry)
  AppGlanceSlice entry = (AppGlanceSlice) {
    .layout = {
      .subtitle_template_string = str
    },
    .expiration_time = time(NULL) + 300        // make the Glance text disappear after 5 minutes
  };

  // Add the slice, and check the result
  const AppGlanceResult result = app_glance_add_slice(session, entry);
  if (result != APP_GLANCE_RESULT_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "AppGlance Error: %d", result);
  }
}

// Generate a string to display the Lockitron state
const char * prv_status_decode(char state) {
  switch(state) {
    case '-':
      return "CLOSED";
    case '+':
      return "OPEN";
    default:
      return "UNKNOWN";
  }
}