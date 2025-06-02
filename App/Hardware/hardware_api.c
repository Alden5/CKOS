// CKOS Hardware API Implementation
// High-level hardware operations built on BSP primitives

#include "hardware_api.h"
#include <string.h>
#include <stdio.h>

// =============================================================================
// SENSOR SYSTEM IMPLEMENTATION
// =============================================================================

int hardware_get_sensor_data(hardware_sensor_data_t* data) {
    if (!data) return -1;
    
    // Get sensor readings through BSP
    bsp_sensor_readings_t bsp_readings;
    int result = bsp_sensors_read(&bsp_readings);
    
    if (result == 0) {
        // Map BSP readings to hardware API format
        data->battery_voltage = bsp_readings.battery_voltage;
        data->battery_percentage = bsp_readings.battery_percentage;
        data->battery_charging = false; // BSP doesn't provide this yet
        data->temperature_celsius = bsp_readings.temperature_celsius;
        data->door_closed = bsp_readings.door_closed;
        data->latch_engaged = bsp_readings.latch_engaged;
        data->charging_active = bsp_readings.charging_active;
        data->timestamp_ms = bsp_get_tick_ms();
    }
    
    return result;
}

float hardware_get_battery_percentage(void) {
    bsp_sensor_readings_t readings;
    if (bsp_sensors_read(&readings) == 0) {
        return readings.battery_percentage;
    }
    return 0.0f;
}

bool hardware_is_door_closed(void) {
    bsp_sensor_readings_t readings;
    if (bsp_sensors_read(&readings) == 0) {
        return readings.door_closed;
    }
    return false;
}

bool hardware_is_latch_engaged(void) {
    bsp_sensor_readings_t readings;
    if (bsp_sensors_read(&readings) == 0) {
        return readings.latch_engaged;
    }
    return false;
}

// =============================================================================
// LOCK MECHANISM IMPLEMENTATION
// =============================================================================

int hardware_lock_engage(void) {
    // For now, this is automatic when door closes
    printf("Hardware: Lock engage requested\n");
    return 0;
}

int hardware_lock_release(void) {
    return bsp_lock_unlock();
}

hardware_lock_state_t hardware_lock_get_state(void) {
    bsp_lock_state_t bsp_state = bsp_lock_get_state();
    
    // Map BSP lock state to hardware API
    switch (bsp_state) {
        case BSP_LOCK_STATE_LOCKED:
            return HARDWARE_LOCK_LOCKED;
        case BSP_LOCK_STATE_UNLOCKING:
            return HARDWARE_LOCK_UNLOCKING;
        case BSP_LOCK_STATE_UNLOCKED:
            return HARDWARE_LOCK_UNLOCKED;
        case BSP_LOCK_STATE_ERROR:
        default:
            return HARDWARE_LOCK_ERROR;
    }
}

int hardware_memory_wire_heat(uint8_t target_temp_celsius, uint16_t duration_ms) {
    (void)target_temp_celsius;
    (void)duration_ms;
    printf("Hardware: Memory wire heating to %d°C for %dms\n", 
           target_temp_celsius, duration_ms);
    return 0;
}

bool hardware_memory_wire_is_heating(void) {
    // Placeholder implementation
    return false;
}

// =============================================================================
// STORAGE SYSTEM IMPLEMENTATION
// =============================================================================

int hardware_config_read(uint32_t address, void* data, uint32_t length) {
    bsp_storage_operation_t op = {
        .address = address,
        .data = (uint8_t*)data,
        .length = length
    };
    
    return bsp_storage_read(&op);
}

int hardware_config_write(uint32_t address, const void* data, uint32_t length) {
    bsp_storage_operation_t op = {
        .address = address,
        .data = (uint8_t*)data,
        .length = length
    };
    
    return bsp_storage_write(&op);
}

int hardware_log_append(const char* log_entry) {
    if (!log_entry) return -1;
    
    printf("Hardware Log: %s\n", log_entry);
    
    // TODO: Implement actual log storage
    return 0;
}

int hardware_log_read(uint32_t index, char* buffer, uint32_t buffer_size) {
    if (!buffer || buffer_size == 0) return -1;
    
    // Placeholder implementation
    snprintf(buffer, buffer_size, "Log entry %lu", (unsigned long)index);
    return 0;
}

// =============================================================================
// CHARGING SYSTEM IMPLEMENTATION
// =============================================================================

bool hardware_is_charging(void) {
    bsp_sensor_readings_t readings;
    if (bsp_sensors_read(&readings) == 0) {
        return readings.charging_active;
    }
    return false;
}

// =============================================================================
// POWER MANAGEMENT IMPLEMENTATION
// =============================================================================

int hardware_power_set_mode(hardware_power_mode_t mode) {
    bsp_power_mode_t bsp_mode;
    
    // Map hardware API power mode to BSP
    switch (mode) {
        case HARDWARE_POWER_ACTIVE:
            bsp_mode = BSP_POWER_MODE_RUN;
            break;
        case HARDWARE_POWER_SLEEP:
            bsp_mode = BSP_POWER_MODE_SLEEP;
            break;
        case HARDWARE_POWER_DEEP_SLEEP:
            bsp_mode = BSP_POWER_MODE_STOP2;
            break;
        case HARDWARE_POWER_STANDBY:
            bsp_mode = BSP_POWER_MODE_STANDBY;
            break;
        default:
            return -1;
    }
    
    return bsp_power_set_mode(bsp_mode);
}

hardware_power_mode_t hardware_power_get_mode(void) {
    // Placeholder - BSP doesn't provide power mode query yet
    return HARDWARE_POWER_ACTIVE;
}

void hardware_power_suppress_sleep(const char* reason) {
    printf("Hardware: Sleep suppressed - %s\n", reason ? reason : "unknown");
}

void hardware_power_allow_sleep(const char* reason) {
    printf("Hardware: Sleep allowed - %s\n", reason ? reason : "unknown");
}

// =============================================================================
// HARDWARE INITIALIZATION
// =============================================================================

int hardware_init(void) {
    printf("Hardware: Initializing hardware subsystems...\n");
    
    // Hardware initialization is handled by BSP layer
    // This function provides high-level initialization coordination
    
    printf("Hardware: Hardware subsystems initialized\n");
    return 0;
}

void hardware_cleanup(void) {
    printf("Hardware: Cleaning up hardware subsystems...\n");
    
    // Cleanup coordination for hardware subsystems
    
    printf("Hardware: Hardware cleanup complete\n");
}