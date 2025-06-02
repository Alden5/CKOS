#ifndef BSP_API_H
#define BSP_API_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// DISPLAY ABSTRACTION
// =============================================================================

#define BSP_DISPLAY_WIDTH  128
#define BSP_DISPLAY_HEIGHT 64

// Display initialization and control
int bsp_display_init(void);
void bsp_display_cleanup(void);
void bsp_display_clear(void);
void bsp_display_refresh(void);

// Pixel operations
void bsp_display_set_pixel(int x, int y, bool on);
bool bsp_display_get_pixel(int x, int y);

// Text rendering (basic 6x8 font)
void bsp_display_draw_text(int x, int y, const char* text);
void bsp_display_draw_text_centered(int y, const char* text);

// Shape drawing primitives
void bsp_display_draw_line(int x1, int y1, int x2, int y2);
void bsp_display_draw_box(int x, int y, int w, int h);
void bsp_display_draw_filled_box(int x, int y, int w, int h);

// =============================================================================
// INPUT ABSTRACTION  
// =============================================================================

typedef enum {
    BSP_BUTTON_UP = 0,
    BSP_BUTTON_DOWN,
    BSP_BUTTON_LEFT,
    BSP_BUTTON_RIGHT,
    BSP_BUTTON_A,      // Enter/Select
    BSP_BUTTON_B,      // Back/Cancel
    BSP_BUTTON_COUNT
} bsp_button_id_t;

typedef struct {
    bsp_button_id_t button;
    bool pressed;
    uint32_t timestamp;
} bsp_button_event_t;

// Input system
int bsp_input_init(void);
void bsp_input_cleanup(void);
bool bsp_input_poll_event(bsp_button_event_t* event);

// =============================================================================
// TIMING ABSTRACTION
// =============================================================================

// Time functions
uint32_t bsp_get_tick_ms(void);
uint64_t bsp_get_utc_time_seconds(void);
void bsp_delay_ms(uint32_t ms);

// =============================================================================
// RTOS ABSTRACTION
// =============================================================================

// For simulator: simulate the three-task architecture
// For STM32: real FreeRTOS implementation

typedef void* bsp_task_handle_t;
typedef void* bsp_queue_handle_t;
typedef void (*bsp_task_function_t)(void* parameters);

// Task management
bsp_task_handle_t bsp_task_create(bsp_task_function_t task_function, 
                                  const char* name,
                                  uint16_t stack_size,
                                  void* parameters,
                                  uint8_t priority);
void bsp_task_delete(bsp_task_handle_t task);
void bsp_task_delay(uint32_t ms);
void bsp_task_yield(void);

// Queue management (for inter-task communication)
bsp_queue_handle_t bsp_queue_create(uint8_t length, uint8_t item_size);
void bsp_queue_delete(bsp_queue_handle_t queue);
bool bsp_queue_send(bsp_queue_handle_t queue, const void* item, uint32_t timeout_ms);
bool bsp_queue_receive(bsp_queue_handle_t queue, void* item, uint32_t timeout_ms);

// RTOS scheduler
void bsp_scheduler_start(void);

// =============================================================================
// HARDWARE SERVICES ABSTRACTION
// =============================================================================

// Storage operations
typedef struct {
    uint32_t address;
    uint8_t* data;
    uint32_t length;
} bsp_storage_operation_t;

int bsp_storage_read(const bsp_storage_operation_t* op);
int bsp_storage_write(const bsp_storage_operation_t* op);

// Sensor readings
typedef struct {
    float battery_voltage;
    uint8_t battery_percentage;
    float temperature_celsius;
    bool door_closed;
    bool latch_engaged;
    bool charging_active;
} bsp_sensor_readings_t;

int bsp_sensors_read(bsp_sensor_readings_t* readings);

// Lock mechanism control
typedef enum {
    BSP_LOCK_STATE_LOCKED = 0,
    BSP_LOCK_STATE_UNLOCKING,
    BSP_LOCK_STATE_UNLOCKED,
    BSP_LOCK_STATE_ERROR
} bsp_lock_state_t;

int bsp_lock_unlock(void);
bsp_lock_state_t bsp_lock_get_state(void);

// Power management  
typedef enum {
    BSP_POWER_MODE_RUN = 0,
    BSP_POWER_MODE_SLEEP,
    BSP_POWER_MODE_STOP2,
    BSP_POWER_MODE_STANDBY
} bsp_power_mode_t;

int bsp_power_set_mode(bsp_power_mode_t mode);

// =============================================================================
// PLATFORM DETECTION
// =============================================================================

#ifdef STM32L452xx
    #define BSP_PLATFORM_STM32
#elif defined(SIMULATOR)
    #define BSP_PLATFORM_SIMULATOR  
#else
    #error "Unknown platform - define STM32L452xx or SIMULATOR"
#endif

// =============================================================================
// DEBUG/DEVELOPMENT FEATURES
// =============================================================================

#ifdef BSP_PLATFORM_SIMULATOR
// Simulator-specific debug functions
void bsp_debug_print_display(void); // ASCII art display output
void bsp_debug_set_sensor_value(const char* sensor, float value);
void bsp_debug_trigger_button(bsp_button_id_t button);
#endif

#ifdef __cplusplus
}
#endif

#endif // BSP_API_H