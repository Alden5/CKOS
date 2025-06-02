# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and Development Commands

**Primary IDE:** STM32CubeIDE (required for this embedded project)

### Building the Project
```bash
# Build via STM32CubeIDE (preferred method)
# Project -> Build Project (or Ctrl+B / Cmd+B)
# Select Debug or Release configuration
```

### Asset Conversion Scripts
```bash
# Convert PBM images to C arrays (run when images change)
python Tools/Scripts/convert_pbm_to_c_array.py <input_pbm> <output_c_file> <c_array_name>

# Generate state machine table (run when state machine definition changes)
python Tools/Scripts/generate_state_table.py <input_csv_json> <output_c> <output_h>
```

### Flashing Firmware
```bash
# Using STM32CubeProgrammer CLI (device must be in DFU mode)
STM32_Programmer_CLI.exe -c port=USB1 -e all -w "path/to/CKOS_V2.bin" 0x08000000 -v
```

**DFU Mode Entry:** Physical access required - press internal DFU button while powering on device

## Architecture Overview

CKOS is firmware for STM32L452-based chastity devices with a sophisticated agent interaction system.

### Core Architecture
- **Real-time OS:** FreeRTOS with three-task architecture
- **Target Hardware:** STM32L452CEUx (ARM Cortex-M4F, 512KB Flash, 160KB SRAM)
- **Display:** ERC12864F7-4 LCD (128x64) with ST7565R controller via U8g2 graphics library
- **Input:** 5-button interface (D-pad + select/back)

### Three-Task System
1. **HardwareService_Task** (Highest Priority): Direct hardware control, sensors, memory wire actuation
2. **ApplicationLogic_Task** (Medium Priority): State machine, agent AI, lock logic, user interaction processing
3. **Display_Task** (Lower Priority): UI rendering, animations, framebuffer management

### Key Design Patterns
- **Active Object Pattern:** Each task has dedicated message queues and event processing
- **Table-Driven State Machine:** State transitions defined in generated tables from CSV/JSON
- **Hardware Abstraction via BSP:** All hardware access through Board Support Package layer
- **Memory Protection Unit (MPU):** Enabled for stack overflow and NULL pointer protection
- **Reference-Counted Sleep Suppression:** Prevents deep sleep during critical operations

### Inter-Task Communication
- **Message Passing:** RTOS queues for data exchange between tasks
- **Task Notifications:** For simple signaling and events
- **No Shared Memory:** Strict ownership model, data copied in messages

### Memory Management
- **Static Slab Allocation:** Fixed memory pools, no malloc/free
- **Stack Monitoring:** FreeRTOS stack overflow detection enabled
- **Flash Layout:** ~60-65% code, ~10% assets, 25% headroom for updates

## Key Source File Organization

```
App/
├── Config/           # System configuration headers (app_config.h, memory_pools.h, etc.)
├── Kernel/          # FreeRTOS hooks, memory pools, task management
├── BSP/             # Hardware abstraction layer (bsp_pins.h, bsp_drivers.h/.c)
├── AppLogic/        # State machine, agent system, lock logic
├── Display/         # UI rendering, animations, asset management
│   └── Assets/      # Compiled C arrays from PBM images
├── Hardware/        # Hardware services, sensors, storage, power management
└── Utils/           # Utilities (CRC, ring buffer, logging)

Core/                # STM32CubeMX auto-generated code
Drivers/             # STM32 HAL/LL drivers
Middlewares/         # FreeRTOS source, U8g2 graphics library
Assets_Src/          # Source PBM images before conversion
Documentation/       # Comprehensive design documents
```

## Development Guidelines

### Code Style
- **Types:** UpperCamelCase
- **Functions:** snake_case
- **Macros:** UPPER_SNAKE_CASE
- **Hardware Access:** Exclusively via BSP layer (never direct HAL calls)

### Critical Development Rules
1. **Power-Aware Programming:** Minimize ISR computation, display updates, use RTOS idle hook
2. **Memory Efficiency:** Use memory pools, monitor usage, avoid recursion
3. **Layer Separation:** ApplicationLogic ↔ SystemComponents ↔ BSP ↔ HAL
4. **Static Analysis:** MISRA C compliance required, zero tolerance for critical issues

### Testing Strategy
- **Unit Testing:** Unity framework on host
- **Integration Testing:** Host-side with FFF mocks
- **Hardware-in-Loop:** STM32L4 target testing
- **Coverage Targets:** 70% line coverage for core logic, 50% overall

## Agent System Overview

The device features three agent personalities with 4-dimensional mood engine:
- **Rookie:** High affection, low strictness (beginner-friendly)
- **Veteran:** Balanced dimensions (moderate user)  
- **Warden:** High strictness, low affection (strict mode)

Agent behaviors influence lock management, user interactions, and unlock conditions based on trust levels and interaction history.

## Lock System Types

1. **Agent Lock:** Interactive with selected agent personality
2. **Custom Lock:** User-configured duration with optional games
3. **Keyholder Lock:** 
   - Basic PIN mode
   - Remote web service integration with rolling unlock codes

## Power Management

- **Expected Battery Life:** 1-2 months typical usage, 6 months standby
- **Deep Sleep Modes:** STOP2 mode with RTC wake, MPU-protected critical operations
- **Power States:** Mapped to application states (MENU, LOCK_ACTIVE, IDLE, etc.)
- **Low Battery Protection:** Prevents unlock attempts if insufficient power

## Hardware Integration Notes

### Display System (ERC12864F7-4)
- **Controller:** ST7565R with native u8g2 support (`u8x8_d_st7565_erc12864`)
- **Interface:** 4-wire SPI at 4MHz, SPI mode 0
- **Framebuffer:** 1024 bytes (128x64 monochrome, full buffer mode)
- **BSP Integration:** Custom callbacks for STM32L4 GPIO/SPI control
- **Alternative Config:** `erc12864_alt` available for bias/contrast optimization

### Memory Wire Unlock System
- **Memory Wire Unlock:** Shape memory alloy (Nitinol) heated to 70°C for mechanical release
- **Dual Sensors:** Latch position sensor and door presence sensor for reliable operation
- **Temperature Compensation:** Required for consistent memory wire performance
- **USB-C Interface:** DFU mode for firmware updates and charging

## Debugging Features

- **FreeRTOS Awareness:** STM32CubeIDE plugin for task monitoring
- **SWV/ITM Tracing:** Printf redirection via Serial Wire Viewer
- **MPU Fault Analysis:** Hardware-assisted memory protection debugging
- **LOG() Macro System:** Severity-based conditional logging

This embedded project requires STM32CubeIDE for development and uses specialized hardware that may not be available for general testing.