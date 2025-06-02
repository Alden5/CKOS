#ifndef DISPLAY_API_H
#define DISPLAY_API_H

#include <stdint.h>
#include <stdbool.h>
#include "../BSP/bsp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// Display command types per architecture documentation
typedef enum {
    DISPLAY_CMD_ACTIVATE_SCREEN = 0,
    DISPLAY_CMD_UPDATE_TEXT_ELEMENT,
    DISPLAY_CMD_START_ANIMATION,
    DISPLAY_CMD_UPDATE_STATUS_BAR,
    DISPLAY_CMD_SET_THEME,
    DISPLAY_CMD_GAME_SPIN_THE_WHEEL_START_ANIM,
    DISPLAY_CMD_GAME_SPIN_THE_WHEEL_SHOW_RESULT,
    DISPLAY_CMD_UPDATE_AGENT_MOOD,
    DISPLAY_CMD_UPDATE_LOCK_STATUS,
    DISPLAY_CMD_COUNT
} DisplayCommandID;

// Screen IDs per architecture documentation  
typedef enum {
    SCREEN_ID_WELCOME = 0,
    SCREEN_ID_TIMEZONE_SETUP,
    SCREEN_ID_TIME_SETUP,
    SCREEN_ID_MAIN_MENU,
    SCREEN_ID_LOCK_SETUP,
    SCREEN_ID_AGENT_SELECTION,
    SCREEN_ID_AGENT_INTERACTION,
    SCREEN_ID_LOCK_STATUS,
    SCREEN_ID_LOCK_CONFIG_CUSTOM,
    SCREEN_ID_LOCK_CONFIG_KEYHOLDER,
    SCREEN_ID_PIN_ENTRY,
    SCREEN_ID_GAME_SPIN_WHEEL,
    SCREEN_ID_VERIFICATION,
    SCREEN_ID_SETTINGS,
    SCREEN_ID_ERROR,
    SCREEN_ID_COUNT
} ScreenID;

// Element IDs for screen updates
typedef enum {
    ELEMENT_ID_TITLE = 0,
    ELEMENT_ID_MAIN_TEXT,
    ELEMENT_ID_STATUS,
    ELEMENT_ID_INSTRUCTIONS,
    ELEMENT_ID_COUNT
} ElementID;

// Theme IDs per architecture documentation
typedef enum {
    THEME_ID_DEFAULT = 0,
    THEME_ID_AGENT_ROOKIE,
    THEME_ID_AGENT_VETERAN,
    THEME_ID_AGENT_WARDEN,
    THEME_ID_COUNT
} ThemeID;

// Display command structure for message passing
typedef struct {
    DisplayCommandID id;
    union {
        struct {
            ScreenID screen_id;
            void* data_ptr;
        } activate_screen;
        
        struct {
            ScreenID screen_id;
            ElementID element_id;
            char text[64];
        } update_text;
        
        struct {
            uint8_t animation_id;
            uint16_t x, y;
            uint8_t loop_count;
        } start_animation;
        
        struct {
            char battery_text[16];
            char time_text[16];
            bool locked;
        } status_bar;
        
        struct {
            ThemeID theme_id;
        } set_theme;
        
        struct {
            int target_segment;
            uint16_t duration_ms;
        } spin_wheel_anim;
        
        struct {
            const char* result_line1;
            const char* result_line2;
        } spin_wheel_result;
        
        struct {
            int agent_id;
            int mood_image_id;
            float affection;
            float strictness;
            float satisfaction;
            float trust;
        } agent_mood;
        
        struct {
            uint32_t time_remaining;
            const char* status_text;
        } lock_status;
    } data;
} DisplayCommand;

// Screen data structures
typedef struct {
    int menu_selection;
    int max_items;
    int visible_start;  // First visible item in scrolling menu
    int max_visible;    // Maximum items to show at once
    const char** options;
} MenuScreenData;

typedef struct {
    int timezone_offset;
    bool dst_active;
} TimezoneScreenData;

typedef struct {
    char time_string[32];
} TimeScreenData;

typedef struct {
    int selection;
    int visible_start;  // First visible item in scrolling menu
    int max_visible;    // Maximum items to show at once
    const char** settings_options;
    int max_settings;
} SettingsScreenData;

// Agent System Screen Data (from Agent_System_Design.txt)
typedef struct {
    int selected_agent;  // 0=Rookie, 1=Veteran, 2=Warden
    const char* agent_descriptions[3];
} AgentSelectionScreenData;

typedef struct {
    int selected_agent;          // Current agent personality
    const char* agent_dialog;    // Current agent message
    const char** interaction_options; // Available user options
    int num_options;
    int selected_option;
    // Agent mood visualization (4D mood system)
    float mood_affection;        // 0.0 - 1.0
    float mood_strictness;       // 0.0 - 1.0
    float mood_satisfaction;     // 0.0 - 1.0
    float mood_trust;           // 0.0 - 1.0
    int mood_image_id;          // Which mood image to display
} AgentInteractionScreenData;

// Lock System Screen Data (from Lock_System_Design.txt)
typedef struct {
    int lock_type;              // 0=Agent, 1=Custom, 2=Keyholder
    uint32_t duration_hours;
    uint32_t duration_minutes;
    bool games_enabled;
    int selected_game;
} CustomLockConfigScreenData;

typedef struct {
    bool is_remote_mode;        // false=basic PIN, true=remote
    char identicon_pattern[25]; // 5x5 pattern for remote mode
    const char* connection_status;
} KeyholderConfigScreenData;

typedef struct {
    char entered_pin[8];        // Current PIN entry
    int pin_length;
    int cursor_pos;
    bool show_digits;           // true=show digits, false=show asterisks
    const char* prompt_text;
} PinEntryScreenData;

typedef struct {
    int lock_type;              // Current lock type
    const char* lock_type_name;
    uint32_t time_remaining_seconds;
    uint32_t session_time_seconds;
    bool is_break_active;
    const char* agent_name;     // For agent locks
    float battery_percentage;
    bool wifi_connected;
    bool bluetooth_connected;
} LockStatusScreenData;

// Game System Screen Data (from Game_System_Design.txt)
#define MAX_WHEEL_SEGMENTS 8
typedef struct {
    const char* segment_text;   // Text like "+5 MIN"
} WheelSegmentData;

typedef struct {
    int num_segments;
    WheelSegmentData segments[MAX_WHEEL_SEGMENTS];
    int highlighted_segment;    // Current/final selection
    bool is_spinning;          // Animation state
    const char* result_text_line1; // e.g., "You won!"
    const char* result_text_line2; // e.g., "-30 Minutes"
} SpinWheelScreenData;

// Verification Screen Data
typedef struct {
    const char* device_serial;
    const char* current_datetime_local;
    const char* current_datetime_utc;
    const char* session_locked_time;
    char identicon_pattern[25]; // 5x5 pattern
    bool show_identicon;
} VerificationScreenData;

// Display Task API (following architecture documentation)
void display_task_init(void);
void display_task_update(void);
bool display_task_send_command(DisplayCommand* cmd);

// Internal screen handlers (called by Display_Task)
void display_screen_welcome(void);
void display_screen_timezone_setup(TimezoneScreenData* data);
void display_screen_time_setup(TimeScreenData* data);
void display_screen_main_menu(MenuScreenData* data);
void display_screen_settings(SettingsScreenData* data);
void display_screen_agent_selection(AgentSelectionScreenData* data);
void display_screen_agent_interaction(AgentInteractionScreenData* data);
void display_screen_lock_status(LockStatusScreenData* data);
void display_screen_custom_lock_config(CustomLockConfigScreenData* data);
void display_screen_keyholder_config(KeyholderConfigScreenData* data);
void display_screen_pin_entry(PinEntryScreenData* data);
void display_screen_spin_wheel(SpinWheelScreenData* data);
void display_screen_verification(VerificationScreenData* data);

// UI Component functions (reusable per documentation)
void ui_component_draw_menu_selection(int x, int y, int width, int height, 
                                     const char* text, bool selected);
void ui_component_draw_scrolling_menu(const char** options, int num_options, 
                                     int selection, int visible_start, int max_visible);
void ui_component_draw_navigation_arrows(int selection, int max_items);
void ui_component_draw_title_bar(const char* title);
void ui_component_draw_status_bar(const char* battery, const char* time, bool locked);
void ui_component_draw_input_hints(const char* hints);

// Agent System UI Components (from Agent_System_Design.txt)
void ui_component_draw_agent_mood_display(int x, int y, int agent_id, int mood_image_id, 
                                         float affection, float strictness, 
                                         float satisfaction, float trust);
void ui_component_draw_agent_dialog_box(int x, int y, int width, int height, 
                                       const char* dialog_text);
void ui_component_draw_agent_selection_card(int x, int y, int width, int height,
                                           const char* agent_name, const char* description, 
                                           bool selected);

// Lock System UI Components (from Lock_System_Design.txt)
void ui_component_draw_pin_pad(int x, int y, int cursor_pos, const char* entered_pin, 
                              bool show_digits);
void ui_component_draw_time_duration_selector(int x, int y, uint32_t hours, 
                                             uint32_t minutes, int focus_field);
void ui_component_draw_lock_status_display(int x, int y, const char* lock_type,
                                          uint32_t time_remaining, uint32_t session_time);
void ui_component_draw_identicon(int x, int y, const char* pattern);

// Game System UI Components (from Game_System_Design.txt)
void ui_component_draw_wheel_segments(int center_x, int center_y, int radius,
                                     WheelSegmentData* segments, int num_segments,
                                     int highlighted_segment);
void ui_component_draw_game_result_display(int x, int y, const char* line1, 
                                          const char* line2);

// Enhanced UI Components
void ui_component_draw_progress_bar(int x, int y, int width, int height,
                                   float percentage, const char* label);
void ui_component_draw_battery_indicator(int x, int y, float percentage, bool charging);
void ui_component_draw_connection_status(int x, int y, bool wifi, bool bluetooth);
void ui_component_draw_confirmation_dialog(const char* title, const char* message,
                                           const char* ok_text, const char* cancel_text,
                                           bool ok_selected);

// =============================================================================
// NEW STANDARDIZED UI FRAMEWORK
// =============================================================================

// Standard screen layout components
void ui_draw_standard_title_bar(const char* title, float battery_percent);
void ui_draw_button_hints(const char* primary_action, const char* secondary_action);
void ui_draw_centered_content(const char* line1, const char* line2, const char* line3);
void ui_draw_menu_list(const char* items[], int count, int selected, int visible_start, int max_visible);

// Specialized screen layouts
void ui_draw_lock_status_display(int hours, int minutes, const char* agent_name, const char* mood);
void ui_draw_agent_selection_list(const char* agents[], const char* descriptions[], int count, int selected);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_API_H