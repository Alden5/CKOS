// CKOS Main Application Entry Point
// Three-Task RTOS Architecture per documentation
//
// This is the single source of truth that runs on both STM32 and simulator
// Hardware differences are abstracted through the BSP layer

#include <stdio.h>
#include <string.h>
#include "BSP/bsp_api.h"
#include "AppLogic/app_logic.h"
#include "Display/display_api.h"
#include "Hardware/hardware_api.h"
#include "Config/app_config.h"

// Task handles for the three-task architecture
static bsp_task_handle_t hardware_service_task_handle;
static bsp_task_handle_t application_logic_task_handle;
static bsp_task_handle_t display_task_handle;

// Inter-task communication queues
static bsp_queue_handle_t hardware_request_queue;
static bsp_queue_handle_t display_command_queue;

// =============================================================================
// HARDWARE SERVICE TASK (Priority 5 - Highest)
// =============================================================================
// Handles all direct hardware interaction:
// - Sensor readings (battery, temperature, door sensors)
// - Lock mechanism control (memory wire unlock)
// - Storage operations (configuration, logs)
// - Power management
// =============================================================================

void hardware_service_task(void* parameters) {
    (void)parameters; // Unused
    
    printf("HardwareService_Task starting...\n");
    
    // Initialize hardware systems through BSP
    if (bsp_sensors_read(NULL) != 0) {
        printf("Warning: Sensor initialization failed\n");
    }
    
    bsp_sensor_readings_t readings;
    uint32_t last_sensor_update = 0;
    
    while (true) {
        uint32_t current_time = bsp_get_tick_ms();
        
        // Update sensors every 1000ms
        if (current_time - last_sensor_update >= 1000) {
            if (bsp_sensors_read(&readings) == 0) {
                // Hardware monitoring and status updates
                // TODO: Send sensor data to ApplicationLogic_Task if needed
            }
            last_sensor_update = current_time;
        }
        
        // Check for hardware requests from ApplicationLogic_Task
        // TODO: Implement hardware request processing
        
        // Yield to other tasks
        bsp_task_delay(10);
    }
}

// =============================================================================
// APPLICATION LOGIC TASK (Priority 4 - Medium)  
// =============================================================================
// Handles business logic:
// - State machine processing
// - User input handling
// - Agent AI and lock logic
// - Menu navigation
// =============================================================================

void application_logic_task(void* parameters) {
    (void)parameters; // Unused
    
    printf("ApplicationLogic_Task starting...\n");
    
    // Initialize application logic
    app_logic_init();
    
    bsp_button_event_t button_event;
    
    while (true) {
        // Process user input
        if (bsp_input_poll_event(&button_event)) {
            app_logic_process_button_event(&button_event);
        }
        
        // Update application state
        app_logic_update();
        
        // Task update frequency: 60Hz for responsive UI
        bsp_task_delay(16); // ~60 FPS
    }
}

// =============================================================================
// DISPLAY TASK (Priority 3 - Lower)
// =============================================================================
// Handles all UI rendering:
// - Screen drawing and updates
// - Animation processing  
// - Framebuffer management
// - Display command processing
// =============================================================================

void display_task_function(void* parameters) {
    (void)parameters; // Unused
    
    printf("Display_Task starting...\n");
    
    // Initialize display system through BSP
    if (bsp_display_init() != 0) {
        printf("ERROR: Display initialization failed!\n");
        return;
    }
    
    // Initialize display task logic
    display_task_init();
    
    while (true) {
        // Process display commands from ApplicationLogic_Task
        display_task_update();
        
        // Task update frequency: 30Hz to save power
        bsp_task_delay(33); // ~30 FPS
    }
}

// =============================================================================
// SYSTEM INITIALIZATION
// =============================================================================

static int initialize_bsp_systems(void) {
    printf("Initializing BSP systems...\n");
    
    // Initialize input system
    if (bsp_input_init() != 0) {
        printf("ERROR: Input system initialization failed\n");
        return -1;
    }
    
    // Initialize display system  
    if (bsp_display_init() != 0) {
        printf("ERROR: Display system initialization failed\n");
        return -1;
    }
    
    printf("BSP systems initialized successfully\n");
    return 0;
}

static int create_rtos_tasks(void) {
    printf("Creating RTOS tasks...\n");
    
    // Create HardwareService_Task (Priority 5 - Highest)
    hardware_service_task_handle = bsp_task_create(
        hardware_service_task,
        "HardwareService", 
        1024,   // 1KB stack as per documentation
        NULL,   // No parameters
        5       // Highest priority
    );
    
    if (!hardware_service_task_handle) {
        printf("ERROR: Failed to create HardwareService_Task\n");
        return -1;
    }
    
    // Create ApplicationLogic_Task (Priority 4 - Medium)
    application_logic_task_handle = bsp_task_create(
        application_logic_task,
        "ApplicationLogic",
        2048,   // 2KB stack for application logic
        NULL,   // No parameters  
        4       // Medium priority
    );
    
    if (!application_logic_task_handle) {
        printf("ERROR: Failed to create ApplicationLogic_Task\n");
        return -1;
    }
    
    // Create Display_Task (Priority 3 - Lower)
    display_task_handle = bsp_task_create(
        display_task_function,
        "Display",
        1536,   // 1.5KB stack for display operations
        NULL,   // No parameters
        3       // Lower priority
    );
    
    if (!display_task_handle) {
        printf("ERROR: Failed to create Display_Task\n");
        return -1;
    }
    
    printf("All RTOS tasks created successfully\n");
    return 0;
}

static int create_communication_queues(void) {
    printf("Creating inter-task communication queues...\n");
    
    // Hardware request queue (ApplicationLogic -> HardwareService)
    hardware_request_queue = bsp_queue_create(10, 64); // 10 messages, 64 bytes each
    if (!hardware_request_queue) {
        printf("ERROR: Failed to create hardware request queue\n");
        return -1;
    }
    
    // Display command queue (ApplicationLogic -> Display)  
    display_command_queue = bsp_queue_create(16, sizeof(DisplayCommand));
    if (!display_command_queue) {
        printf("ERROR: Failed to create display command queue\n");
        return -1;
    }
    
    printf("Communication queues created successfully\n");
    return 0;
}

// =============================================================================
// MAIN ENTRY POINT
// =============================================================================

int main() {
    printf("CKOS Starting...\n");
    printf("Platform: ");
    
#ifdef BSP_PLATFORM_STM32
    printf("STM32L452CEUx\n");
#elif defined(BSP_PLATFORM_SIMULATOR)
    printf("Simulator\n");
#else
    printf("Unknown\n");
#endif
    
    // Step 1: Initialize BSP systems
    if (initialize_bsp_systems() != 0) {
        printf("FATAL: BSP initialization failed\n");
        return -1;
    }
    
    // Step 2: Create inter-task communication
    if (create_communication_queues() != 0) {
        printf("FATAL: Queue creation failed\n");
        return -1; 
    }
    
    // Step 3: Create RTOS tasks
    if (create_rtos_tasks() != 0) {
        printf("FATAL: Task creation failed\n");
        return -1;
    }
    
    printf("CKOS initialization complete - starting scheduler\n");
    
    // Step 4: Start RTOS scheduler (never returns)
    bsp_scheduler_start();
    
    // Should never reach here
    printf("ERROR: Scheduler returned unexpectedly\n");
    return -1;
}