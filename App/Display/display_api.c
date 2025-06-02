// CKOS Display Implementation  
// Unified display system that runs on both STM32 and simulator through BSP abstraction

#include "display_api.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// Display Task state (per architecture documentation)
static struct {
    ScreenID current_screen;
    ThemeID current_theme;
    DisplayCommand command_queue[16];  // Simplified queue for unified implementation
    int queue_head, queue_tail, queue_count;
    bool initialized;
} display_task_state = {0};

// Current screen data
static MenuScreenData current_menu_data = {0};
static TimezoneScreenData current_timezone_data = {0};
static TimeScreenData current_time_data = {0};
static SettingsScreenData current_settings_data = {0};
static AgentSelectionScreenData current_agent_selection_data = {0};
static AgentInteractionScreenData current_agent_interaction_data = {0};
static LockStatusScreenData current_lock_status_data = {0};
static CustomLockConfigScreenData current_custom_lock_data = {0};
static KeyholderConfigScreenData current_keyholder_data = {0};
static PinEntryScreenData current_pin_entry_data = {0};
static SpinWheelScreenData current_spin_wheel_data = {0};
static VerificationScreenData current_verification_data = {0};

void display_task_init(void) {
    memset(&display_task_state, 0, sizeof(display_task_state));
    display_task_state.current_screen = SCREEN_ID_WELCOME;
    display_task_state.current_theme = THEME_ID_DEFAULT;
    display_task_state.initialized = true;
    
    // BSP display initialization handled by main.cpp
}

void display_task_update(void) {
    if (!display_task_state.initialized) return;
    
    // Process command queue (simplified for unified implementation)
    if (display_task_state.queue_count > 0) {
        DisplayCommand* cmd = &display_task_state.command_queue[display_task_state.queue_head];
        
        switch (cmd->id) {
            case DISPLAY_CMD_ACTIVATE_SCREEN:
                display_task_state.current_screen = cmd->data.activate_screen.screen_id;
                if (cmd->data.activate_screen.data_ptr) {
                    // Copy screen data based on screen type
                    switch (cmd->data.activate_screen.screen_id) {
                        case SCREEN_ID_MAIN_MENU:
                            current_menu_data = *(MenuScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_TIMEZONE_SETUP:
                            current_timezone_data = *(TimezoneScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_TIME_SETUP:
                            current_time_data = *(TimeScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_SETTINGS:
                            current_settings_data = *(SettingsScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_AGENT_SELECTION:
                            current_agent_selection_data = *(AgentSelectionScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_AGENT_INTERACTION:
                            current_agent_interaction_data = *(AgentInteractionScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_LOCK_STATUS:
                            current_lock_status_data = *(LockStatusScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_LOCK_CONFIG_CUSTOM:
                            current_custom_lock_data = *(CustomLockConfigScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_LOCK_CONFIG_KEYHOLDER:
                            current_keyholder_data = *(KeyholderConfigScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_PIN_ENTRY:
                            current_pin_entry_data = *(PinEntryScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_GAME_SPIN_WHEEL:
                            current_spin_wheel_data = *(SpinWheelScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        case SCREEN_ID_VERIFICATION:
                            current_verification_data = *(VerificationScreenData*)cmd->data.activate_screen.data_ptr;
                            break;
                        default:
                            break;
                    }
                }
                break;
                
            case DISPLAY_CMD_SET_THEME:
                display_task_state.current_theme = cmd->data.set_theme.theme_id;
                break;
                
            case DISPLAY_CMD_UPDATE_AGENT_MOOD:
                // Update current agent interaction data with new mood
                current_agent_interaction_data.mood_affection = cmd->data.agent_mood.affection;
                current_agent_interaction_data.mood_strictness = cmd->data.agent_mood.strictness;
                current_agent_interaction_data.mood_satisfaction = cmd->data.agent_mood.satisfaction;
                current_agent_interaction_data.mood_trust = cmd->data.agent_mood.trust;
                current_agent_interaction_data.mood_image_id = cmd->data.agent_mood.mood_image_id;
                break;
                
            case DISPLAY_CMD_UPDATE_LOCK_STATUS:
                // Update lock status display
                current_lock_status_data.time_remaining_seconds = cmd->data.lock_status.time_remaining;
                break;
                
            default:
                break;
        }
        
        // Remove command from queue
        display_task_state.queue_head = (display_task_state.queue_head + 1) % 16;
        display_task_state.queue_count--;
    }
    
    // Clear display and render current screen
    bsp_display_clear();
    
    switch (display_task_state.current_screen) {
        case SCREEN_ID_WELCOME:
            display_screen_welcome();
            break;
        case SCREEN_ID_TIMEZONE_SETUP:
            display_screen_timezone_setup(&current_timezone_data);
            break;
        case SCREEN_ID_TIME_SETUP:
            display_screen_time_setup(&current_time_data);
            break;
        case SCREEN_ID_MAIN_MENU:
            display_screen_main_menu(&current_menu_data);
            break;
        case SCREEN_ID_SETTINGS:
            display_screen_settings(&current_settings_data);
            break;
        case SCREEN_ID_AGENT_SELECTION:
            display_screen_agent_selection(&current_agent_selection_data);
            break;
        case SCREEN_ID_AGENT_INTERACTION:
            display_screen_agent_interaction(&current_agent_interaction_data);
            break;
        case SCREEN_ID_LOCK_STATUS:
            display_screen_lock_status(&current_lock_status_data);
            break;
        case SCREEN_ID_LOCK_CONFIG_CUSTOM:
            display_screen_custom_lock_config(&current_custom_lock_data);
            break;
        case SCREEN_ID_LOCK_CONFIG_KEYHOLDER:
            display_screen_keyholder_config(&current_keyholder_data);
            break;
        case SCREEN_ID_PIN_ENTRY:
            display_screen_pin_entry(&current_pin_entry_data);
            break;
        case SCREEN_ID_GAME_SPIN_WHEEL:
            display_screen_spin_wheel(&current_spin_wheel_data);
            break;
        case SCREEN_ID_VERIFICATION:
            display_screen_verification(&current_verification_data);
            break;
        default:
            ui_component_draw_title_bar("CKOS");
            bsp_display_draw_text_centered(30, "Unknown Screen");
            break;
    }
    
    bsp_display_refresh();
}

bool display_task_send_command(DisplayCommand* cmd) {
    if (!cmd || display_task_state.queue_count >= 16) {
        return false;
    }
    
    display_task_state.command_queue[display_task_state.queue_tail] = *cmd;
    display_task_state.queue_tail = (display_task_state.queue_tail + 1) % 16;
    display_task_state.queue_count++;
    
    return true;
}

// =============================================================================
// SCREEN IMPLEMENTATIONS 
// =============================================================================

void display_screen_welcome(void) {
    // NEW STANDARDIZED WELCOME SCREEN
    ui_draw_standard_title_bar("CKOS v2.0", 85.0f);
    
    // Clean, centered welcome content
    ui_draw_centered_content("Welcome to", "Chastity Key OS", "System Ready");
    
    // Clear action hint
    ui_draw_button_hints("Continue", NULL);
}

void display_screen_timezone_setup(TimezoneScreenData* data) {
    char buffer[64];
    
    ui_component_draw_title_bar("Timezone");
    
    // Main timezone display area with better spacing
    bsp_display_draw_box(15, 18, 98, 25);
    
    if (data) {
        // Current timezone display
        snprintf(buffer, sizeof(buffer), "UTC%+d", data->timezone_offset);
        bsp_display_draw_text_centered(23, buffer);
        
        // DST status
        bsp_display_draw_text_centered(31, data->dst_active ? "DST: ON" : "DST: OFF");
    } else {
        // Default display when data is NULL
        bsp_display_draw_text_centered(23, "UTC+0");
        bsp_display_draw_text_centered(31, "DST: OFF");
    }
    
    // Enhanced input hints
    ui_component_draw_input_hints("<->: Zone  ^v: DST  A: Next");
}

void display_screen_time_setup(TimeScreenData* data) {
    ui_component_draw_title_bar("Time Setup");
    
    bsp_display_draw_box(15, 18, 98, 25);
    
    if (data && data->time_string[0] != '\0') {
        bsp_display_draw_text_centered(23, data->time_string);
        bsp_display_draw_text_centered(31, "Timezone applied");
    } else {
        bsp_display_draw_text_centered(23, "00:00:00");
        bsp_display_draw_text_centered(31, "Timezone applied");
    }
    
    // Enhanced input hints
    ui_component_draw_input_hints("A: Continue");
}

void display_screen_main_menu(MenuScreenData* data) {
    // NEW STANDARDIZED MAIN MENU
    ui_draw_standard_title_bar("Main Menu", 85.0f);
    
    if (data && data->options && data->menu_selection < data->max_items) {
        // Clean menu list with standardized layout
        ui_draw_menu_list(data->options, data->max_items, data->menu_selection, 
                         data->visible_start, data->max_visible);
    } else {
        // Error state with centered message
        ui_draw_centered_content("No menu data", "available", NULL);
    }
    
    // Consistent button hints
    ui_draw_button_hints("Select", "Settings");
}

void display_screen_settings(SettingsScreenData* data) {
    ui_component_draw_title_bar("Settings");
    
    if (data && data->settings_options && data->selection < data->max_settings) {
        // Scrolling settings menu
        ui_component_draw_scrolling_menu(data->settings_options, data->max_settings,
                                       data->selection, data->visible_start,
                                       data->max_visible);
        
        // Position indicator
        char position_text[16];
        snprintf(position_text, sizeof(position_text), "%d/%d", 
                 data->selection + 1, data->max_settings);
        bsp_display_draw_text_centered(52, position_text);
    } else {
        // Default display when data is invalid
        ui_component_draw_menu_selection(15, 25, 98, 15, "No Settings", true);
        bsp_display_draw_text_centered(52, "0/0");
    }
    
    ui_component_draw_input_hints("^v: Move  A: Select  B: Back");
}

// =============================================================================
// UI COMPONENT IMPLEMENTATIONS
// =============================================================================

void ui_component_draw_menu_selection(int x, int y, int width, int height, 
                                     const char* text, bool selected) {
    if (selected) {
        // Draw selection indicator with border
        bsp_display_draw_box(x - 1, y - 1, width + 2, height + 2);
        bsp_display_draw_text(x - 3, y + 1, ">");
    }
    
    if (text) {
        // Left-align text with padding
        bsp_display_draw_text(x + 3, y + 1, text);
    }
}

void ui_component_draw_scrolling_menu(const char** options, int num_options, 
                                     int selection, int visible_start, int max_visible) {
    if (!options) return;
    
    int menu_y = 18;
    int item_height = 8;
    
    for (int i = 0; i < max_visible && (visible_start + i) < num_options; i++) {
        int item_index = visible_start + i;
        bool is_selected = (item_index == selection);
        
        ui_component_draw_menu_selection(12, menu_y + i * item_height, 104, item_height - 1,
                                       options[item_index], is_selected);
    }
    
    // Draw scroll indicators if needed
    if (visible_start > 0) {
        bsp_display_draw_text_centered(menu_y - 4, "^ More");
    }
    
    if (visible_start + max_visible < num_options) {
        bsp_display_draw_text_centered(menu_y + max_visible * item_height + 1, "v More");
    }
}

void ui_component_draw_navigation_arrows(int selection, int max_items) {
    (void)selection;
    (void)max_items;
    // Simple navigation indicators
    bsp_display_draw_text(5, 30, "<");
    bsp_display_draw_text(118, 30, ">");
}

void ui_component_draw_title_bar(const char* title) {
    if (!title) return;
    
    // Draw title text at top
    bsp_display_draw_text_centered(2, title);
    
    // Draw separator line below title
    bsp_display_draw_line(5, 12, BSP_DISPLAY_WIDTH - 6, 12);
}

void ui_component_draw_status_bar(const char* battery, const char* time, bool locked) {
    int y = BSP_DISPLAY_HEIGHT - 9;
    
    // Draw status bar background
    bsp_display_draw_line(0, y - 1, BSP_DISPLAY_WIDTH - 1, y - 1);
    
    // Draw battery info on left
    if (battery) {
        bsp_display_draw_text(2, y, battery);
    }
    
    // Draw time in center
    if (time) {
        bsp_display_draw_text_centered(y, time);
    }
    
    // Draw lock status on right
    if (locked) {
        bsp_display_draw_text(BSP_DISPLAY_WIDTH - 30, y, "LOCKED");
    }
}

void ui_component_draw_input_hints(const char* hints) {
    if (!hints) return;
    
    // Draw hints at bottom with separator line
    bsp_display_draw_line(5, BSP_DISPLAY_HEIGHT - 10, BSP_DISPLAY_WIDTH - 6, BSP_DISPLAY_HEIGHT - 10);
    bsp_display_draw_text_centered(BSP_DISPLAY_HEIGHT - 7, hints);
}

// =============================================================================
// PLACEHOLDER IMPLEMENTATIONS FOR ADVANCED FEATURES
// =============================================================================

void display_screen_agent_selection(AgentSelectionScreenData* data) {
    // NEW STANDARDIZED AGENT SELECTION
    ui_draw_standard_title_bar("Select Agent", 85.0f);
    
    if (!data) {
        ui_draw_centered_content("No agent data", "available", NULL);
        ui_draw_button_hints(NULL, "Back");
        return;
    }
    
    // Clean agent list with descriptions
    const char* agent_names[] = {"Rookie", "Veteran", "Warden"};
    const char* agent_descriptions[] = {
        "Friendly & Supportive",
        "Balanced Experience", 
        "Strict & Demanding"
    };
    
    ui_draw_agent_selection_list(agent_names, agent_descriptions, 3, data->selected_agent);
    
    // Clear navigation hints
    ui_draw_button_hints("Choose Agent", "Back");
}

void display_screen_agent_interaction(AgentInteractionScreenData* data) {
    const char* agent_names[] = {"Rookie", "Veteran", "Warden"};
    
    if (!data) {
        ui_component_draw_title_bar("Agent Interaction");
        bsp_display_draw_text_centered(30, "No interaction data");
        ui_component_draw_input_hints("^v: Options  A: Select  B: Back");
        return;
    }
    
    // Show current agent name in title
    char title[32];
    snprintf(title, sizeof(title), "Agent: %s", 
             data->selected_agent < 3 ? agent_names[data->selected_agent] : "Unknown");
    ui_component_draw_title_bar(title);
    
    // Agent mood display area (top right)
    ui_component_draw_agent_mood_display(85, 16, data->selected_agent, data->mood_image_id,
                                         data->mood_affection, data->mood_strictness,
                                         data->mood_satisfaction, data->mood_trust);
    
    // Agent dialog box (main area)
    ui_component_draw_agent_dialog_box(8, 18, 75, 20, data->agent_dialog);
    
    // User interaction options (bottom area)
    if (data->interaction_options && data->num_options > 0) {
        int option_y = 40;
        for (int i = 0; i < data->num_options && i < 3; i++) {
            bool selected = (i == data->selected_option);
            ui_component_draw_menu_selection(12, option_y + i * 6, 100, 5,
                                           data->interaction_options[i], selected);
        }
    }
    
    ui_component_draw_input_hints("^v: Options  A: Select  B: Back");
}

void display_screen_lock_status(LockStatusScreenData* data) {
    // NEW STANDARDIZED LOCK STATUS
    ui_draw_standard_title_bar("Lock Status", data ? data->battery_percentage : 85.0f);
    
    if (!data) {
        ui_draw_centered_content("No lock status", "data available", NULL);
        ui_draw_button_hints("Request", "Menu");
        return;
    }
    
    // Convert time to hours and minutes for clean display
    uint32_t hours = data->time_remaining_seconds / 3600;
    uint32_t minutes = (data->time_remaining_seconds % 3600) / 60;
    
    // Use specialized lock status display
    ui_draw_lock_status_display(hours, minutes, data->agent_name, "Content");
    
    // Context-dependent button hints
    if (data->agent_name) {
        ui_draw_button_hints("Chat with Agent", "Emergency");
    } else {
        ui_draw_button_hints("Request", "Emergency");
    }
}

void display_screen_custom_lock_config(CustomLockConfigScreenData* data) {
    ui_component_draw_title_bar("Custom Lock Setup");
    
    if (!data) {
        bsp_display_draw_text_centered(30, "No config data");
        ui_component_draw_input_hints("^v: Navigate  <>: Change  A: Confirm");
        return;
    }
    
    // Duration selector
    bsp_display_draw_text(8, 18, "Duration:");
    ui_component_draw_time_duration_selector(8, 25, data->duration_hours, data->duration_minutes, 0);
    
    // Games option
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Games: %s", data->games_enabled ? "ON" : "OFF");
    bsp_display_draw_text(8, 36, buffer);
    
    if (data->games_enabled) {
        const char* game_names[] = {"Spin Wheel", "Dice Roll", "Card Draw"};
        snprintf(buffer, sizeof(buffer), "Game: %s", 
                 data->selected_game < 3 ? game_names[data->selected_game] : "Unknown");
        bsp_display_draw_text(8, 44, buffer);
    }
    
    // Total duration preview
    uint32_t total_minutes = data->duration_hours * 60 + data->duration_minutes;
    snprintf(buffer, sizeof(buffer), "Total: %lu mins", (unsigned long)total_minutes);
    bsp_display_draw_text_centered(52, buffer);
    
    ui_component_draw_input_hints("^v: Navigate  <>: Change  A: Confirm");
}

void display_screen_keyholder_config(KeyholderConfigScreenData* data) {
    (void)data;
    ui_component_draw_title_bar("Keyholder Setup");
    bsp_display_draw_text_centered(30, "No keyholder data");
    ui_component_draw_input_hints("^v: Navigate  A: Continue  B: Back  <>: Mode");
}

void display_screen_pin_entry(PinEntryScreenData* data) {
    ui_component_draw_title_bar("PIN Entry");
    
    if (!data) {
        bsp_display_draw_text_centered(30, "No PIN data");
        ui_component_draw_input_hints("^v<>: Navigate  A: Select  B: Delete");
        return;
    }
    
    // Show prompt text
    if (data->prompt_text) {
        bsp_display_draw_text_centered(18, data->prompt_text);
    }
    
    // PIN display area
    bsp_display_draw_box(15, 25, 98, 15);
    char display_pin[16] = {0};
    if (data->show_digits) {
        strncpy(display_pin, data->entered_pin, sizeof(display_pin) - 1);
    } else {
        // Show asterisks
        for (int i = 0; i < data->pin_length && i < 15; i++) {
            display_pin[i] = '*';
        }
    }
    bsp_display_draw_text_centered(30, display_pin);
    
    // Cursor indicator
    if (data->cursor_pos < data->pin_length) {
        int cursor_x = 64 + (data->cursor_pos - data->pin_length / 2) * 7;
        bsp_display_draw_text(cursor_x, 35, "_");
    }
    
    // PIN pad component
    ui_component_draw_pin_pad(20, 43, data->cursor_pos, data->entered_pin, data->show_digits);
    
    ui_component_draw_input_hints("^v<>: Navigate  A: Select  B: Delete");
}

void display_screen_spin_wheel(SpinWheelScreenData* data) {
    (void)data;
    ui_component_draw_title_bar("Spin the Wheel");
    bsp_display_draw_text_centered(30, "No wheel data");
    ui_component_draw_input_hints("A: Spin  B: Back");
}

void display_screen_verification(VerificationScreenData* data) {
    (void)data;
    ui_component_draw_title_bar("Verification");
    bsp_display_draw_text_centered(30, "No verification data");
    ui_component_draw_input_hints("B: Back");
}

// Additional UI component placeholders
void ui_component_draw_agent_mood_display(int x, int y, int agent_id, int mood_image_id, 
                                         float affection, float strictness, 
                                         float satisfaction, float trust) {
    (void)agent_id; (void)mood_image_id;
    
    // Draw mood indicator box
    bsp_display_draw_box(x, y, 35, 35);
    
    // Simple mood visualization - text-based for now
    char mood_text[8];
    if (affection > 0.7f) {
        strcpy(mood_text, "Happy");
    } else if (strictness > 0.7f) {
        strcpy(mood_text, "Stern");
    } else if (satisfaction < 0.3f) {
        strcpy(mood_text, "Upset");
    } else {
        strcpy(mood_text, "Calm");
    }
    
    bsp_display_draw_text_centered(y + 10, mood_text);
    
    // Show mood bars (simplified)
    int bar_y = y + 18;
    int bar_width = 30;
    
    // Affection bar
    bsp_display_draw_text(x, bar_y, "A:");
    int fill = (int)(affection * bar_width);
    bsp_display_draw_line(x + 10, bar_y, x + 10 + fill, bar_y);
    
    // Strictness bar
    bsp_display_draw_text(x, bar_y + 4, "S:");
    fill = (int)(strictness * bar_width);
    bsp_display_draw_line(x + 10, bar_y + 4, x + 10 + fill, bar_y + 4);
    
    // Trust bar
    bsp_display_draw_text(x, bar_y + 8, "T:");
    fill = (int)(trust * bar_width);
    bsp_display_draw_line(x + 10, bar_y + 8, x + 10 + fill, bar_y + 8);
}

void ui_component_draw_agent_dialog_box(int x, int y, int width, int height, 
                                       const char* dialog_text) {
    // Draw dialog box with double border for emphasis
    bsp_display_draw_box(x, y, width, height);
    bsp_display_draw_box(x + 1, y + 1, width - 2, height - 2);
    
    // Draw speech indicator
    bsp_display_draw_text(x + 2, y - 2, "Agent says:");
    
    if (dialog_text) {
        // Word wrap text within dialog box
        char buffer[256];
        strncpy(buffer, dialog_text, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
        
        int text_y = y + 3;
        int line_height = 7;
        char* line = strtok(buffer, " ");
        char current_line[32] = {0};
        
        while (line && text_y < y + height - 5) {
            // Check if adding this word would exceed line width
            if (strlen(current_line) + strlen(line) + 1 < 30) {
                if (strlen(current_line) > 0) {
                    strcat(current_line, " ");
                }
                strcat(current_line, line);
            } else {
                // Draw current line and start new one
                if (strlen(current_line) > 0) {
                    bsp_display_draw_text(x + 3, text_y, current_line);
                    text_y += line_height;
                }
                strcpy(current_line, line);
            }
            line = strtok(NULL, " ");
        }
        
        // Draw final line
        if (strlen(current_line) > 0 && text_y < y + height - 5) {
            bsp_display_draw_text(x + 3, text_y, current_line);
        }
    }
}

void ui_component_draw_agent_selection_card(int x, int y, int width, int height,
                                           const char* agent_name, const char* description, 
                                           bool selected) {
    ui_component_draw_menu_selection(x, y, width, height, agent_name, selected);
    if (description) {
        bsp_display_draw_text(x + 2, y + height + 2, description);
    }
}

void ui_component_draw_pin_pad(int x, int y, int cursor_pos, const char* entered_pin, 
                              bool show_digits) {
    (void)entered_pin; (void)show_digits;
    
    // 3x4 numeric keypad layout
    const char* keypad[4][3] = {
        {"1", "2", "3"},
        {"4", "5", "6"},
        {"7", "8", "9"},
        {"*", "0", "#"}
    };
    
    int key_width = 15;
    int key_height = 8;
    int spacing = 18;
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 3; col++) {
            int key_x = x + col * spacing;
            int key_y = y + row * (key_height + 2);
            
            // Determine if this key is selected
            int key_index = row * 3 + col;
            bool selected = (key_index == cursor_pos);
            
            // Draw key
            if (selected) {
                bsp_display_draw_box(key_x - 1, key_y - 1, key_width + 2, key_height + 2);
            }
            bsp_display_draw_box(key_x, key_y, key_width, key_height);
            
            // Draw key label
            int text_x = key_x + key_width / 2 - 3;
            int text_y = key_y + 2;
            bsp_display_draw_text(text_x, text_y, keypad[row][col]);
        }
    }
}

void ui_component_draw_time_duration_selector(int x, int y, uint32_t hours, 
                                             uint32_t minutes, int focus_field) {
    char buffer[32];
    
    // Hours field
    snprintf(buffer, sizeof(buffer), "%02lu", (unsigned long)hours);
    if (focus_field == 0) {
        bsp_display_draw_box(x - 2, y - 2, 20, 12);
    }
    bsp_display_draw_text(x, y, buffer);
    bsp_display_draw_text(x, y + 10, "HRS");
    
    // Separator
    bsp_display_draw_text(x + 25, y, ":");
    
    // Minutes field
    snprintf(buffer, sizeof(buffer), "%02lu", (unsigned long)minutes);
    if (focus_field == 1) {
        bsp_display_draw_box(x + 33, y - 2, 20, 12);
    }
    bsp_display_draw_text(x + 35, y, buffer);
    bsp_display_draw_text(x + 35, y + 10, "MIN");
    
    // Navigation hints
    bsp_display_draw_text(x + 60, y, "<> Change");
    bsp_display_draw_text(x + 60, y + 8, "^v Field");
}

void ui_component_draw_lock_status_display(int x, int y, const char* lock_type,
                                          uint32_t time_remaining, uint32_t session_time) {
    (void)x; (void)y; (void)lock_type; (void)time_remaining; (void)session_time;
    // Placeholder implementation
}

void ui_component_draw_identicon(int x, int y, const char* pattern) {
    (void)x; (void)y; (void)pattern;
    // Placeholder implementation
}

void ui_component_draw_wheel_segments(int center_x, int center_y, int radius,
                                     WheelSegmentData* segments, int num_segments,
                                     int highlighted_segment) {
    (void)center_x; (void)center_y; (void)radius; (void)segments; 
    (void)num_segments; (void)highlighted_segment;
    // Placeholder implementation
}

void ui_component_draw_game_result_display(int x, int y, const char* line1, 
                                          const char* line2) {
    if (line1) bsp_display_draw_text(x, y, line1);
    if (line2) bsp_display_draw_text(x, y + 10, line2);
}

void ui_component_draw_progress_bar(int x, int y, int width, int height,
                                   float percentage, const char* label) {
    bsp_display_draw_box(x, y, width, height);
    int fill_width = (int)(width * percentage);
    bsp_display_draw_filled_box(x + 1, y + 1, fill_width - 2, height - 2);
    
    if (label) {
        bsp_display_draw_text(x, y - 10, label);
    }
}

void ui_component_draw_battery_indicator(int x, int y, float percentage, bool charging) {
    bsp_display_draw_box(x, y, 20, 8);
    int fill = (int)(18 * percentage);
    bsp_display_draw_filled_box(x + 1, y + 1, fill, 6);
    
    if (charging) {
        bsp_display_draw_text(x + 22, y, "+");
    }
}

void ui_component_draw_connection_status(int x, int y, bool wifi, bool bluetooth) {
    if (wifi) {
        bsp_display_draw_text(x, y, "WiFi");
    }
    if (bluetooth) {
        bsp_display_draw_text(x + 30, y, "BT");
    }
}

void ui_component_draw_confirmation_dialog(const char* title, const char* message,
                                           const char* ok_text, const char* cancel_text,
                                           bool ok_selected) {
    // Draw dialog box
    bsp_display_draw_box(20, 15, 88, 35);
    bsp_display_draw_filled_box(21, 16, 86, 33);
    
    // Draw title and message
    if (title) bsp_display_draw_text_centered(20, title);
    if (message) bsp_display_draw_text_centered(30, message);
    
    // Draw buttons
    if (ok_text) {
        ui_component_draw_menu_selection(30, 40, 30, 8, ok_text, ok_selected);
    }
    if (cancel_text) {
        ui_component_draw_menu_selection(68, 40, 30, 8, cancel_text, !ok_selected);
    }
}

// =============================================================================
// NEW STANDARDIZED UI FRAMEWORK
// =============================================================================

void ui_draw_standard_title_bar(const char* title, float battery_percent) {
    // Top title bar with battery indicator (12 pixels high)
    bsp_display_draw_line(0, 11, BSP_DISPLAY_WIDTH - 1, 11);
    
    // Title on left
    if (title) {
        bsp_display_draw_text(2, 2, title);
    }
    
    // Battery indicator on right
    ui_component_draw_battery_indicator(BSP_DISPLAY_WIDTH - 25, 2, battery_percent / 100.0f, false);
}

void ui_draw_button_hints(const char* primary_action, const char* secondary_action) {
    // Bottom button hints bar (12 pixels high from bottom)
    int y = BSP_DISPLAY_HEIGHT - 12;
    bsp_display_draw_line(0, y, BSP_DISPLAY_WIDTH - 1, y);
    
    // Primary action (A button) on left
    if (primary_action) {
        char hint[32];
        snprintf(hint, sizeof(hint), "A: %s", primary_action);
        bsp_display_draw_text(2, y + 2, hint);
    }
    
    // Secondary action (B button) on right
    if (secondary_action) {
        char hint[32];
        snprintf(hint, sizeof(hint), "B: %s", secondary_action);
        // Right-align the text
        int text_width = strlen(hint) * 7; // 6 pixels + 1 space per char
        int x = BSP_DISPLAY_WIDTH - text_width - 2;
        bsp_display_draw_text(x, y + 2, hint);
    }
}

void ui_draw_centered_content(const char* line1, const char* line2, const char* line3) {
    // Draw up to 3 lines of centered content in the main content area
    int content_y_start = 14; // Below title bar
    int content_height = BSP_DISPLAY_HEIGHT - 14 - 12; // Above button hints
    int line_height = 10;
    
    int total_lines = 0;
    if (line1) total_lines++;
    if (line2) total_lines++;
    if (line3) total_lines++;
    
    int start_y = content_y_start + (content_height - (total_lines * line_height)) / 2;
    
    if (line1) {
        bsp_display_draw_text_centered(start_y, line1);
        start_y += line_height;
    }
    if (line2) {
        bsp_display_draw_text_centered(start_y, line2);
        start_y += line_height;
    }
    if (line3) {
        bsp_display_draw_text_centered(start_y, line3);
    }
}

void ui_draw_menu_list(const char* items[], int count, int selected, int visible_start, int max_visible) {
    // Draw a scrollable menu list in the content area
    int content_y_start = 14; // Below title bar
    int item_height = 10;
    
    for (int i = 0; i < max_visible && (visible_start + i) < count; i++) {
        int item_index = visible_start + i;
        int y = content_y_start + (i * item_height);
        bool is_selected = (item_index == selected);
        
        // Draw selection indicator
        if (is_selected) {
            bsp_display_draw_text(2, y, ">");
        }
        
        // Draw menu item
        bsp_display_draw_text(12, y, items[item_index]);
    }
    
    // Draw scroll indicators if needed
    if (visible_start > 0) {
        // Up arrow
        bsp_display_draw_text(BSP_DISPLAY_WIDTH - 10, content_y_start, "^");
    }
    if (visible_start + max_visible < count) {
        // Down arrow
        int arrow_y = content_y_start + (max_visible - 1) * item_height;
        bsp_display_draw_text(BSP_DISPLAY_WIDTH - 10, arrow_y, "v");
    }
}

void ui_draw_lock_status_display(int hours, int minutes, const char* agent_name, const char* mood) {
    // Specialized display for lock status screen
    int content_y_start = 16;
    
    // Large time display
    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%dh %02dm", hours, minutes);
    bsp_display_draw_text_centered(content_y_start + 5, time_str);
    bsp_display_draw_text_centered(content_y_start + 15, "remaining");
    
    // Agent info
    if (agent_name && mood) {
        char agent_str[32];
        snprintf(agent_str, sizeof(agent_str), "Agent: %s (%s)", agent_name, mood);
        bsp_display_draw_text_centered(content_y_start + 30, agent_str);
    }
}

void ui_draw_agent_selection_list(const char* agents[], const char* descriptions[], int count, int selected) {
    // Specialized display for agent selection
    int content_y_start = 14;
    int item_height = 12; // Larger for descriptions
    
    for (int i = 0; i < count; i++) {
        int y = content_y_start + (i * item_height);
        bool is_selected = (i == selected);
        
        // Selection indicator
        if (is_selected) {
            bsp_display_draw_text(2, y, ">");
        }
        
        // Agent name
        bsp_display_draw_text(12, y, agents[i]);
        
        // Description on next line, smaller and indented
        if (descriptions[i]) {
            bsp_display_draw_text(16, y + 8, descriptions[i]);
        }
    }
}