// CKOS Utility Functions
// Common utility functions for the CKOS application

#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// STRING UTILITIES
// =============================================================================

// Safe string copy with null termination guarantee
char* utils_safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return dest;
    }
    
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
    return dest;
}

// Safe string concatenation
char* utils_safe_strcat(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return dest;
    }
    
    size_t dest_len = strlen(dest);
    if (dest_len >= dest_size - 1) {
        return dest; // No space left
    }
    
    strncat(dest, src, dest_size - dest_len - 1);
    return dest;
}

// =============================================================================
// MATHEMATICAL UTILITIES
// =============================================================================

// Clamp a value between min and max
int utils_clamp_int(int value, int min_val, int max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

float utils_clamp_float(float value, float min_val, float max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// Linear interpolation
float utils_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// =============================================================================
// TIME UTILITIES
// =============================================================================

// Convert seconds to human-readable time string
void utils_seconds_to_time_string(uint32_t seconds, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size < 9) { // Need at least "HH:MM:SS\0"
        return;
    }
    
    uint32_t hours = seconds / 3600;
    uint32_t minutes = (seconds % 3600) / 60;
    uint32_t secs = seconds % 60;
    
    if (hours > 99) {
        snprintf(buffer, buffer_size, "99:59:59"); // Max display
    } else {
        snprintf(buffer, buffer_size, "%02lu:%02lu:%02lu", 
                (unsigned long)hours, (unsigned long)minutes, (unsigned long)secs);
    }
}

// Convert time string to seconds
uint32_t utils_time_string_to_seconds(const char* time_str) {
    if (!time_str) return 0;
    
    uint32_t hours = 0, minutes = 0, seconds = 0;
    int parsed = sscanf(time_str, "%lu:%lu:%lu", 
                       (unsigned long*)&hours, 
                       (unsigned long*)&minutes, 
                       (unsigned long*)&seconds);
    
    if (parsed >= 1) {
        return hours * 3600 + (parsed >= 2 ? minutes * 60 : 0) + 
               (parsed >= 3 ? seconds : 0);
    }
    
    return 0;
}

// =============================================================================
// CRC UTILITIES
// =============================================================================

// Simple CRC16 calculation for data integrity
static const uint16_t crc16_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    // ... (full table would be here in production)
    // Abbreviated for space
};

uint16_t utils_crc16(const uint8_t* data, size_t length) {
    if (!data) return 0;
    
    uint16_t crc = 0xFFFF;
    
    for (size_t i = 0; i < length; i++) {
        uint8_t tbl_idx = ((crc >> 8) ^ data[i]) & 0xFF;
        crc = ((crc << 8) ^ crc16_table[tbl_idx]) & 0xFFFF;
    }
    
    return crc;
}

// =============================================================================
// MEMORY UTILITIES
// =============================================================================

// Secure memory clear (prevents optimization)
void utils_secure_memset(void* ptr, int value, size_t size) {
    if (!ptr || size == 0) return;
    
    volatile uint8_t* volatile_ptr = (volatile uint8_t*)ptr;
    while (size--) {
        *volatile_ptr++ = (uint8_t)value;
    }
}

// =============================================================================
// VALIDATION UTILITIES
// =============================================================================

// Validate PIN format (4-8 digits)
bool utils_validate_pin(const char* pin) {
    if (!pin) return false;
    
    size_t len = strlen(pin);
    if (len < 4 || len > 8) return false;
    
    for (size_t i = 0; i < len; i++) {
        if (pin[i] < '0' || pin[i] > '9') {
            return false;
        }
    }
    
    return true;
}

// Validate timezone offset (-12 to +12)
bool utils_validate_timezone(int offset_hours) {
    return (offset_hours >= -12 && offset_hours <= 12);
}

// =============================================================================
// FORMATTING UTILITIES
// =============================================================================

// Format battery percentage
void utils_format_battery(float percentage, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size < 5) return; // Need at least "100%\0"
    
    int percent = (int)(percentage + 0.5f); // Round to nearest integer
    percent = utils_clamp_int(percent, 0, 100);
    
    snprintf(buffer, buffer_size, "%d%%", percent);
}

// Format file size in human-readable format
void utils_format_file_size(uint32_t bytes, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size < 8) return; // Need space for "999.9KB\0"
    
    if (bytes < 1024) {
        snprintf(buffer, buffer_size, "%luB", (unsigned long)bytes);
    } else if (bytes < 1024 * 1024) {
        float kb = bytes / 1024.0f;
        snprintf(buffer, buffer_size, "%.1fKB", kb);
    } else {
        float mb = bytes / (1024.0f * 1024.0f);
        snprintf(buffer, buffer_size, "%.1fMB", mb);
    }
}

// =============================================================================
// RING BUFFER UTILITY
// =============================================================================

// Initialize ring buffer
bool utils_ring_buffer_init(utils_ring_buffer_t* rb, uint8_t* buffer, size_t size) {
    if (!rb || !buffer || size == 0) return false;
    
    rb->buffer = buffer;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    
    return true;
}

// Add byte to ring buffer
bool utils_ring_buffer_put(utils_ring_buffer_t* rb, uint8_t byte) {
    if (!rb || rb->count >= rb->size) return false;
    
    rb->buffer[rb->tail] = byte;
    rb->tail = (rb->tail + 1) % rb->size;
    rb->count++;
    
    return true;
}

// Get byte from ring buffer
bool utils_ring_buffer_get(utils_ring_buffer_t* rb, uint8_t* byte) {
    if (!rb || !byte || rb->count == 0) return false;
    
    *byte = rb->buffer[rb->head];
    rb->head = (rb->head + 1) % rb->size;
    rb->count--;
    
    return true;
}

// =============================================================================
// DEBUGGING UTILITIES
// =============================================================================

// Debug print with timestamp
void utils_debug_printf(const char* format, ...) {
    if (!format) return;
    
    // Simple timestamp (would use BSP timing in real implementation)
    printf("[DEBUG] ");
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("\n");
}

// Hex dump utility for debugging
void utils_hex_dump(const uint8_t* data, size_t length, const char* prefix) {
    if (!data || length == 0) return;
    
    if (prefix) {
        printf("%s", prefix);
    }
    
    for (size_t i = 0; i < length; i++) {
        if (i > 0 && i % 16 == 0) {
            printf("\n");
            if (prefix) printf("%s", prefix);
        }
        printf("%02X ", data[i]);
    }
    printf("\n");
}