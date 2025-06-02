# CKOS UX Overhaul Design Plan
## Comprehensive User Experience Redesign for 128x64 Display

### Current Problems Identified:
1. **Inconsistent button mappings** across screens (A/B buttons do different things)
2. **Poor information hierarchy** with scattered, overlapping elements
3. **Confusing navigation flows** with no clear back path or context
4. **Overcrowded screens** trying to show too much at once
5. **Unclear state indicators** and missing visual feedback
6. **Inefficient space usage** with excessive borders and poor layout

---

## 🎯 **NEW UX DESIGN PRINCIPLES**

### 1. **Consistent Navigation Pattern**
- **A Button**: Always PRIMARY ACTION (Select/Confirm/Enter)
- **B Button**: Always SECONDARY ACTION (Back/Cancel/Exit)  
- **Up/Down**: Always VERTICAL NAVIGATION (Menu items, lists, values)
- **Left/Right**: Always HORIZONTAL NAVIGATION (Tabs, time, settings)

### 2. **Screen Layout Standard**
```
┌─────────────────────────────────────────────────┐ ← Title bar (12px)
│ [Icon] Screen Name              Battery [██▒▒] │
├─────────────────────────────────────────────────┤
│                                                 │ ← Content area (40px)
│        PRIMARY CONTENT AREA                     │
│                                                 │
├─────────────────────────────────────────────────┤
│ A: Action Name          B: Back                 │ ← Button hints (12px)
└─────────────────────────────────────────────────┘
```

### 3. **Information Hierarchy**
- **Level 1**: Current state/primary information (large, center)
- **Level 2**: Secondary details (smaller, sides) 
- **Level 3**: Status indicators (minimal, corners)
- **Level 4**: Navigation hints (bottom bar)

---

## 🗂️ **NEW NAVIGATION HIERARCHY**

```
┌─ 🏠 HOME SCREEN (Lock Status if locked, Menu if unlocked)
│
├─ 🔐 LOCK SYSTEM
│  ├─ Agent Lock
│  │  ├─ Select Agent (Rookie/Veteran/Warden)
│  │  ├─ Agent Chat
│  │  └─ Lock Configuration
│  ├─ Custom Timer
│  │  ├─ Set Duration
│  │  ├─ Game Options
│  │  └─ Confirm & Lock
│  └─ Keyholder
│     ├─ PIN Mode
│     └─ Remote Mode
│
├─ ⚙️ SETTINGS
│  ├─ Display Settings
│  ├─ Time & Date
│  ├─ Audio Settings
│  ├─ Power Settings
│  └─ About Device
│
├─ 📊 STATUS
│  ├─ Lock History
│  ├─ Battery Info
│  └─ System Diagnostics
│
└─ 🆘 EMERGENCY
   └─ Emergency Release
```

---

## 🎨 **REDESIGNED SCREEN LAYOUTS**

### **Home Screen (Lock Status)**
```
┌─────────────────────────────────────────────────┐
│ 🔐 Lock Status              [██▒▒] 85%          │
├─────────────────────────────────────────────────┤
│                                                 │
│              ⏰ 2h 34m                          │
│           remaining                             │
│                                                 │
│        Agent: Rookie (😊 Happy)                │
│                                                 │
├─────────────────────────────────────────────────┤
│ A: Chat with Agent      B: Emergency           │
└─────────────────────────────────────────────────┘
```

### **Main Menu (Unlocked State)**
```
┌─────────────────────────────────────────────────┐
│ 🏠 Main Menu                [██▒▒] 85%          │
├─────────────────────────────────────────────────┤
│                                                 │
│  ► 🔐 Agent Lock                                │
│    ⏲️ Custom Timer                              │
│    🔑 Keyholder Lock                             │
│    ⚙️ Settings                                   │
│                                                 │
├─────────────────────────────────────────────────┤
│ A: Select                   ↕️: Navigate         │
└─────────────────────────────────────────────────┘
```

### **Agent Selection**
```
┌─────────────────────────────────────────────────┐
│ 👥 Select Agent             [██▒▒] 85%          │
├─────────────────────────────────────────────────┤
│                                                 │
│  ► 😊 Rookie    (Friendly & Supportive)        │
│    😐 Veteran   (Balanced Experience)           │
│    😤 Warden    (Strict & Demanding)            │
│                                                 │
│                                                 │
├─────────────────────────────────────────────────┤
│ A: Choose Agent             B: Back             │
└─────────────────────────────────────────────────┘
```

### **Agent Chat Interface**
```
┌─────────────────────────────────────────────────┐
│ 💬 Chat: Rookie             😊 Happy            │
├─────────────────────────────────────────────────┤
│                                                 │
│ Agent: "Ready to lock up?                      │
│ How long were you thinking?"                   │
│                                                 │
│  ► 30 minutes                                   │
│    1 hour                                       │
│    2 hours                                      │
├─────────────────────────────────────────────────┤
│ A: Select                   B: Back             │
└─────────────────────────────────────────────────┘
```

---

## ⚡ **IMPLEMENTATION PLAN**

### **Phase 1: Core Navigation Framework**
1. **Create consistent button handler** that works across all screens
2. **Implement standard screen layout components**:
   - `ui_draw_title_bar(title, battery_percent)`
   - `ui_draw_button_hints(primary_action, secondary_action)`
   - `ui_draw_menu_list(items[], selected_index)`
3. **Remove automatic transitions** and add proper debouncing

### **Phase 2: Screen Redesigns**
1. **Home/Status screens** - Clear lock state and primary actions
2. **Menu systems** - Consistent navigation with proper hierarchy
3. **Agent interface** - Simplified chat with clear options
4. **Settings screens** - Progressive disclosure, one setting per screen

### **Phase 3: Visual Polish**
1. **Icons and indicators** - Simple pixel art icons for clarity
2. **Better spacing** - Optimal use of 128x64 pixels
3. **Visual feedback** - Button press acknowledgment, loading states
4. **Error handling** - Clear error messages and recovery options

---

## 🔧 **TECHNICAL IMPLEMENTATION DETAILS**

### **Button Debouncing**
```c
#define BUTTON_DEBOUNCE_MS 150
static uint32_t last_button_time[BSP_BUTTON_COUNT] = {0};

bool is_button_debounced(bsp_button_id_t button) {
    uint32_t now = bsp_get_tick_ms();
    if (now - last_button_time[button] < BUTTON_DEBOUNCE_MS) {
        return false;
    }
    last_button_time[button] = now;
    return true;
}
```

### **Consistent Screen Structure**
```c
typedef struct {
    const char* title;
    const char* primary_action;
    const char* secondary_action;
    void (*render_content)(void);
    void (*handle_primary)(void);
    void (*handle_secondary)(void);
} screen_definition_t;
```

### **Navigation Stack**
```c
typedef struct {
    screen_id_t screen_stack[8];
    int stack_depth;
} navigation_context_t;

void nav_push_screen(screen_id_t screen);
void nav_pop_screen(void);
void nav_goto_home(void);
```

---

## 📋 **SUCCESS METRICS**

1. **Navigation Consistency**: Same buttons do same things on every screen
2. **Information Clarity**: One primary task per screen, clear hierarchy
3. **User Feedback**: Every button press has immediate visual response
4. **Context Awareness**: Always know current location and how to go back
5. **Space Efficiency**: Maximum information in minimum pixels
6. **Error Prevention**: Clear states, no dead ends, proper validation

This redesign transforms the CKOS interface from a confusing collection of screens into a coherent, usable system that respects the constraints of the 128x64 display while providing a smooth, intuitive user experience.