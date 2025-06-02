// CKOS Simulator-Specific Main Entry Point
// Single-threaded SDL2 event loop that coordinates with RTOS simulation
//
// This version fixes SDL2 threading issues on macOS by keeping all SDL calls
// on the main thread while still running the same application logic

#include <stdio.h>
#include <string.h>
#include <atomic>
#include <thread>
#include <chrono>
#include "BSP/bsp_api.h"
#include "AppLogic/app_logic.h"
#include "Display/display_api.h"
#include "Hardware/hardware_api.h"
#include "Config/app_config.h"

// Global state for coordinating between main thread and simulation
static std::atomic<bool> g_running{true};
static std::atomic<bool> g_app_initialized{false};

// Simulation state update frequencies
static const int HARDWARE_UPDATE_MS = 100;  // 10 Hz
static const int APP_LOGIC_UPDATE_MS = 16;   // ~60 Hz  
static const int DISPLAY_UPDATE_MS = 33;     // ~30 Hz

// Last update times
static uint32_t last_hardware_update = 0;
static uint32_t last_app_logic_update = 0;
static uint32_t last_display_update = 0;

// =============================================================================
// SIMULATION THREAD FUNCTIONS
// =============================================================================

void hardware_simulation_update() {
    static bsp_sensor_readings_t readings;
    
    // Simulate sensor readings
    if (bsp_sensors_read(&readings) == 0) {
        // Hardware monitoring simulation
    }
}

void app_logic_simulation_update() {
    static bool initialized = false;
    
    if (!initialized) {
        app_logic_init();
        initialized = true;
        g_app_initialized = true;
        printf("Application logic initialized\n");
    }
    
    // Process button events (handled by main thread SDL loop)
    // App logic state updates
    app_logic_update();
}

void display_simulation_update() {
    // Process display commands and update screen
    display_task_update();
}

// =============================================================================
// MAIN SDL EVENT LOOP (Main Thread Only)
// =============================================================================

int main() {
    printf("CKOS Simulator Starting...\n");
    printf("Platform: Simulator (Single-threaded SDL)\n");
    
    // Initialize BSP systems (SDL initialization happens here)
    printf("Initializing BSP systems...\n");
    
    if (bsp_display_init() != 0) {
        printf("ERROR: Display initialization failed\n");
        return -1;
    }
    
    if (bsp_input_init() != 0) {
        printf("ERROR: Input initialization failed\n");
        return -1;
    }
    
    printf("BSP systems initialized successfully\n");
    
    // Initialize application components
    hardware_init();
    display_task_init();
    
    printf("CKOS initialization complete\n");
    printf("Running single-threaded simulation...\n");
    printf("Controls: Arrow keys to navigate, A/B for select/back, ESC to exit\n");
    
    // Main simulation loop (single-threaded)
    while (g_running) {
        uint32_t current_time = bsp_get_tick_ms();
        
        // 1. Handle SDL events (must be on main thread)
        bsp_button_event_t button_event;
        while (bsp_input_poll_event(&button_event)) {
            if (g_app_initialized) {
                app_logic_process_button_event(&button_event);
            }
        }
        
        // 2. Update hardware simulation at 10 Hz
        if (current_time - last_hardware_update >= HARDWARE_UPDATE_MS) {
            hardware_simulation_update();
            last_hardware_update = current_time;
        }
        
        // 3. Update application logic at ~60 Hz
        if (current_time - last_app_logic_update >= APP_LOGIC_UPDATE_MS) {
            app_logic_simulation_update();
            last_app_logic_update = current_time;
        }
        
        // 4. Update display at ~30 Hz
        if (current_time - last_display_update >= DISPLAY_UPDATE_MS) {
            display_simulation_update();
            last_display_update = current_time;
        }
        
        // Small delay to prevent excessive CPU usage
        bsp_delay_ms(1);
    }
    
    printf("Simulator shutting down...\n");
    
    // Cleanup
    bsp_display_cleanup();
    bsp_input_cleanup();
    
    return 0;
}