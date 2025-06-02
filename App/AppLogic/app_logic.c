// CKOS Application Logic Implementation
// Unified implementation that runs on both STM32 and simulator through BSP abstraction

#include "app_logic.h"
#include "../Display/display_api.h"
#include <stdio.h>
#include <string.h>
#ifndef TEST_MODE
#include <time.h>
#endif

// Global application state
AppLogicState g_app_state = {0};

// Forward declarations for helper functions
static void update_menu_scroll_window(void);
static void update_settings_scroll_window(void);

// Main menu options from documentation
static const char* main_menu_options[] = {
    "Agent Lock",
    "Custom Lock", 
    "Keyholder Lock",
    "Game Mode",
    "Emergency Release",
    "Lock History",
    "Settings",
    "About Device"
};
#define MAIN_MENU_COUNT (sizeof(main_menu_options) / sizeof(main_menu_options[0]))

// Settings menu options
static const char* settings_options[] = {
    "Display Brightness",
    "Display Contrast", 
    "Display Sleep Timeout",
    "Sound Settings",
    "Vibration Settings",
    "WiFi Configuration",
    "Bluetooth Settings",
    "Timezone Settings",
    "Language Settings",
    "Power Management",
    "Security Settings",
    "Factory Reset",
    "Firmware Update",
    "Diagnostics",
    "About Device"
};
#define SETTINGS_COUNT (sizeof(settings_options) / sizeof(settings_options[0]))

void app_logic_init(void) {
    printf("Initializing application logic...\n");
    
    memset(&g_app_state, 0, sizeof(g_app_state));
    
    // Initialize state as per documentation - first boot setup
    g_app_state.first_boot = true;
    g_app_state.timezone_configured = false;
    g_app_state.time_configured = false;
    
    // Initialize timezone settings (default to UTC)
    g_app_state.timezone_offset_hours = 0;
    g_app_state.dst_active = false;
    
    // Get current UTC time as starting point
#ifdef TEST_MODE
    g_app_state.utc_time_seconds = 1640995200; // Fixed time for testing (Jan 1, 2022)
#else
    g_app_state.utc_time_seconds = bsp_get_utc_time_seconds();
#endif
    
    // Initialize agent mood values as per documentation
    // Rookie agent defaults (high affection, low strictness)
    g_app_state.selected_agent = AGENT_ROOKIE;
    g_app_state.agent_mood_affection = 0.9f;
    g_app_state.agent_mood_strictness = 0.3f;
    g_app_state.agent_mood_satisfaction = 0.7f;
    g_app_state.agent_mood_trust = 0.5f;
    
    // Initialize lock system
    g_app_state.device_locked = false;
    g_app_state.active_lock_type = LOCK_TYPE_AGENT;
    
    // Initialize UI state
    g_app_state.menu_selection = 0;
    g_app_state.max_menu_items = MAIN_MENU_COUNT;
    g_app_state.menu_visible_start = 0;
    g_app_state.max_visible_menu_items = 4;  // Show 4 menu items at once
    g_app_state.settings_selection = 0;
    g_app_state.max_settings_items = SETTINGS_COUNT;
    g_app_state.settings_visible_start = 0;
    g_app_state.max_visible_settings_items = 4;  // Show 4 settings at once
    
    // Set initial state - Welcome screen on first power-up per documentation
    g_app_state.current_state = STATE_WELCOME;
    g_app_state.previous_state = STATE_WELCOME;
    
    // Initialize Display_Task per architecture documentation
    display_task_init();
    app_logic_activate_screen(SCREEN_ID_WELCOME, NULL);
    
    printf("Application logic initialized\n");
    printf("Initial state: %s\n", app_logic_get_state_name(g_app_state.current_state));
}

void app_logic_update(void) {
    // Update simulation time
#ifndef TEST_MODE
    static uint64_t last_update = 0;
    uint64_t current_time = bsp_get_utc_time_seconds();
    
    if (current_time != last_update) {
        g_app_state.utc_time_seconds = current_time;
        last_update = current_time;
    }
#endif
    
    // Update Display_Task per architecture documentation
    display_task_update();
}

void app_logic_process_button_event(const bsp_button_event_t* event) {
    if (!event || !event->pressed) {
        return; // Only process button press events, not releases
    }
    
    // Button debouncing - ignore rapid button presses
    #define BUTTON_DEBOUNCE_MS 150
    if (g_app_state.last_button == event->button && 
        (event->timestamp - g_app_state.last_button_time) < BUTTON_DEBOUNCE_MS) {
        return; // Ignore this button press (too soon after last one)
    }
    
    printf("Button pressed: %d in state %s\n", event->button, 
           app_logic_get_state_name(g_app_state.current_state));
    
    // Update last button info for debouncing/repeat detection
    g_app_state.last_button_time = event->timestamp;
    g_app_state.last_button = event->button;
    
    // Handle state-specific input
    switch (g_app_state.current_state) {
        case STATE_WELCOME:
            // Any button press proceeds to timezone setup on first boot
            if (g_app_state.first_boot) {
                app_logic_change_state(STATE_TIMEZONE_SETUP);
            } else {
                app_logic_change_state(STATE_MENU);
            }
            break;
            
        case STATE_TIMEZONE_SETUP: {
            TimezoneScreenData data = {
                .timezone_offset = g_app_state.timezone_offset_hours,
                .dst_active = g_app_state.dst_active
            };
            
            switch (event->button) {
                case BSP_BUTTON_LEFT:
                    g_app_state.timezone_offset_hours--;
                    if (g_app_state.timezone_offset_hours < -12) {
                        g_app_state.timezone_offset_hours = -12;
                    }
                    data.timezone_offset = g_app_state.timezone_offset_hours;
                    app_logic_activate_screen(SCREEN_ID_TIMEZONE_SETUP, &data);
                    break;
                    
                case BSP_BUTTON_RIGHT:
                    g_app_state.timezone_offset_hours++;
                    if (g_app_state.timezone_offset_hours > 12) {
                        g_app_state.timezone_offset_hours = 12;
                    }
                    data.timezone_offset = g_app_state.timezone_offset_hours;
                    app_logic_activate_screen(SCREEN_ID_TIMEZONE_SETUP, &data);
                    break;
                    
                case BSP_BUTTON_UP:
                case BSP_BUTTON_DOWN:
                    g_app_state.dst_active = !g_app_state.dst_active;
                    data.dst_active = g_app_state.dst_active;
                    app_logic_activate_screen(SCREEN_ID_TIMEZONE_SETUP, &data);
                    break;
                    
                case BSP_BUTTON_A:
                    g_app_state.timezone_configured = true;
                    if (g_app_state.first_boot && !g_app_state.time_configured) {
                        app_logic_change_state(STATE_TIME_SETUP);
                    } else {
                        MenuScreenData data = {
                            .menu_selection = g_app_state.menu_selection,
                            .max_items = g_app_state.max_menu_items,
                            .visible_start = g_app_state.menu_visible_start,
                            .max_visible = g_app_state.max_visible_menu_items,
                            .options = main_menu_options
                        };
                        app_logic_activate_screen(SCREEN_ID_MAIN_MENU, &data);
                        app_logic_change_state(STATE_MENU);
                    }
                    break;
                    
                case BSP_BUTTON_B:
                    // Skip timezone setup - use default UTC
                    if (g_app_state.first_boot && !g_app_state.time_configured) {
                        app_logic_change_state(STATE_TIME_SETUP);
                    } else {
                        MenuScreenData data = {
                            .menu_selection = g_app_state.menu_selection,
                            .max_items = g_app_state.max_menu_items,
                            .visible_start = g_app_state.menu_visible_start,
                            .max_visible = g_app_state.max_visible_menu_items,
                            .options = main_menu_options
                        };
                        app_logic_activate_screen(SCREEN_ID_MAIN_MENU, &data);
                        app_logic_change_state(STATE_MENU);
                    }
                    break;
                    
                default:
                    break;
            }
            break;
        }
        
        case STATE_TIME_SETUP: {
            switch (event->button) {
                case BSP_BUTTON_A:
                    g_app_state.time_configured = true;
                    g_app_state.first_boot = false;
                    app_logic_change_state(STATE_MENU);
                    break;
                    
                case BSP_BUTTON_B:
                    // Skip time setup - use system time
                    g_app_state.first_boot = false;
                    app_logic_change_state(STATE_MENU);
                    break;
                    
                default:
                    break;
            }
            break;
        }
        
        case STATE_MENU: {
            switch (event->button) {
                case BSP_BUTTON_DOWN:
                    g_app_state.menu_selection++;
                    if (g_app_state.menu_selection >= g_app_state.max_menu_items) {
                        g_app_state.menu_selection = g_app_state.max_menu_items - 1;
                    }
                    update_menu_scroll_window();
                    break;
                    
                case BSP_BUTTON_UP:
                    if (g_app_state.menu_selection > 0) {
                        g_app_state.menu_selection--;
                    }
                    update_menu_scroll_window();
                    break;
                    
                case BSP_BUTTON_A:
                    // Handle menu selection
                    if (g_app_state.menu_selection == 6) { // Settings
                        app_logic_change_state(STATE_SETTINGS);
                    } else {
                        printf("Feature not yet implemented: %s\n", 
                               main_menu_options[g_app_state.menu_selection]);
                    }
                    break;
                    
                case BSP_BUTTON_B:
                    app_logic_change_state(STATE_WELCOME);
                    break;
                    
                default:
                    break;
            }
            
            MenuScreenData data = {
                .menu_selection = g_app_state.menu_selection,
                .max_items = g_app_state.max_menu_items,
                .visible_start = g_app_state.menu_visible_start,
                .max_visible = g_app_state.max_visible_menu_items,
                .options = main_menu_options
            };
            app_logic_activate_screen(SCREEN_ID_MAIN_MENU, &data);
            break;
        }
        
        case STATE_SETTINGS: {
            switch (event->button) {
                case BSP_BUTTON_DOWN:
                    g_app_state.settings_selection++;
                    if (g_app_state.settings_selection >= g_app_state.max_settings_items) {
                        g_app_state.settings_selection = g_app_state.max_settings_items - 1;
                    }
                    update_settings_scroll_window();
                    break;
                    
                case BSP_BUTTON_UP:
                    if (g_app_state.settings_selection > 0) {
                        g_app_state.settings_selection--;
                    }
                    update_settings_scroll_window();
                    break;
                    
                case BSP_BUTTON_A:
                    if (g_app_state.settings_selection == 14) { // About Device
                        printf("About Device selected\n");
                    }
                    break;
                    
                case BSP_BUTTON_B:
                    app_logic_change_state(STATE_MENU);
                    break;
                    
                default:
                    break;
            }
            
            app_logic_update_settings_menu();
            break;
        }
        
        default:
            // Data should be handled in app_logic_activate_screen
            break;
    }
}

void app_logic_change_state(AppState new_state) {
    if (new_state == g_app_state.current_state) {
        return; // No change needed
    }
    
    printf("State change: %s -> %s\n", 
           app_logic_get_state_name(g_app_state.current_state),
           app_logic_get_state_name(new_state));
    
    g_app_state.previous_state = g_app_state.current_state;
    g_app_state.current_state = new_state;
    
    // Handle state entry actions
    switch (new_state) {
        case STATE_WELCOME:
            app_logic_activate_screen(SCREEN_ID_WELCOME, NULL);
            break;
            
        case STATE_TIMEZONE_SETUP: {
            TimezoneScreenData data = {
                .timezone_offset = g_app_state.timezone_offset_hours,
                .dst_active = g_app_state.dst_active
            };
            app_logic_activate_screen(SCREEN_ID_TIMEZONE_SETUP, &data);
            break;
        }
        
        case STATE_TIME_SETUP: {
            TimeScreenData data = {0};
            app_logic_get_local_time_string(data.time_string, sizeof(data.time_string));
            app_logic_activate_screen(SCREEN_ID_TIME_SETUP, &data);
            break;
        }
        
        case STATE_MENU: {
            // Reset menu selection and ensure scroll window is correct
            g_app_state.menu_selection = 0;
            g_app_state.menu_visible_start = 0;
            update_menu_scroll_window();
            
            MenuScreenData data = {
                .menu_selection = g_app_state.menu_selection,
                .max_items = g_app_state.max_menu_items,
                .visible_start = g_app_state.menu_visible_start,
                .max_visible = g_app_state.max_visible_menu_items,
                .options = main_menu_options
            };
            app_logic_activate_screen(SCREEN_ID_MAIN_MENU, &data);
            break;
        }
        
        case STATE_SETTINGS: {
            // Reset settings selection
            g_app_state.settings_selection = 0;
            g_app_state.settings_visible_start = 0;
            update_settings_scroll_window();
            app_logic_update_settings_menu();
            break;
        }
        
        default:
            break;
    }
}

void app_logic_activate_screen(ScreenID screen_id, void* data) {
    DisplayCommand cmd = {0};
    cmd.id = DISPLAY_CMD_ACTIVATE_SCREEN;
    cmd.data.activate_screen.screen_id = screen_id;
    cmd.data.activate_screen.data_ptr = data;
    
    display_task_send_command(&cmd);
}

void app_logic_update_settings_menu(void) {
    SettingsScreenData data = {
        .selection = g_app_state.settings_selection,
        .visible_start = g_app_state.settings_visible_start,
        .max_visible = g_app_state.max_visible_settings_items,
        .settings_options = settings_options,
        .max_settings = g_app_state.max_settings_items
    };
    app_logic_activate_screen(SCREEN_ID_SETTINGS, &data);
}

void app_logic_get_local_time_string(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return;
    
#ifdef TEST_MODE
    // Fixed time string for testing
    snprintf(buffer, buffer_size, "12:34:56");
#else
    // Convert UTC to local time using BSP
    uint64_t local_time_seconds = g_app_state.utc_time_seconds + 
                                  (g_app_state.timezone_offset_hours * 3600);
    
    time_t local_time = (time_t)local_time_seconds;
    struct tm* tm_info = gmtime(&local_time);
    
    if (tm_info) {
        strftime(buffer, buffer_size, "%H:%M:%S", tm_info);
    } else {
        snprintf(buffer, buffer_size, "00:00:00");
    }
#endif
}

uint64_t app_logic_get_current_utc_time(void) {
    return g_app_state.utc_time_seconds;
}

const char* app_logic_get_state_name(AppState state) {
    switch (state) {
        case STATE_FIRST_TIME_SETUP: return "FIRST_TIME_SETUP";
        case STATE_WELCOME: return "WELCOME";
        case STATE_TIMEZONE_SETUP: return "TIMEZONE_SETUP";
        case STATE_TIME_SETUP: return "TIME_SETUP";
        case STATE_MENU: return "MENU";
        case STATE_LOCK_SETUP: return "LOCK_SETUP";
        case STATE_LOCK_ACTIVE: return "LOCK_ACTIVE";
        case STATE_AGENT_INTERACTION: return "AGENT_INTERACTION";
        case STATE_UNLOCK_SEQUENCE: return "UNLOCK_SEQUENCE";
        case STATE_SETTINGS: return "SETTINGS";
        case STATE_ERROR: return "ERROR";
        case STATE_IDLE: return "IDLE";
        default: return "UNKNOWN";
    }
}

// Helper functions
static void update_menu_scroll_window(void) {
    // Ensure the current selection is visible in the scroll window
    if (g_app_state.menu_selection < g_app_state.menu_visible_start) {
        g_app_state.menu_visible_start = g_app_state.menu_selection;
    } else if (g_app_state.menu_selection >= 
               g_app_state.menu_visible_start + g_app_state.max_visible_menu_items) {
        g_app_state.menu_visible_start = g_app_state.menu_selection - 
                                        g_app_state.max_visible_menu_items + 1;
    }
}

static void update_settings_scroll_window(void) {
    // Ensure the current selection is visible in the scroll window
    if (g_app_state.settings_selection < g_app_state.settings_visible_start) {
        g_app_state.settings_visible_start = g_app_state.settings_selection;
    } else if (g_app_state.settings_selection >= 
               g_app_state.settings_visible_start + g_app_state.max_visible_settings_items) {
        g_app_state.settings_visible_start = g_app_state.settings_selection - 
                                            g_app_state.max_visible_settings_items + 1;
    }
}

// Placeholder implementations for advanced features
void app_logic_show_agent_interaction_screen(void) {
    printf("Agent interaction screen not yet implemented\n");
}

void app_logic_handle_agent_interaction_input(const bsp_button_event_t* event) {
    (void)event;
    printf("Agent interaction input not yet implemented\n");
}

void app_logic_start_agent_lock(void) {
    printf("Agent lock system not yet implemented\n");
}

void app_logic_process_agent_request(void) {
    printf("Agent request processing not yet implemented\n");
}

void app_logic_show_lock_setup_screen(void) {
    printf("Lock setup screen not yet implemented\n");
}

void app_logic_handle_lock_setup_input(const bsp_button_event_t* event) {
    (void)event;
    printf("Lock setup input not yet implemented\n");
}

void app_logic_show_lock_status_screen(void) {
    printf("Lock status screen not yet implemented\n");
}

void app_logic_handle_lock_active_input(const bsp_button_event_t* event) {
    (void)event;
    printf("Lock active input not yet implemented\n");
}

void app_logic_start_spin_wheel_game(void) {
    printf("Spin wheel game not yet implemented\n");
}

void app_logic_spin_wheel(void) {
    printf("Spin wheel action not yet implemented\n");
}

// Additional functionality can be added here as needed
void app_logic_send_display_command(DisplayCommandID cmd_id, void* data) {
    (void)cmd_id;
    (void)data;
    printf("Send display command not yet implemented\n");
}