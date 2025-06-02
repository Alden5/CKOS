// CKOS Display UI Unit Tests
// Tests for enhanced display functionality and UI components

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// Mock BSP functions for testing
static char mock_display_buffer[128 * 64 / 8];
static bool mock_display_initialized = false;
static int mock_text_calls = 0;
static int mock_box_calls = 0;
static int mock_line_calls = 0;

// Mock BSP implementations
void bsp_display_clear(void) {
    memset(mock_display_buffer, 0, sizeof(mock_display_buffer));
}

void bsp_display_refresh(void) {
    // Mock refresh - just set a flag
    mock_display_initialized = true;
}

void bsp_display_set_pixel(int x, int y, bool on) {
    (void)x; (void)y; (void)on;
    // Mock pixel setting
}

bool bsp_display_get_pixel(int x, int y) {
    (void)x; (void)y;
    return false; // Mock implementation
}

void bsp_display_draw_text(int x, int y, const char* text) {
    (void)x; (void)y; (void)text;
    mock_text_calls++;
}

void bsp_display_draw_text_centered(int y, const char* text) {
    (void)y; (void)text;
    mock_text_calls++;
}

void bsp_display_draw_line(int x1, int y1, int x2, int y2) {
    (void)x1; (void)y1; (void)x2; (void)y2;
    mock_line_calls++;
}

void bsp_display_draw_box(int x, int y, int w, int h) {
    (void)x; (void)y; (void)w; (void)h;
    mock_box_calls++;
}

void bsp_display_draw_filled_box(int x, int y, int w, int h) {
    (void)x; (void)y; (void)w; (void)h;
    mock_box_calls++;
}

// Include the display API after mocks
#include "../../App/Display/display_api.h"

// Test helpers
void reset_mock_counters(void) {
    mock_text_calls = 0;
    mock_box_calls = 0;
    mock_line_calls = 0;
}

void print_test_result(const char* test_name, bool passed) {
    printf("[%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

// =============================================================================
// BASIC UI COMPONENT TESTS
// =============================================================================

bool test_title_bar_component(void) {
    reset_mock_counters();
    
    ui_component_draw_title_bar("Test Title");
    
    // Should call text function once for title and line function once for separator
    bool result = (mock_text_calls == 1) && (mock_line_calls == 1);
    print_test_result("Title Bar Component", result);
    return result;
}

bool test_menu_selection_component(void) {
    reset_mock_counters();
    
    // Test selected item
    ui_component_draw_menu_selection(10, 20, 100, 10, "Test Item", true);
    int selected_box_calls = mock_box_calls;
    int selected_text_calls = mock_text_calls;
    
    reset_mock_counters();
    
    // Test unselected item
    ui_component_draw_menu_selection(10, 20, 100, 10, "Test Item", false);
    int unselected_box_calls = mock_box_calls;
    int unselected_text_calls = mock_text_calls;
    
    // Selected item should have more drawing calls (selection indicator)
    bool result = (selected_box_calls > unselected_box_calls) && 
                  (selected_text_calls > unselected_text_calls);
    print_test_result("Menu Selection Component", result);
    return result;
}

bool test_progress_bar_component(void) {
    reset_mock_counters();
    
    ui_component_draw_progress_bar(10, 10, 50, 8, 0.75f, "Test Progress");
    
    // Should draw outline box and filled box for progress
    bool result = (mock_box_calls >= 2) && (mock_text_calls >= 1);
    print_test_result("Progress Bar Component", result);
    return result;
}

bool test_battery_indicator_component(void) {
    reset_mock_counters();
    
    // Test battery without charging
    ui_component_draw_battery_indicator(10, 10, 0.8f, false);
    int no_charge_text_calls = mock_text_calls;
    
    reset_mock_counters();
    
    // Test battery with charging
    ui_component_draw_battery_indicator(10, 10, 0.8f, true);
    int charge_text_calls = mock_text_calls;
    
    // Charging indicator should add an extra text call
    bool result = (charge_text_calls > no_charge_text_calls) && (mock_box_calls >= 1);
    print_test_result("Battery Indicator Component", result);
    return result;
}

// =============================================================================
// ADVANCED UI COMPONENT TESTS
// =============================================================================

bool test_pin_pad_component(void) {
    reset_mock_counters();
    
    ui_component_draw_pin_pad(20, 30, 5, "12345", true);
    
    // Should draw 12 keys (3x4 grid), so at least 12 box calls
    bool result = (mock_box_calls >= 12) && (mock_text_calls >= 12);
    print_test_result("PIN Pad Component", result);
    return result;
}

bool test_time_duration_selector_component(void) {
    reset_mock_counters();
    
    ui_component_draw_time_duration_selector(10, 20, 2, 30, 0);
    
    // Should draw hours, minutes, separator, and navigation hints
    bool result = (mock_text_calls >= 4);
    print_test_result("Time Duration Selector Component", result);
    return result;
}

bool test_agent_mood_display_component(void) {
    reset_mock_counters();
    
    ui_component_draw_agent_mood_display(80, 20, 1, 0, 0.8f, 0.3f, 0.6f, 0.9f);
    
    // Should draw mood box, text, and mood bars
    bool result = (mock_box_calls >= 1) && (mock_text_calls >= 1) && (mock_line_calls >= 3);
    print_test_result("Agent Mood Display Component", result);
    return result;
}

bool test_agent_dialog_box_component(void) {
    reset_mock_counters();
    
    ui_component_draw_agent_dialog_box(10, 20, 80, 30, "Hello, this is a test message from the agent!");
    
    // Should draw dialog box and text
    bool result = (mock_box_calls >= 2) && (mock_text_calls >= 2);
    print_test_result("Agent Dialog Box Component", result);
    return result;
}

bool test_confirmation_dialog_component(void) {
    reset_mock_counters();
    
    ui_component_draw_confirmation_dialog("Confirm", "Are you sure?", "Yes", "No", true);
    
    // Should draw dialog box, title, message, and buttons
    bool result = (mock_box_calls >= 3) && (mock_text_calls >= 2);
    print_test_result("Confirmation Dialog Component", result);
    return result;
}

// =============================================================================
// SCREEN RENDERING TESTS
// =============================================================================

bool test_welcome_screen(void) {
    reset_mock_counters();
    
    display_screen_welcome();
    
    // Welcome screen should have multiple text and box elements
    bool result = (mock_text_calls >= 4) && (mock_box_calls >= 2) && (mock_line_calls >= 1);
    print_test_result("Welcome Screen Rendering", result);
    return result;
}

bool test_main_menu_screen(void) {
    reset_mock_counters();
    
    const char* menu_options[] = {"Agent Lock", "Custom Lock", "Keyholder Lock", "Settings"};
    MenuScreenData menu_data = {
        .menu_selection = 1,
        .max_items = 4,
        .visible_start = 0,
        .max_visible = 4,
        .options = menu_options
    };
    
    display_screen_main_menu(&menu_data);
    
    // Main menu should render title, menu items, and hints
    bool result = (mock_text_calls >= 3) && (mock_box_calls >= 1);
    print_test_result("Main Menu Screen Rendering", result);
    return result;
}

bool test_agent_selection_screen(void) {
    reset_mock_counters();
    
    AgentSelectionScreenData agent_data = {
        .selected_agent = 1
    };
    
    display_screen_agent_selection(&agent_data);
    
    // Agent selection should render title, agent cards, and hints
    bool result = (mock_text_calls >= 5) && (mock_box_calls >= 1);
    print_test_result("Agent Selection Screen Rendering", result);
    return result;
}

bool test_lock_status_screen(void) {
    reset_mock_counters();
    
    LockStatusScreenData status_data = {
        .lock_type = 0,
        .lock_type_name = "Agent Lock",
        .time_remaining_seconds = 3661, // 1 hour, 1 minute, 1 second
        .session_time_seconds = 1800,   // 30 minutes
        .is_break_active = false,
        .agent_name = "Rookie",
        .battery_percentage = 85.0f,
        .wifi_connected = false,
        .bluetooth_connected = false
    };
    
    display_screen_lock_status(&status_data);
    
    // Lock status should render title, time display, and status info
    bool result = (mock_text_calls >= 4) && (mock_box_calls >= 2);
    print_test_result("Lock Status Screen Rendering", result);
    return result;
}

bool test_pin_entry_screen(void) {
    reset_mock_counters();
    
    PinEntryScreenData pin_data = {
        .entered_pin = "1234",
        .pin_length = 4,
        .cursor_pos = 4,
        .show_digits = false,
        .prompt_text = "Enter PIN:"
    };
    
    display_screen_pin_entry(&pin_data);
    
    // PIN entry should render title, prompt, PIN display, and keypad
    bool result = (mock_text_calls >= 15) && (mock_box_calls >= 13); // Keypad + PIN display
    print_test_result("PIN Entry Screen Rendering", result);
    return result;
}

// =============================================================================
// DISPLAY TASK TESTS
// =============================================================================

bool test_display_task_initialization(void) {
    display_task_init();
    
    // Test should pass if no crashes occur
    bool result = true;
    print_test_result("Display Task Initialization", result);
    return result;
}

bool test_display_command_sending(void) {
    DisplayCommand cmd = {
        .id = DISPLAY_CMD_ACTIVATE_SCREEN,
        .data.activate_screen = {
            .screen_id = SCREEN_ID_WELCOME,
            .data_ptr = NULL
        }
    };
    
    bool sent = display_task_send_command(&cmd);
    
    print_test_result("Display Command Sending", sent);
    return sent;
}

bool test_display_task_update(void) {
    reset_mock_counters();
    
    // Send a command first
    DisplayCommand cmd = {
        .id = DISPLAY_CMD_ACTIVATE_SCREEN,
        .data.activate_screen = {
            .screen_id = SCREEN_ID_WELCOME,
            .data_ptr = NULL
        }
    };
    display_task_send_command(&cmd);
    
    // Process the command
    display_task_update();
    
    // Should have rendered the welcome screen
    bool result = (mock_text_calls > 0);
    print_test_result("Display Task Update Processing", result);
    return result;
}

// =============================================================================
// NULL SAFETY TESTS
// =============================================================================

bool test_null_safety(void) {
    reset_mock_counters();
    
    // Test components with NULL parameters
    ui_component_draw_title_bar(NULL);
    ui_component_draw_menu_selection(0, 0, 0, 0, NULL, false);
    ui_component_draw_agent_dialog_box(0, 0, 0, 0, NULL);
    ui_component_draw_progress_bar(0, 0, 0, 0, 0.5f, NULL);
    
    // Test screens with NULL data
    display_screen_main_menu(NULL);
    display_screen_agent_selection(NULL);
    display_screen_lock_status(NULL);
    display_screen_pin_entry(NULL);
    
    // Should not crash and should handle NULL gracefully
    bool result = true; // If we get here, no crashes occurred
    print_test_result("NULL Safety Tests", result);
    return result;
}

// =============================================================================
// EDGE CASE TESTS
// =============================================================================

bool test_boundary_values(void) {
    reset_mock_counters();
    
    // Test with extreme values
    ui_component_draw_progress_bar(0, 0, 1, 1, 0.0f, "");   // 0% progress
    ui_component_draw_progress_bar(0, 0, 1, 1, 1.0f, "");   // 100% progress
    ui_component_draw_progress_bar(0, 0, 1, 1, -0.1f, "");  // Below 0%
    ui_component_draw_progress_bar(0, 0, 1, 1, 1.1f, "");   // Above 100%
    
    ui_component_draw_battery_indicator(0, 0, 0.0f, false); // Empty battery
    ui_component_draw_battery_indicator(0, 0, 1.0f, true);  // Full battery charging
    
    ui_component_draw_time_duration_selector(0, 0, 0, 0, 0);      // Zero time
    ui_component_draw_time_duration_selector(0, 0, 99, 59, 1);    // Max reasonable time
    
    bool result = true; // Should handle boundary values gracefully
    print_test_result("Boundary Value Tests", result);
    return result;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void) {
    printf("CKOS Display UI Unit Tests\n");
    printf("===========================\n\n");
    
    int passed = 0;
    int total = 0;
    
    // Basic UI Component Tests
    printf("Basic UI Component Tests:\n");
    total++; if (test_title_bar_component()) passed++;
    total++; if (test_menu_selection_component()) passed++;
    total++; if (test_progress_bar_component()) passed++;
    total++; if (test_battery_indicator_component()) passed++;
    printf("\n");
    
    // Advanced UI Component Tests
    printf("Advanced UI Component Tests:\n");
    total++; if (test_pin_pad_component()) passed++;
    total++; if (test_time_duration_selector_component()) passed++;
    total++; if (test_agent_mood_display_component()) passed++;
    total++; if (test_agent_dialog_box_component()) passed++;
    total++; if (test_confirmation_dialog_component()) passed++;
    printf("\n");
    
    // Screen Rendering Tests
    printf("Screen Rendering Tests:\n");
    total++; if (test_welcome_screen()) passed++;
    total++; if (test_main_menu_screen()) passed++;
    total++; if (test_agent_selection_screen()) passed++;
    total++; if (test_lock_status_screen()) passed++;
    total++; if (test_pin_entry_screen()) passed++;
    printf("\n");
    
    // Display Task Tests
    printf("Display Task Tests:\n");
    total++; if (test_display_task_initialization()) passed++;
    total++; if (test_display_command_sending()) passed++;
    total++; if (test_display_task_update()) passed++;
    printf("\n");
    
    // Edge Case Tests
    printf("Edge Case Tests:\n");
    total++; if (test_null_safety()) passed++;
    total++; if (test_boundary_values()) passed++;
    printf("\n");
    
    // Summary
    printf("Test Results: %d/%d passed (%.1f%%)\n", 
           passed, total, (float)passed / total * 100.0f);
    
    if (passed == total) {
        printf("All tests PASSED! ✓\n");
        return 0;
    } else {
        printf("Some tests FAILED! ✗\n");
        return 1;
    }
}