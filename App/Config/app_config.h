#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// CKOS Application Configuration
// Platform-independent configuration settings

// =============================================================================
// VERSION INFORMATION
// =============================================================================

#define CKOS_VERSION_MAJOR    2
#define CKOS_VERSION_MINOR    0
#define CKOS_VERSION_PATCH    0
#define CKOS_VERSION_STRING   "2.0.0"

// =============================================================================
// HARDWARE CONFIGURATION
// =============================================================================

// Display specifications
#define CONFIG_DISPLAY_WIDTH       128
#define CONFIG_DISPLAY_HEIGHT      64
#define CONFIG_DISPLAY_BPP          1    // 1 bit per pixel (monochrome)

// Input system
#define CONFIG_BUTTON_COUNT         6    // UP, DOWN, LEFT, RIGHT, A, B
#define CONFIG_BUTTON_DEBOUNCE_MS   50   // Button debounce time

// =============================================================================
// RTOS CONFIGURATION
// =============================================================================

// Task stack sizes (in bytes)
#define CONFIG_HARDWARE_TASK_STACK_SIZE     1024    // 1KB
#define CONFIG_APP_LOGIC_TASK_STACK_SIZE    2048    // 2KB  
#define CONFIG_DISPLAY_TASK_STACK_SIZE      1536    // 1.5KB

// Task priorities (higher number = higher priority)
#define CONFIG_HARDWARE_TASK_PRIORITY       5       // Highest
#define CONFIG_APP_LOGIC_TASK_PRIORITY      4       // Medium
#define CONFIG_DISPLAY_TASK_PRIORITY        3       // Lower

// Queue sizes
#define CONFIG_HARDWARE_REQUEST_QUEUE_SIZE  10      // Hardware request messages
#define CONFIG_DISPLAY_COMMAND_QUEUE_SIZE   16      // Display command messages

// =============================================================================
// TIMING CONFIGURATION
// =============================================================================

// Task update frequencies
#define CONFIG_APP_LOGIC_UPDATE_FREQ_HZ     60      // 60 FPS for responsive UI
#define CONFIG_DISPLAY_UPDATE_FREQ_HZ       30      // 30 FPS to save power
#define CONFIG_HARDWARE_UPDATE_FREQ_HZ      10      // 10 Hz for sensors

// Timeout values
#define CONFIG_DISPLAY_SLEEP_TIMEOUT_MS     30000   // 30 seconds
#define CONFIG_BUTTON_REPEAT_DELAY_MS       500     // Initial repeat delay
#define CONFIG_BUTTON_REPEAT_RATE_MS        100     // Subsequent repeat rate

// =============================================================================
// MEMORY CONFIGURATION
// =============================================================================

// Buffer sizes
#define CONFIG_TEXT_BUFFER_SIZE             64      // General text buffers
#define CONFIG_LOG_ENTRY_MAX_SIZE           256     // Log entry max size
#define CONFIG_PIN_MAX_LENGTH               8       // Maximum PIN length

// Storage layout
#define CONFIG_STORAGE_CONFIG_START_ADDR    0x0000  // Configuration start
#define CONFIG_STORAGE_CONFIG_SIZE          4096    // 4KB for config
#define CONFIG_STORAGE_LOG_START_ADDR       0x1000  // Log storage start

// =============================================================================
// AGENT SYSTEM CONFIGURATION
// =============================================================================

// Agent personalities
#define CONFIG_AGENT_COUNT                  3       // Rookie, Veteran, Warden

// Mood system ranges
#define CONFIG_AGENT_MOOD_MIN               0.0f    // Minimum mood value
#define CONFIG_AGENT_MOOD_MAX               1.0f    // Maximum mood value
#define CONFIG_AGENT_MOOD_DECAY_RATE        0.001f  // Per update decay

// =============================================================================
// LOCK SYSTEM CONFIGURATION
// =============================================================================

// Lock duration limits
#define CONFIG_LOCK_MIN_DURATION_SECONDS    60      // 1 minute minimum
#define CONFIG_LOCK_MAX_DURATION_SECONDS    (100UL * 365 * 24 * 3600) // 100 years max

// Memory wire parameters
#define CONFIG_MEMORY_WIRE_TARGET_TEMP      70      // Target temperature in °C
#define CONFIG_MEMORY_WIRE_HEAT_DURATION    5000    // Heat duration in ms

// =============================================================================
// CONNECTIVITY CONFIGURATION  
// =============================================================================

// WiFi settings
#define CONFIG_WIFI_CONNECT_TIMEOUT_MS      30000   // 30 second timeout
#define CONFIG_WIFI_SSID_MAX_LENGTH         32      // SSID max length
#define CONFIG_WIFI_PASSWORD_MAX_LENGTH     64      // Password max length

// Bluetooth settings
#define CONFIG_BT_DEVICE_NAME               "CKOS-Device"
#define CONFIG_BT_CONNECT_TIMEOUT_MS        15000   // 15 second timeout

// =============================================================================
// POWER MANAGEMENT CONFIGURATION
// =============================================================================

// Battery thresholds
#define CONFIG_BATTERY_LOW_THRESHOLD        20      // 20% low battery warning
#define CONFIG_BATTERY_CRITICAL_THRESHOLD   5       // 5% critical battery

// Power save timeouts
#define CONFIG_IDLE_TO_SLEEP_TIMEOUT_MS     60000   // 1 minute to sleep
#define CONFIG_SLEEP_TO_DEEP_SLEEP_MS       300000  // 5 minutes to deep sleep

// =============================================================================
// DEBUG CONFIGURATION
// =============================================================================

#ifdef DEBUG
    #define CONFIG_DEBUG_ENABLED            1
    #define CONFIG_DEBUG_SERIAL_BAUD        115200
    #define CONFIG_DEBUG_LOG_LEVEL          3       // 0=Error, 1=Warn, 2=Info, 3=Debug
#else
    #define CONFIG_DEBUG_ENABLED            0
    #define CONFIG_DEBUG_LOG_LEVEL          1       // Error and warnings only
#endif

// =============================================================================
// SIMULATOR-SPECIFIC CONFIGURATION
// =============================================================================

#ifdef BSP_PLATFORM_SIMULATOR
    #define CONFIG_SIM_WINDOW_SCALE         4       // 4x scaling for visibility
    #define CONFIG_SIM_AUTO_TEST_ENABLED    1       // Enable automated testing
    #define CONFIG_SIM_DEBUG_ASCII_OUTPUT   1       // ASCII art display output
#endif

#endif // APP_CONFIG_H
