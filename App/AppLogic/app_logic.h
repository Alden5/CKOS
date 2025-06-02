#ifndef APP_LOGIC_H
#define APP_LOGIC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../BSP/bsp_api.h"
#include "../Display/display_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// Application states from documentation
typedef enum {
    STATE_FIRST_TIME_SETUP = 0,
    STATE_WELCOME,
    STATE_TIMEZONE_SETUP,
    STATE_TIME_SETUP,
    STATE_MENU,
    STATE_LOCK_SETUP,
    STATE_LOCK_ACTIVE,
    STATE_AGENT_INTERACTION,
    STATE_UNLOCK_SEQUENCE,
    STATE_SETTINGS,
    STATE_ERROR,
    STATE_IDLE,
    STATE_COUNT
} AppState;

// Lock types from documentation
typedef enum {
    LOCK_TYPE_AGENT = 0,
    LOCK_TYPE_CUSTOM,
    LOCK_TYPE_KEYHOLDER,
    LOCK_TYPE_COUNT
} LockType;

// Agent personalities from documentation
typedef enum {
    AGENT_ROOKIE = 0,    // Beginner mode
    AGENT_VETERAN,       // Advanced mode  
    AGENT_WARDEN,        // Permanent/Strict mode
    AGENT_COUNT
} AgentPersonality;

// Application state structure
typedef struct {
    AppState current_state;
    AppState previous_state;
    
    // First-time setup flags
    bool first_boot;
    bool timezone_configured;
    bool time_configured;
    
    // Time management (UTC-based as per documentation)
    int32_t timezone_offset_hours;  // Offset from UTC
    bool dst_active;                // Daylight saving time flag
    uint64_t utc_time_seconds;      // UTC time since epoch
    
    // Lock system state
    LockType active_lock_type;
    AgentPersonality selected_agent;
    uint32_t lock_duration_seconds;
    uint64_t lock_start_time;
    bool device_locked;
    
    // Agent system state (simplified for now)
    float agent_mood_affection;     // 0.0 - 1.0
    float agent_mood_strictness;    // 0.0 - 1.0
    float agent_mood_satisfaction;  // 0.0 - 1.0  
    float agent_mood_trust;         // 0.0 - 1.0
    
    // UI state
    int menu_selection;
    int max_menu_items;
    int menu_visible_start;
    int max_visible_menu_items;
    int settings_selection;
    int max_settings_items;
    int settings_visible_start;
    int max_visible_settings_items;
    
    // Input handling
    uint32_t last_button_time;
    bsp_button_id_t last_button;
} AppLogicState;

extern AppLogicState g_app_state;

// Core application functions
void app_logic_init(void);
void app_logic_update(void);
void app_logic_process_button_event(const bsp_button_event_t* event);

// State management
void app_logic_change_state(AppState new_state);
const char* app_logic_get_state_name(AppState state);

// Display functions (now using Display_Task)
void app_logic_send_display_command(DisplayCommandID cmd_id, void* data);
void app_logic_activate_screen(ScreenID screen_id, void* data);
void app_logic_update_settings_menu(void);

// Utility functions
void app_logic_get_local_time_string(char* buffer, size_t buffer_size);
uint64_t app_logic_get_current_utc_time(void);

// Agent System functions
void app_logic_show_agent_interaction_screen(void);
void app_logic_handle_agent_interaction_input(const bsp_button_event_t* event);
void app_logic_start_agent_lock(void);
void app_logic_process_agent_request(void);

// Lock System functions
void app_logic_show_lock_setup_screen(void);
void app_logic_handle_lock_setup_input(const bsp_button_event_t* event);
void app_logic_show_lock_status_screen(void);
void app_logic_handle_lock_active_input(const bsp_button_event_t* event);

// Game System functions
void app_logic_start_spin_wheel_game(void);
void app_logic_spin_wheel(void);

#ifdef __cplusplus
}
#endif

#endif // APP_LOGIC_H