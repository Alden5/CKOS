// BSP Simulator Implementation (Single-threaded)
// SDL2-based implementation of the BSP API for host-side simulation
// This version avoids threading issues by running everything on the main thread

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <time.h>
#include <unistd.h>
#include "../App/BSP/bsp_api.h"

// =============================================================================
// SIMULATOR STATE
// =============================================================================

typedef struct {
    // SDL components
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* display_texture;
    
    // Display framebuffer (matches hardware: 128x64 monochrome)
    uint8_t framebuffer[BSP_DISPLAY_WIDTH * BSP_DISPLAY_HEIGHT / 8];
    
    // Simulated hardware state
    float battery_percentage;
    float battery_voltage;
    bool charging_active;
    float temperature_celsius;
    bool door_closed;
    bool latch_engaged;
    
    // Lock state simulation
    bsp_lock_state_t lock_state;
    
    // Power management
    bsp_power_mode_t power_mode;
    
    // Initialization state
    bool initialized;
} simulator_state_t;

static simulator_state_t g_sim_state = {0};

// Complete 6x8 font for text rendering - includes all printable ASCII characters
static const uint8_t font_6x8[][6] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Space (32)
    {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00}, // ! (33)
    {0x00, 0x07, 0x00, 0x07, 0x00, 0x00}, // " (34)
    {0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00}, // # (35)
    {0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00}, // $ (36)
    {0x23, 0x13, 0x08, 0x64, 0x62, 0x00}, // % (37)
    {0x36, 0x49, 0x55, 0x22, 0x50, 0x00}, // & (38)
    {0x00, 0x05, 0x03, 0x00, 0x00, 0x00}, // ' (39)
    {0x00, 0x1C, 0x22, 0x41, 0x00, 0x00}, // ( (40)
    {0x00, 0x41, 0x22, 0x1C, 0x00, 0x00}, // ) (41)
    {0x14, 0x08, 0x3E, 0x08, 0x14, 0x00}, // * (42)
    {0x08, 0x08, 0x3E, 0x08, 0x08, 0x00}, // + (43)
    {0x00, 0x50, 0x30, 0x00, 0x00, 0x00}, // , (44)
    {0x08, 0x08, 0x08, 0x08, 0x08, 0x00}, // - (45)
    {0x00, 0x60, 0x60, 0x00, 0x00, 0x00}, // . (46)
    {0x20, 0x10, 0x08, 0x04, 0x02, 0x00}, // / (47)
    {0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00}, // 0 (48)
    {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00}, // 1 (49)
    {0x42, 0x61, 0x51, 0x49, 0x46, 0x00}, // 2 (50)
    {0x21, 0x41, 0x45, 0x4B, 0x31, 0x00}, // 3 (51)
    {0x18, 0x14, 0x12, 0x7F, 0x10, 0x00}, // 4 (52)
    {0x27, 0x45, 0x45, 0x45, 0x39, 0x00}, // 5 (53)
    {0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00}, // 6 (54)
    {0x01, 0x71, 0x09, 0x05, 0x03, 0x00}, // 7 (55)
    {0x36, 0x49, 0x49, 0x49, 0x36, 0x00}, // 8 (56)
    {0x06, 0x49, 0x49, 0x29, 0x1E, 0x00}, // 9 (57)
    {0x00, 0x36, 0x36, 0x00, 0x00, 0x00}, // : (58)
    {0x00, 0x56, 0x36, 0x00, 0x00, 0x00}, // ; (59)
    {0x08, 0x14, 0x22, 0x41, 0x00, 0x00}, // < (60)
    {0x14, 0x14, 0x14, 0x14, 0x14, 0x00}, // = (61)
    {0x00, 0x41, 0x22, 0x14, 0x08, 0x00}, // > (62)
    {0x02, 0x01, 0x51, 0x09, 0x06, 0x00}, // ? (63)
    {0x32, 0x49, 0x79, 0x41, 0x3E, 0x00}, // @ (64)
    {0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00}, // A (65)
    {0x7F, 0x49, 0x49, 0x49, 0x36, 0x00}, // B (66)
    {0x3E, 0x41, 0x41, 0x41, 0x22, 0x00}, // C (67)
    {0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00}, // D (68)
    {0x7F, 0x49, 0x49, 0x49, 0x41, 0x00}, // E (69)
    {0x7F, 0x09, 0x09, 0x09, 0x01, 0x00}, // F (70)
    {0x3E, 0x41, 0x49, 0x49, 0x7A, 0x00}, // G (71)
    {0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00}, // H (72)
    {0x00, 0x41, 0x7F, 0x41, 0x00, 0x00}, // I (73)
    {0x20, 0x40, 0x41, 0x3F, 0x01, 0x00}, // J (74)
    {0x7F, 0x08, 0x14, 0x22, 0x41, 0x00}, // K (75)
    {0x7F, 0x40, 0x40, 0x40, 0x40, 0x00}, // L (76)
    {0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00}, // M (77)
    {0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00}, // N (78)
    {0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00}, // O (79)
    {0x7F, 0x09, 0x09, 0x09, 0x06, 0x00}, // P (80)
    {0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00}, // Q (81)
    {0x7F, 0x09, 0x19, 0x29, 0x46, 0x00}, // R (82)
    {0x46, 0x49, 0x49, 0x49, 0x31, 0x00}, // S (83)
    {0x01, 0x01, 0x7F, 0x01, 0x01, 0x00}, // T (84)
    {0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00}, // U (85)
    {0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00}, // V (86)
    {0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00}, // W (87)
    {0x63, 0x14, 0x08, 0x14, 0x63, 0x00}, // X (88)
    {0x07, 0x08, 0x70, 0x08, 0x07, 0x00}, // Y (89)
    {0x61, 0x51, 0x49, 0x45, 0x43, 0x00}, // Z (90)
    {0x00, 0x7F, 0x41, 0x41, 0x00, 0x00}, // [ (91)
    {0x02, 0x04, 0x08, 0x10, 0x20, 0x00}, // \\ (92)
    {0x00, 0x41, 0x41, 0x7F, 0x00, 0x00}, // ] (93)
    {0x04, 0x02, 0x01, 0x02, 0x04, 0x00}, // ^ (94)
    {0x40, 0x40, 0x40, 0x40, 0x40, 0x00}, // _ (95)
    {0x00, 0x01, 0x02, 0x04, 0x00, 0x00}, // ` (96)
    {0x20, 0x54, 0x54, 0x54, 0x78, 0x00}, // a (97)
    {0x7F, 0x48, 0x44, 0x44, 0x38, 0x00}, // b (98)
    {0x38, 0x44, 0x44, 0x44, 0x20, 0x00}, // c (99)
    {0x38, 0x44, 0x44, 0x48, 0x7F, 0x00}, // d (100)
    {0x38, 0x54, 0x54, 0x54, 0x18, 0x00}, // e (101)
    {0x08, 0x7E, 0x09, 0x01, 0x02, 0x00}, // f (102)
    {0x0C, 0x52, 0x52, 0x52, 0x3E, 0x00}, // g (103)
    {0x7F, 0x08, 0x04, 0x04, 0x78, 0x00}, // h (104)
    {0x00, 0x44, 0x7D, 0x40, 0x00, 0x00}, // i (105)
    {0x20, 0x40, 0x44, 0x3D, 0x00, 0x00}, // j (106)
    {0x7F, 0x10, 0x28, 0x44, 0x00, 0x00}, // k (107)
    {0x00, 0x41, 0x7F, 0x40, 0x00, 0x00}, // l (108)
    {0x7C, 0x04, 0x18, 0x04, 0x78, 0x00}, // m (109)
    {0x7C, 0x08, 0x04, 0x04, 0x78, 0x00}, // n (110)
    {0x38, 0x44, 0x44, 0x44, 0x38, 0x00}, // o (111)
    {0x7C, 0x14, 0x14, 0x14, 0x08, 0x00}, // p (112)
    {0x08, 0x14, 0x14, 0x18, 0x7C, 0x00}, // q (113)
    {0x7C, 0x08, 0x04, 0x04, 0x08, 0x00}, // r (114)
    {0x48, 0x54, 0x54, 0x54, 0x20, 0x00}, // s (115)
    {0x04, 0x3F, 0x44, 0x40, 0x20, 0x00}, // t (116)
    {0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00}, // u (117)
    {0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00}, // v (118)
    {0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00}, // w (119)
    {0x44, 0x28, 0x10, 0x28, 0x44, 0x00}, // x (120)
    {0x0C, 0x50, 0x50, 0x50, 0x3C, 0x00}, // y (121)
    {0x44, 0x64, 0x54, 0x4C, 0x44, 0x00}, // z (122)
    {0x00, 0x08, 0x36, 0x41, 0x00, 0x00}, // { (123)
    {0x00, 0x00, 0x7F, 0x00, 0x00, 0x00}, // | (124)
    {0x00, 0x41, 0x36, 0x08, 0x00, 0x00}, // } (125)
    {0x10, 0x08, 0x08, 0x10, 0x08, 0x00}, // ~ (126)
};

// =============================================================================
// DISPLAY IMPLEMENTATION
// =============================================================================

int bsp_display_init(void) {
    if (g_sim_state.initialized) {
        return 0; // Already initialized
    }
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        printf("ERROR: SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }
    
    // Create window with 4x scaling for visibility
    g_sim_state.window = SDL_CreateWindow(
        "CKOS Simulator - STM32L452 ERC12864F7-4 Display (128x64)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        BSP_DISPLAY_WIDTH * 4,
        BSP_DISPLAY_HEIGHT * 4,
        SDL_WINDOW_SHOWN
    );
    
    if (!g_sim_state.window) {
        printf("ERROR: SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    
    g_sim_state.renderer = SDL_CreateRenderer(g_sim_state.window, -1, 
                                             SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_sim_state.renderer) {
        printf("ERROR: SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(g_sim_state.window);
        SDL_Quit();
        return -1;
    }
    
    g_sim_state.display_texture = SDL_CreateTexture(g_sim_state.renderer,
                                                   SDL_PIXELFORMAT_RGBA8888,
                                                   SDL_TEXTUREACCESS_STREAMING,
                                                   BSP_DISPLAY_WIDTH, BSP_DISPLAY_HEIGHT);
    if (!g_sim_state.display_texture) {
        printf("ERROR: SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(g_sim_state.renderer);
        SDL_DestroyWindow(g_sim_state.window);
        SDL_Quit();
        return -1;
    }
    
    // Initialize framebuffer
    memset(g_sim_state.framebuffer, 0, sizeof(g_sim_state.framebuffer));
    
    // Initialize simulated hardware state
    g_sim_state.battery_percentage = 85.0f;
    g_sim_state.battery_voltage = 3.7f;
    g_sim_state.charging_active = false;
    g_sim_state.temperature_celsius = 23.5f;
    g_sim_state.door_closed = true;
    g_sim_state.latch_engaged = true;
    g_sim_state.lock_state = BSP_LOCK_STATE_LOCKED;
    g_sim_state.power_mode = BSP_POWER_MODE_RUN;
    
    g_sim_state.initialized = true;
    printf("BSP Display initialized successfully\n");
    return 0;
}

void bsp_display_cleanup(void) {
    if (!g_sim_state.initialized) return;
    
    if (g_sim_state.display_texture) {
        SDL_DestroyTexture(g_sim_state.display_texture);
        g_sim_state.display_texture = NULL;
    }
    
    if (g_sim_state.renderer) {
        SDL_DestroyRenderer(g_sim_state.renderer);
        g_sim_state.renderer = NULL;
    }
    
    if (g_sim_state.window) {
        SDL_DestroyWindow(g_sim_state.window);
        g_sim_state.window = NULL;
    }
    
    SDL_Quit();
    g_sim_state.initialized = false;
}

void bsp_display_clear(void) {
    memset(g_sim_state.framebuffer, 0, sizeof(g_sim_state.framebuffer));
}

void bsp_display_refresh(void) {
    if (!g_sim_state.renderer || !g_sim_state.display_texture) {
        return;
    }
    
    // Convert framebuffer to SDL texture
    uint32_t pixels[BSP_DISPLAY_WIDTH * BSP_DISPLAY_HEIGHT];
    
    for (int y = 0; y < BSP_DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < BSP_DISPLAY_WIDTH; x++) {
            bool pixel_on = bsp_display_get_pixel(x, y);
            // RGBA8888 format: Black text on light gray background
            pixels[y * BSP_DISPLAY_WIDTH + x] = pixel_on ? 0x000000FF : 0xC0C0C0FF;
        }
    }
    
    SDL_UpdateTexture(g_sim_state.display_texture, NULL, pixels, BSP_DISPLAY_WIDTH * sizeof(uint32_t));
    
    // Clear renderer with dark background
    SDL_SetRenderDrawColor(g_sim_state.renderer, 32, 32, 32, 255);
    SDL_RenderClear(g_sim_state.renderer);
    
    // Render display texture scaled up
    SDL_RenderCopy(g_sim_state.renderer, g_sim_state.display_texture, NULL, NULL);
    
    SDL_RenderPresent(g_sim_state.renderer);
}

void bsp_display_set_pixel(int x, int y, bool on) {
    if (x < 0 || x >= BSP_DISPLAY_WIDTH || y < 0 || y >= BSP_DISPLAY_HEIGHT) {
        return;
    }
    
    int byte_index = (y / 8) * BSP_DISPLAY_WIDTH + x;
    int bit_index = y % 8;
    
    if (on) {
        g_sim_state.framebuffer[byte_index] |= (1 << bit_index);
    } else {
        g_sim_state.framebuffer[byte_index] &= ~(1 << bit_index);
    }
}

bool bsp_display_get_pixel(int x, int y) {
    if (x < 0 || x >= BSP_DISPLAY_WIDTH || y < 0 || y >= BSP_DISPLAY_HEIGHT) {
        return false;
    }
    
    int byte_index = (y / 8) * BSP_DISPLAY_WIDTH + x;
    int bit_index = y % 8;
    
    return (g_sim_state.framebuffer[byte_index] & (1 << bit_index)) != 0;
}

void bsp_display_draw_text(int x, int y, const char* text) {
    if (!text) return;
    
    int current_x = x;
    
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        
        // Convert to font index (ASCII 32 = space)
        int font_index = (c >= 32 && c <= 126) ? (c - 32) : 0;
        
        // Draw character
        for (int col = 0; col < 6; col++) {
            uint8_t column = font_6x8[font_index][col];
            for (int row = 0; row < 8; row++) {
                if (column & (1 << row)) {
                    bsp_display_set_pixel(current_x + col, y + row, true);
                }
            }
        }
        
        current_x += 7; // 6 pixels + 1 space
    }
}

void bsp_display_draw_text_centered(int y, const char* text) {
    if (!text) return;
    
    int text_width = strlen(text) * 7; // 6 pixels + 1 space per char
    int x = (BSP_DISPLAY_WIDTH - text_width) / 2;
    
    bsp_display_draw_text(x, y, text);
}

void bsp_display_draw_line(int x1, int y1, int x2, int y2) {
    // Simple line drawing using Bresenham's algorithm
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    int x = x1, y = y1;
    
    while (true) {
        bsp_display_set_pixel(x, y, true);
        
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void bsp_display_draw_box(int x, int y, int w, int h) {
    // Draw hollow rectangle
    bsp_display_draw_line(x, y, x + w - 1, y);           // Top
    bsp_display_draw_line(x, y + h - 1, x + w - 1, y + h - 1); // Bottom
    bsp_display_draw_line(x, y, x, y + h - 1);           // Left
    bsp_display_draw_line(x + w - 1, y, x + w - 1, y + h - 1); // Right
}

void bsp_display_draw_filled_box(int x, int y, int w, int h) {
    for (int dy = 0; dy < h; dy++) {
        bsp_display_draw_line(x, y + dy, x + w - 1, y + dy);
    }
}

// =============================================================================
// INPUT IMPLEMENTATION (Non-blocking, main thread only)
// =============================================================================

int bsp_input_init(void) {
    // Input initialization is handled by SDL_Init in display_init
    return 0;
}

void bsp_input_cleanup(void) {
    // Cleanup handled by display_cleanup
}

bool bsp_input_poll_event(bsp_button_event_t* event) {
    if (!event) return false;
    
    // Process SDL events (non-blocking, main thread only)
    SDL_Event sdl_event;
    if (SDL_PollEvent(&sdl_event)) {
        if (sdl_event.type == SDL_QUIT) {
            printf("SDL Quit event received\n");
            exit(0);
        } else if (sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) {
            bool pressed = (sdl_event.type == SDL_KEYDOWN);
            bsp_button_id_t button_id = BSP_BUTTON_COUNT; // Invalid
            
            switch (sdl_event.key.keysym.sym) {
                case SDLK_UP:    button_id = BSP_BUTTON_UP; break;
                case SDLK_DOWN:  button_id = BSP_BUTTON_DOWN; break;
                case SDLK_LEFT:  button_id = BSP_BUTTON_LEFT; break;
                case SDLK_RIGHT: button_id = BSP_BUTTON_RIGHT; break;
                case SDLK_a:     button_id = BSP_BUTTON_A; break;
                case SDLK_b:     button_id = BSP_BUTTON_B; break;
                case SDLK_ESCAPE: 
                    printf("ESC pressed - exiting simulator\n");
                    exit(0);
                    break;
            }
            
            if (button_id < BSP_BUTTON_COUNT) {
                event->button = button_id;
                event->pressed = pressed;
                event->timestamp = bsp_get_tick_ms();
                return true;
            }
        }
    }
    
    return false;
}

// =============================================================================
// TIMING IMPLEMENTATION
// =============================================================================

uint32_t bsp_get_tick_ms(void) {
    return SDL_GetTicks();
}

uint64_t bsp_get_utc_time_seconds(void) {
    return (uint64_t)time(NULL);
}

void bsp_delay_ms(uint32_t ms) {
    SDL_Delay(ms);
}

// =============================================================================
// SIMPLIFIED RTOS SIMULATION (No threading)
// =============================================================================

// For single-threaded simulation, these are simple stubs
bsp_task_handle_t bsp_task_create(bsp_task_function_t task_function, 
                                  const char* name,
                                  uint16_t stack_size,
                                  void* parameters,
                                  uint8_t priority) {
    (void)task_function; (void)stack_size; (void)parameters; (void)priority;
    printf("Created task: %s (single-threaded mode)\n", name);
    return (bsp_task_handle_t)0x1; // Dummy handle
}

void bsp_task_delete(bsp_task_handle_t task) {
    (void)task;
}

void bsp_task_delay(uint32_t ms) {
    // In single-threaded mode, this is just a regular delay
    bsp_delay_ms(ms);
}

void bsp_task_yield(void) {
    // No-op in single-threaded mode
}

void bsp_scheduler_start(void) {
    // In single-threaded mode, this is handled by main loop
    printf("Single-threaded scheduler (no-op)\n");
}

// Simplified queue implementation (single-threaded)
typedef struct {
    uint8_t* buffer;
    uint8_t item_size;
    uint8_t length;
    uint8_t head, tail, count;
} simple_queue_t;

bsp_queue_handle_t bsp_queue_create(uint8_t length, uint8_t item_size) {
    simple_queue_t* queue = malloc(sizeof(simple_queue_t));
    if (!queue) return NULL;
    
    queue->buffer = malloc(length * item_size);
    if (!queue->buffer) {
        free(queue);
        return NULL;
    }
    
    queue->item_size = item_size;
    queue->length = length;
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    
    return (bsp_queue_handle_t)queue;
}

void bsp_queue_delete(bsp_queue_handle_t queue) {
    simple_queue_t* q = (simple_queue_t*)queue;
    if (q) {
        free(q->buffer);
        free(q);
    }
}

bool bsp_queue_send(bsp_queue_handle_t queue, const void* item, uint32_t timeout_ms) {
    (void)timeout_ms; // No timeout in single-threaded mode
    
    simple_queue_t* q = (simple_queue_t*)queue;
    if (!q || q->count >= q->length) {
        return false;
    }
    
    uint8_t* dest = q->buffer + (q->tail * q->item_size);
    memcpy(dest, item, q->item_size);
    
    q->tail = (q->tail + 1) % q->length;
    q->count++;
    
    return true;
}

bool bsp_queue_receive(bsp_queue_handle_t queue, void* item, uint32_t timeout_ms) {
    (void)timeout_ms; // No timeout in single-threaded mode
    
    simple_queue_t* q = (simple_queue_t*)queue;
    if (!q || q->count == 0) {
        return false;
    }
    
    uint8_t* src = q->buffer + (q->head * q->item_size);
    memcpy(item, src, q->item_size);
    
    q->head = (q->head + 1) % q->length;
    q->count--;
    
    return true;
}

// =============================================================================
// HARDWARE SERVICES SIMULATION
// =============================================================================

int bsp_storage_read(const bsp_storage_operation_t* op) {
    (void)op;
    return 0; // Success
}

int bsp_storage_write(const bsp_storage_operation_t* op) {
    (void)op;
    return 0; // Success
}

int bsp_sensors_read(bsp_sensor_readings_t* readings) {
    if (!readings) return -1;
    
    readings->battery_voltage = g_sim_state.battery_voltage;
    readings->battery_percentage = (uint8_t)g_sim_state.battery_percentage;
    readings->temperature_celsius = g_sim_state.temperature_celsius;
    readings->door_closed = g_sim_state.door_closed;
    readings->latch_engaged = g_sim_state.latch_engaged;
    readings->charging_active = g_sim_state.charging_active;
    
    return 0;
}

int bsp_lock_unlock(void) {
    g_sim_state.lock_state = BSP_LOCK_STATE_UNLOCKING;
    printf("Simulator: Lock unlocking...\n");
    return 0;
}

bsp_lock_state_t bsp_lock_get_state(void) {
    return g_sim_state.lock_state;
}

int bsp_power_set_mode(bsp_power_mode_t mode) {
    g_sim_state.power_mode = mode;
    printf("Simulator: Power mode set to %d\n", mode);
    return 0;
}

// =============================================================================
// SLEEP SUPPRESSION (Single-threaded stubs)
// =============================================================================

void bsp_power_suppress_sleep(void) {
    // No-op in single-threaded mode
}

void bsp_power_allow_sleep(void) {
    // No-op in single-threaded mode
}

// =============================================================================
// DEBUG FUNCTIONS (Simulator-specific)
// =============================================================================

#ifdef BSP_PLATFORM_SIMULATOR
void bsp_debug_print_display(void) {
    printf("\n=== DISPLAY DEBUG (128x64) ===\n");
    for (int y = 0; y < BSP_DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < BSP_DISPLAY_WIDTH; x++) {
            bool pixel_on = bsp_display_get_pixel(x, y);
            printf("%c", pixel_on ? '#' : '.');
        }
        printf("\n");
    }
    printf("=== END DISPLAY DEBUG ===\n\n");
}

void bsp_debug_set_sensor_value(const char* sensor, float value) {
    if (strcmp(sensor, "battery") == 0) {
        g_sim_state.battery_percentage = value;
    } else if (strcmp(sensor, "temperature") == 0) {
        g_sim_state.temperature_celsius = value;
    }
    printf("Simulator: Set %s to %.2f\n", sensor, value);
}

void bsp_debug_trigger_button(bsp_button_id_t button) {
    if (button < BSP_BUTTON_COUNT) {
        bsp_button_event_t event;
        event.button = button;
        event.pressed = true;
        event.timestamp = bsp_get_tick_ms();
        // Simulate button press and release
        printf("Simulator: Triggered button %d\n", button);
    }
}
#endif