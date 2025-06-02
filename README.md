# CKOS - Chastity Keyholding Operating System

**CKOS** is a sophisticated firmware for STM32L452-based chastity devices, featuring an advanced agent interaction system, comprehensive security features, and real-time operating system capabilities.

## 🚀 Features

### Agent Interaction System
- **4-dimensional mood engine** with strictness, affection, patience, and trust parameters
- **Escalating request chains** that evolve based on user behavior
- **Dynamic relationship building** with persistent agent personalities
- **Proactive agent behaviors** including spontaneous offers and milestone celebrations
- **Context-aware dialog system** with mood-influenced responses

### Core Functionality
- **Multi-tier lock system** (Beginner, Advanced, Permanent)
- **Game system** with adaptive difficulty and trust-gated content
- **Power management** with multiple sleep modes
- **Secure EEPROM storage** for agent state persistence
- **Hardware abstraction layer** for easy hardware customization

### User Interface
- **SSD1306 OLED display** support (128x64)
- **Intuitive navigation** with D-pad and action buttons
- **Dynamic option generation** based on relationship state
- **Visual agent mood representation**

## 🏗️ Architecture

CKOS is built on **FreeRTOS** with a modular task-based architecture:

- **ApplicationLogic_Task**: Main state machine and agent system
- **Display_Task**: UI rendering and user interface management
- **HardwareService_Task**: Hardware abstraction and device drivers
- **Power management** and inter-task communication systems

## 📋 Hardware Requirements

- **MCU**: STM32L452CEUx (ARM Cortex-M4F)
- **Display**: SSD1306 OLED (128x64, I2C)
- **Input**: 5-button interface (D-pad + select/back)
- **Storage**: Internal EEPROM emulation
- **Security**: Hardware-based lock mechanism

## 🛠️ Development Setup

### Prerequisites
- **STM32CubeIDE** (latest version)
- **STM32CubeMX** (included with IDE)
- **GCC ARM toolchain** (included with IDE)

### Building the Project
1. Clone this repository
2. Open `CKOS_V2.ioc` in STM32CubeMX to regenerate code if needed
3. Open the project in STM32CubeIDE
4. Build and flash to your STM32L452 device

For detailed build instructions, see [`Doccumentation/Build_Flash_Debug_Guide.txt`](Doccumentation/Build_Flash_Debug_Guide.txt)

## 📚 Documentation

Comprehensive documentation is available in the [`Doccumentation/`](Doccumentation/) directory:

- **[Architecture Overview](Doccumentation/architecture.txt)** - System design and component relationships
- **[Agent System Design](Doccumentation/Agent_System_Design.txt)** - Detailed agent interaction system
- **[UI System Design](Doccumentation/UI_System_Design.txt)** - User interface architecture
- **[Lock System Design](Doccumentation/Lock_System_Design.txt)** - Security and lock management
- **[Game System Design](Doccumentation/Game_System_Design.txt)** - Interactive game features
- **[Power Management](Doccumentation/Power_Management_System_Design.txt)** - Battery optimization
- **[Hardware Service Layer](Doccumentation/Hardware_Service_Layer_Design.txt)** - HAL and drivers
- **[Memory Management](Doccumentation/Memory_Layout_and_Management.txt)** - Memory allocation strategy

## 🎮 Agent Personalities

CKOS features three distinct agent personalities, each with unique characteristics:

### Rookie (Beginner-Friendly)
- **High affection, low strictness**
- Encouraging and supportive
- Patient with new users
- Focus on positive reinforcement

### Veteran (Balanced)
- **Moderate all dimensions**
- Professional but friendly
- Dynamic emotional responses
- Challenges users appropriately

### Warden (Strict)
- **High strictness, low affection**
- Cold and professional
- Minimal patience
- Focused on discipline

## 🔐 Security Features

- **Hardware-enforced locking** mechanism
- **Secure state persistence** in EEPROM
- **Tamper detection** and response
- **Power-loss protection** for critical data
- **Trust-based access control** system

## 🎯 Request System

Users can interact with agents through various request types:

- **Time modification** (decrease/increase with escalation chains)
- **Early unlock** requests (trust-gated emergency protocols)
- **Hygiene breaks** (need-based with promise system)
- **Game interactions** (relationship-building activities)

Each request type features multiple escalation levels and context-aware responses.

## 🔄 Development Status

- ✅ **Core Architecture** - Complete
- ✅ **Agent System Design** - Complete  
- ✅ **Documentation** - Comprehensive
- 🚧 **Implementation** - In Progress
- ⏳ **Testing** - Planned
- ⏳ **Hardware Integration** - Planned

## 🤝 Contributing

This is a specialized embedded project. Please review the documentation thoroughly before contributing. See the testing strategy in [`Doccumentation/Testing_Strategy_and_QA.txt`](Doccumentation/Testing_Strategy_and_QA.txt).

## 📄 License

This project is intended for educational and research purposes. Please use responsibly and in accordance with local laws and regulations.

## ⚠️ Disclaimer

This firmware is designed for consensual adult use only. Users are responsible for ensuring safe and appropriate usage. The developers assume no liability for misuse or hardware malfunctions.

---

**Built with**: STM32CubeIDE, FreeRTOS, STM32L4 HAL, U8g2 Graphics Library 