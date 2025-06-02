#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// STRING UTILITIES
// =============================================================================

char* utils_safe_strcpy(char* dest, const char* src, size_t dest_size);
char* utils_safe_strcat(char* dest, const char* src, size_t dest_size);

// =============================================================================
// MATHEMATICAL UTILITIES
// =============================================================================

int utils_clamp_int(int value, int min_val, int max_val);
float utils_clamp_float(float value, float min_val, float max_val);
float utils_lerp(float a, float b, float t);

// =============================================================================
// TIME UTILITIES
// =============================================================================

void utils_seconds_to_time_string(uint32_t seconds, char* buffer, size_t buffer_size);
uint32_t utils_time_string_to_seconds(const char* time_str);

// =============================================================================
// CRC UTILITIES
// =============================================================================

uint16_t utils_crc16(const uint8_t* data, size_t length);

// =============================================================================
// MEMORY UTILITIES
// =============================================================================

void utils_secure_memset(void* ptr, int value, size_t size);

// =============================================================================
// VALIDATION UTILITIES
// =============================================================================

bool utils_validate_pin(const char* pin);
bool utils_validate_timezone(int offset_hours);

// =============================================================================
// FORMATTING UTILITIES
// =============================================================================

void utils_format_battery(float percentage, char* buffer, size_t buffer_size);
void utils_format_file_size(uint32_t bytes, char* buffer, size_t buffer_size);

// =============================================================================
// RING BUFFER UTILITY
// =============================================================================

typedef struct {
    uint8_t* buffer;
    size_t size;
    size_t head;
    size_t tail;
    size_t count;
} utils_ring_buffer_t;

bool utils_ring_buffer_init(utils_ring_buffer_t* rb, uint8_t* buffer, size_t size);
bool utils_ring_buffer_put(utils_ring_buffer_t* rb, uint8_t byte);
bool utils_ring_buffer_get(utils_ring_buffer_t* rb, uint8_t* byte);

// =============================================================================
// DEBUGGING UTILITIES
// =============================================================================

void utils_debug_printf(const char* format, ...);
void utils_hex_dump(const uint8_t* data, size_t length, const char* prefix);

#ifdef __cplusplus
}
#endif

#endif // UTILS_H
