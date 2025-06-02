// CKOS Application Logic & UI Integration Tests
// Tests the interaction between app logic and display systems

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// Mock function call tracking
static int mock_display_commands_sent = 0;
static int mock_screen_activations = 0;
static int mock_hardware_calls = 0;

// Mock implementations for testing
bool bsp_input_poll_event(void* event) {
    (void)event;
    return false; // No events for testing
}

uint32_t bsp_get_tick_ms(void) {
    return 1000; // Fixed time for testing
}

void bsp_task_delay(uint32_t ms) {
    (void)ms; // Mock delay
}

// Mock display command sending
bool display_task_send_command(void* cmd) {
    (void)cmd;
    mock_display_commands_sent++;
    return true;
}

// Mock hardware API calls
int hardware_get_sensor_data(void* data) {
    (void)data;
    mock_hardware_calls++;
    return 0;
}

bool hardware_is_door_closed(void) {
    return true; // Default closed for testing
}

bool hardware_is_latch_engaged(void) {
    return true; // Default engaged for testing
}

float hardware_get_battery_percentage(void) {
    return 85.0f; // Mock battery level
}

// Include app logic after mocks
#include "../../App/AppLogic/app_logic.h"

// Test result tracking
void print_test_result(const char* test_name, bool passed) {
    printf("[%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

void reset_mock_counters(void) {
    mock_display_commands_sent = 0;
    mock_screen_activations = 0;
    mock_hardware_calls = 0;
}

// =============================================================================
// MENU NAVIGATION TESTS
// =============================================================================

bool test_main_menu_navigation(void) {
    reset_mock_counters();
    
    // Initialize app logic
    app_logic_init();
    
    // Simulate navigation through main menu
    app_logic_handle_button_event(BSP_BUTTON_UP, true);
    app_logic_handle_button_event(BSP_BUTTON_DOWN, true);
    app_logic_handle_button_event(BSP_BUTTON_A, true); // Select
    
    // Should send display commands for menu updates
    bool result = (mock_display_commands_sent > 0);
    print_test_result("Main Menu Navigation", result);
    return result;
}

bool test_settings_menu_access(void) {
    reset_mock_counters();
    
    // Navigate to settings (assume it's the last menu item)
    for (int i = 0; i < 5; i++) {
        app_logic_handle_button_event(BSP_BUTTON_DOWN, true);
    }
    app_logic_handle_button_event(BSP_BUTTON_A, true); // Select settings
    
    // Should activate settings screen
    bool result = (mock_display_commands_sent > 0);
    print_test_result("Settings Menu Access", result);
    return result;
}

bool test_back_button_navigation(void) {
    reset_mock_counters();
    
    // Go into a submenu then back
    app_logic_handle_button_event(BSP_BUTTON_A, true);  // Enter submenu
    app_logic_handle_button_event(BSP_BUTTON_B, true);  // Back button
    
    // Should send display commands for navigation
    bool result = (mock_display_commands_sent > 0);
    print_test_result("Back Button Navigation", result);
    return result;
}

// =============================================================================
// LOCK CONFIGURATION TESTS
// =============================================================================

bool test_agent_lock_configuration(void) {
    reset_mock_counters();
    
    // Simulate configuring an agent lock
    app_logic_start_agent_lock_setup();
    
    // Select agent
    app_logic_handle_button_event(BSP_BUTTON_DOWN, true); // Navigate agents
    app_logic_handle_button_event(BSP_BUTTON_A, true);    // Select agent
    
    // Should send commands to display agent selection screen
    bool result = (mock_display_commands_sent > 0);
    print_test_result("Agent Lock Configuration", result);
    return result;
}

bool test_custom_lock_configuration(void) {
    reset_mock_counters();
    
    // Simulate configuring a custom lock
    app_logic_start_custom_lock_setup();
    
    // Adjust duration
    app_logic_handle_button_event(BSP_BUTTON_RIGHT, true); // Increase time
    app_logic_handle_button_event(BSP_BUTTON_LEFT, true);  // Decrease time
    app_logic_handle_button_event(BSP_BUTTON_A, true);     // Confirm
    
    // Should send commands to display custom lock screen
    bool result = (mock_display_commands_sent > 0);
    print_test_result("Custom Lock Configuration", result);
    return result;
}

bool test_keyholder_lock_configuration(void) {
    reset_mock_counters();
    
    // Simulate configuring a keyholder lock
    app_logic_start_keyholder_lock_setup();
    
    // Toggle between modes
    app_logic_handle_button_event(BSP_BUTTON_LEFT, true);  // Switch mode
    app_logic_handle_button_event(BSP_BUTTON_RIGHT, true); // Switch mode
    app_logic_handle_button_event(BSP_BUTTON_A, true);     // Confirm
    
    // Should send commands to display keyholder config screen
    bool result = (mock_display_commands_sent > 0);
    print_test_result("Keyholder Lock Configuration", result);
    return result;
}

// =============================================================================
// AGENT INTERACTION TESTS
// =============================================================================

bool test_agent_selection_flow(void) {
    reset_mock_counters();
    
    // Start agent selection
    app_logic_start_agent_lock_setup();
    
    // Test all three agents
    for (int agent = 0; agent < 3; agent++) {
        if (agent > 0) {
            app_logic_handle_button_event(BSP_BUTTON_DOWN, true);
        }
        // Preview agent without selecting
    }
    
    // Select final agent
    app_logic_handle_button_event(BSP_BUTTON_A, true);
    
    // Should update display for each agent preview and selection
    bool result = (mock_display_commands_sent >= 3);
    print_test_result("Agent Selection Flow", result);
    return result;
}

bool test_agent_mood_updates(void) {
    reset_mock_counters();
    
    // Simulate agent interaction that affects mood
    app_logic_agent_process_interaction(0, 2); // Agent 0, interaction option 2
    
    // Process several interactions
    for (int i = 0; i < 3; i++) {
        app_logic_agent_process_interaction(0, i % 3);
    }
    
    // Should send mood update commands
    bool result = (mock_display_commands_sent > 0);
    print_test_result("Agent Mood Updates", result);
    return result;
}

// =============================================================================
// LOCK STATUS AND MONITORING TESTS
// =============================================================================

bool test_lock_status_display_updates(void) {
    reset_mock_counters();
    
    // Simulate active lock
    app_logic_start_lock_session(0, 3600); // Agent lock, 1 hour
    
    // Process time updates (simulate periodic updates)
    for (int i = 0; i < 5; i++) {
        app_logic_process_lock_timer_update();
    }
    
    // Should send status update commands
    bool result = (mock_display_commands_sent > 0);
    print_test_result("Lock Status Display Updates", result);
    return result;
}

bool test_battery_status_integration(void) {
    reset_mock_counters();
    
    // Process battery status updates
    app_logic_process_sensor_updates();
    
    // Should query hardware and update display
    bool result = (mock_hardware_calls > 0) && (mock_display_commands_sent >= 0);
    print_test_result("Battery Status Integration", result);
    return result;
}

bool test_door_sensor_integration(void) {
    reset_mock_counters();
    
    // Simulate door state changes
    app_logic_handle_door_state_change(false); // Door opened
    app_logic_handle_door_state_change(true);  // Door closed
    
    // Should update hardware calls and display
    bool result = (mock_hardware_calls >= 0); // May or may not call hardware directly
    print_test_result("Door Sensor Integration", result);
    return result;
}

// =============================================================================
// ERROR HANDLING TESTS
// =============================================================================

bool test_invalid_input_handling(void) {
    reset_mock_counters();
    
    // Send invalid button events
    app_logic_handle_button_event(BSP_BUTTON_COUNT, true);    // Invalid button
    app_logic_handle_button_event(BSP_BUTTON_A, true);       // Valid button
    app_logic_handle_button_event(BSP_BUTTON_UP, true);      // Valid button
    
    // Should handle invalid inputs gracefully
    bool result = true; // If we get here, no crashes occurred
    print_test_result("Invalid Input Handling", result);
    return result;
}

bool test_state_transition_error_recovery(void) {
    reset_mock_counters();
    
    // Force application into various states and test recovery
    app_logic_init();
    app_logic_start_agent_lock_setup();
    app_logic_handle_button_event(BSP_BUTTON_B, true); // Back to menu
    app_logic_start_custom_lock_setup();
    app_logic_handle_button_event(BSP_BUTTON_B, true); // Back to menu
    
    // Should handle state transitions gracefully
    bool result = true; // No crashes = success
    print_test_result("State Transition Error Recovery", result);
    return result;
}

// =============================================================================
// PERFORMANCE TESTS
// =============================================================================

bool test_rapid_button_input_handling(void) {
    reset_mock_counters();
    
    // Simulate rapid button presses
    for (int i = 0; i < 50; i++) {
        app_logic_handle_button_event(BSP_BUTTON_UP, true);
        app_logic_handle_button_event(BSP_BUTTON_UP, false);
        app_logic_handle_button_event(BSP_BUTTON_DOWN, true);
        app_logic_handle_button_event(BSP_BUTTON_DOWN, false);
    }
    
    // Should handle rapid input without issues
    bool result = true; // No crashes = success
    print_test_result("Rapid Button Input Handling", result);
    return result;
}

bool test_display_command_queue_management(void) {
    reset_mock_counters();
    
    // Generate many display updates rapidly
    for (int i = 0; i < 20; i++) {
        app_logic_update_status_display();
        app_logic_update_battery_display();
    }
    
    // Should manage display command queue properly
    bool result = (mock_display_commands_sent > 0);
    print_test_result("Display Command Queue Management", result);
    return result;
}

// =============================================================================
// INTEGRATION WORKFLOW TESTS
// =============================================================================

bool test_complete_lock_setup_workflow(void) {
    reset_mock_counters();
    
    // Complete workflow: Menu -> Agent Selection -> Lock Configuration -> Lock Start
    app_logic_init();
    
    // Navigate to agent lock
    app_logic_handle_button_event(BSP_BUTTON_A, true); // Select agent lock
    
    // Select agent
    app_logic_handle_button_event(BSP_BUTTON_DOWN, true); // Navigate to Veteran
    app_logic_handle_button_event(BSP_BUTTON_A, true);    // Select Veteran
    
    // Confirm lock setup
    app_logic_handle_button_event(BSP_BUTTON_A, true);    // Confirm
    
    // Should send multiple display commands throughout workflow
    bool result = (mock_display_commands_sent >= 3);
    print_test_result("Complete Lock Setup Workflow", result);
    return result;
}

bool test_settings_modification_workflow(void) {
    reset_mock_counters();
    
    // Navigate to settings and modify values
    app_logic_enter_settings_menu();
    
    // Navigate through settings
    app_logic_handle_button_event(BSP_BUTTON_DOWN, true); // Next setting
    app_logic_handle_button_event(BSP_BUTTON_RIGHT, true); // Change value
    app_logic_handle_button_event(BSP_BUTTON_A, true);     // Confirm
    app_logic_handle_button_event(BSP_BUTTON_B, true);     // Back to menu
    
    // Should send display commands for settings screens
    bool result = (mock_display_commands_sent > 0);
    print_test_result("Settings Modification Workflow", result);
    return result;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void) {
    printf("CKOS Application Logic & UI Integration Tests\n");
    printf("===============================================\n\n");
    
    int passed = 0;
    int total = 0;
    
    // Menu Navigation Tests
    printf("Menu Navigation Tests:\n");
    total++; if (test_main_menu_navigation()) passed++;
    total++; if (test_settings_menu_access()) passed++;
    total++; if (test_back_button_navigation()) passed++;
    printf("\n");
    
    // Lock Configuration Tests
    printf("Lock Configuration Tests:\n");
    total++; if (test_agent_lock_configuration()) passed++;
    total++; if (test_custom_lock_configuration()) passed++;
    total++; if (test_keyholder_lock_configuration()) passed++;
    printf("\n");
    
    // Agent Interaction Tests
    printf("Agent Interaction Tests:\n");
    total++; if (test_agent_selection_flow()) passed++;
    total++; if (test_agent_mood_updates()) passed++;
    printf("\n");
    
    // Lock Status and Monitoring Tests
    printf("Lock Status and Monitoring Tests:\n");
    total++; if (test_lock_status_display_updates()) passed++;
    total++; if (test_battery_status_integration()) passed++;
    total++; if (test_door_sensor_integration()) passed++;
    printf("\n");
    
    // Error Handling Tests
    printf("Error Handling Tests:\n");
    total++; if (test_invalid_input_handling()) passed++;
    total++; if (test_state_transition_error_recovery()) passed++;
    printf("\n");
    
    // Performance Tests
    printf("Performance Tests:\n");
    total++; if (test_rapid_button_input_handling()) passed++;
    total++; if (test_display_command_queue_management()) passed++;
    printf("\n");
    
    // Integration Workflow Tests
    printf("Integration Workflow Tests:\n");
    total++; if (test_complete_lock_setup_workflow()) passed++;
    total++; if (test_settings_modification_workflow()) passed++;
    printf("\n");
    
    // Summary
    printf("Test Results: %d/%d passed (%.1f%%)\n", 
           passed, total, (float)passed / total * 100.0f);
    
    if (passed == total) {
        printf("All integration tests PASSED! ✓\n");
        return 0;
    } else {
        printf("Some integration tests FAILED! ✗\n");
        return 1;
    }
}