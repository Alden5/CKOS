#ifndef HARDWARE_API_H
#define HARDWARE_API_H

#include <stdint.h>
#include <stdbool.h>
#include "../BSP/bsp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// Hardware abstraction layer for CKOS device components
// This layer provides high-level hardware operations built on BSP primitives

// =============================================================================
// SENSOR SYSTEM
// =============================================================================

typedef struct {
    float battery_voltage;
    uint8_t battery_percentage;
    bool battery_charging;
    bool charging_active;
    float temperature_celsius;
    bool door_closed;
    bool latch_engaged;
    uint32_t timestamp_ms;
} hardware_sensor_data_t;

// Get latest sensor readings
int hardware_get_sensor_data(hardware_sensor_data_t* data);

// Individual sensor queries
float hardware_get_battery_percentage(void);
bool hardware_is_door_closed(void);
bool hardware_is_latch_engaged(void);

// =============================================================================
// LOCK MECHANISM
// =============================================================================

typedef enum {
    HARDWARE_LOCK_LOCKED = 0,
    HARDWARE_LOCK_UNLOCKING,
    HARDWARE_LOCK_UNLOCKED,
    HARDWARE_LOCK_ERROR
} hardware_lock_state_t;

// Lock control operations
int hardware_lock_engage(void);
int hardware_lock_release(void);
hardware_lock_state_t hardware_lock_get_state(void);

// Memory wire unlock system
int hardware_memory_wire_heat(uint8_t target_temp_celsius, uint16_t duration_ms);
bool hardware_memory_wire_is_heating(void);

// =============================================================================
// STORAGE SYSTEM  
// =============================================================================

// Configuration storage
int hardware_config_read(uint32_t address, void* data, uint32_t length);
int hardware_config_write(uint32_t address, const void* data, uint32_t length);

// Log storage 
int hardware_log_append(const char* log_entry);
int hardware_log_read(uint32_t index, char* buffer, uint32_t buffer_size);

// =============================================================================
// CHARGING SYSTEM
// =============================================================================

// Charging status
bool hardware_is_charging(void);

// =============================================================================
// POWER MANAGEMENT
// =============================================================================

typedef enum {
    HARDWARE_POWER_ACTIVE = 0,
    HARDWARE_POWER_SLEEP,
    HARDWARE_POWER_DEEP_SLEEP,
    HARDWARE_POWER_STANDBY
} hardware_power_mode_t;

// Power control
int hardware_power_set_mode(hardware_power_mode_t mode);
hardware_power_mode_t hardware_power_get_mode(void);

// Sleep suppression (prevents deep sleep during critical operations)
void hardware_power_suppress_sleep(const char* reason);
void hardware_power_allow_sleep(const char* reason);

// =============================================================================
// HARDWARE INITIALIZATION
// =============================================================================

// Initialize all hardware subsystems
int hardware_init(void);
void hardware_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif // HARDWARE_API_H
