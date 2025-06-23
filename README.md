# MaplePad RP2350 - Enhanced Dreamcast Controller

A feature-rich Dreamcast controller emulator built on the Raspberry Pi RP2350 microcontroller with advanced display support, SD card functionality, and conflict-free pin assignments.

## 🚀 Features

### Core Functionality
- **Full Dreamcast Controller Emulation** - Standard HKT-7700 and Arcade HKT-7300 modes
- **Maple Bus Communication** - Native Dreamcast protocol support
- **VMU Memory Card Emulation** - Complete save game functionality
- **Multi-Display Support** - SSD1306, SSD1309, and SSD1331 OLEDs
- **SD Card Integration** - VMU save/load with external storage
- **RP2350 Optimization** - Enhanced performance and memory utilization

### Advanced Features
- **Automatic Display Detection** - Seamless switching between display types
- **Font Rendering System** - Custom bitmap font with putString/putLetter functions
- **Conflict-Free Pin Assignment** - Optimized layout eliminates hardware conflicts
- **Enhanced Menu System** - Configuration and settings interface
- **Real-time Status Display** - System information and diagnostics

## 🔧 Hardware Requirements

### Microcontroller
- **Raspberry Pi RP2350** (recommended) or RP2040
- USB programming capability
- 3.3V/5V power supply

### Display Options (Auto-detected)
- **SSD1331** - 96x64 Color OLED (SPI)
- **SSD1306** - 128x64 Monochrome OLED (I2C)  
- **SSD1309** - 128x64 Monochrome OLED (I2C)

### Storage (Optional)
- **SD Card Module** - For VMU save/load functionality
- SPI interface compatible

### Controls
- **Digital Buttons** - A, B, X, Y, Start, D-pad (8 directions)
- **Analog Inputs** - Left/Right triggers, Analog stick (X/Y)
- **Arcade Mode** - Additional C and Z buttons (HKT-7300)

## 📌 Pin Assignment (Conflict-Free)

### Controller Inputs
| Pin | Function | Description |
|-----|----------|-------------|
| GP0 | Button A | Primary action button |
| GP1 | Button B | Secondary action button |
| GP4 | Button X | Tertiary action button |
| GP5 | Button Y | Quaternary action button |
| GP6 | D-pad Up | Directional pad up |
| GP7 | D-pad Down | Directional pad down |
| GP8 | D-pad Left | Directional pad left |
| GP9 | D-pad Right | Directional pad right |
| GP10 | Start | Start/menu button |

### Dreamcast Communication
| Pin | Function | Description |
|-----|----------|-------------|
| GP11 | MAPLE_A | Maple bus data line A |
| GP12 | MAPLE_B | Maple bus data line B |

### SD Card Interface (SPI0)
| Pin | Function | Description |
|-----|----------|-------------|
| GP16 | SD_MISO | SD card data input |
| GP17 | SD_CS | SD card chip select |
| GP18 | SD_SCK | SD card SPI clock |
| GP19 | SD_MOSI | SD card data output |

### SSD1331 Color Display (SPI1)
| Pin | Function | Description |
|-----|----------|-------------|
| GP14 | SCK | SPI clock |
| GP15 | MOSI | SPI data output |
| GP20 | DC | Data/Command select |
| GP21 | RST | Display reset |

### SSD1306/SSD1309 Monochrome Display (I2C0)
| Pin | Function | Description |
|-----|----------|-------------|
| GP12 | SDA | I2C data (shared with Maple when idle) |
| GP13 | SCL | I2C clock |

### Configuration & Control
| Pin | Function | Description |
|-----|----------|-------------|
| GP22 | OLED_PIN | Display type detection |
| GP23 | PAGE_BUTTON | VMU page control |

### Analog Inputs (ADC)
| Pin | Function | Description |
|-----|----------|-------------|
| GP26 | ADC0 | Analog stick X-axis |
| GP27 | ADC1 | Analog stick Y-axis |
| GP28 | ADC2 | Left trigger (L) |
| GP29 | ADC3 | Right trigger (R) |

### Arcade Mode (HKT-7300)
| Pin | Function | Description |
|-----|----------|-------------|
| GP24 | Button C | Additional arcade button |
| GP25 | Button Z | Additional arcade button |

### Power
| Pin | Function | Description |
|-----|----------|-------------|
| 5V | Power In | USB or external 5V supply |
| 3V3 | Power Out | 3.3V for peripherals |
| GND | Ground | Multiple ground connections |

### Available for Expansion
| Pin | Status | Notes |
|-----|--------|-------|
| GP2 | Available | Future expansion |
| GP3 | Available | Future expansion |
| GP13 | Available | When not using I2C displays |

## 🛠️ Building the Firmware

### Prerequisites
```bash
# Install Pico SDK
git clone https://github.com/raspberrypi/pico-sdk.git
export PICO_SDK_PATH=/path/to/pico-sdk

# Install build tools
sudo apt install cmake gcc-arm-none-eabi ninja-build
```

### Build Process
```bash
# Clone repository
git clone [repository-url]
cd MaplePad-alter

# Create build directory
mkdir build && cd build

# Configure build
cmake ..

# Build firmware
ninja maplepad

# Output files:
# - maplepad.elf (main executable)
# - maplepad.uf2 (drag-and-drop programming)
# - maplepad.hex (Intel HEX format)
# - maplepad.bin (binary format)
```

### Programming the Device
```bash
# Method 1: UF2 (Recommended)
# 1. Hold BOOTSEL button and connect USB
# 2. Copy maplepad.uf2 to USB drive
# 3. Device automatically reboots

# Method 2: picotool
picotool load maplepad.uf2
picotool reboot
```

## ⚙️ Configuration

### Display Selection
The system automatically detects the connected display type via GP22:
- **Low (0V)** - SSD1306/SSD1309 monochrome displays
- **High (3.3V)** - SSD1331 color display

### Controller Mode
Set in `maple.h`:
```c
#define HKT7700 0  // Standard controller (9 buttons)
#define HKT7300 1  // Arcade stick (11 buttons)
```

### SD Card Features
- **Automatic Detection** - System detects SD card presence
- **VMU Save/Load** - Save VMU pages to SD card
- **Menu Interface** - Access via button combinations

## 🎮 Usage

### Basic Operation
1. Connect to Dreamcast via Maple bus
2. Power on - system initializes automatically
3. Display shows status information
4. Controller functions as standard Dreamcast pad

### VMU Operations
- **Save to SD**: Access SD menu, select save option
- **Load from SD**: Access SD menu, select load option
- **Page Management**: Use page button to cycle VMU pages

### Menu System
- Access configuration menu via button combinations
- Adjust display settings, calibration, and preferences
- View system information and diagnostics

## 🔍 Troubleshooting

### Build Issues
- **Missing Functions**: Ensure all source files are included
- **Pin Conflicts**: Verify pin assignments match hardware
- **SDK Version**: Use compatible Pico SDK version

### Hardware Issues
- **No Display**: Check power and pin connections
- **SD Card Errors**: Verify SPI connections and card format
- **Controller Not Detected**: Check Maple bus wiring

### Software Issues
- **Boot Failures**: Check firmware programming
- **Display Corruption**: Verify display type detection
- **Save/Load Errors**: Check SD card functionality

## 📋 File Structure

```
MaplePad-alter/
├── src/
│   ├── maple.c              # Main controller logic
│   ├── maple.h              # Core definitions
│   ├── display.c            # Display abstraction layer
│   ├── display.h            # Display interface
│   ├── sdcard.c             # SD card implementation
│   ├── sdcard.h             # SD card interface
│   ├── ssd1306.c/h          # SSD1306 driver
│   ├── ssd1309.c/h          # SSD1309 driver
│   ├── ssd1331.c/h          # SSD1331 driver
│   ├── font.c/h             # Font rendering system
│   └── menu.c/h             # Menu system
├── build/                   # Build output
├── CMakeLists.txt          # Build configuration
└── README.md               # This file
```

## 🤝 Contributing

### Development Guidelines
- Follow existing code style and conventions
- Test thoroughly on hardware before submitting
- Document any new features or pin assignments
- Ensure backward compatibility when possible

### Reporting Issues
- Include hardware configuration details
- Provide build environment information
- Describe steps to reproduce problems
- Include relevant log output or error messages

## 📄 License

This project is licensed under Creative Commons Attribution 4.0 International License.

**Share** — copy and redistribute the material in any medium or format  
**Adapt** — remix, transform, and build upon the material for any purpose, even commercially  
**Attribution** — You must give appropriate credit, provide a link to the license, and indicate if changes were made

## 🙏 Acknowledgments

- **Charlie Cole** - Original Pop'n Music Controller foundation
- **Mackie Kannard-Smith (mackieks)** - MaplePad development and enhancements
- **Raspberry Pi Foundation** - RP2040/RP2350 platform and SDK
- **Community Contributors** - Testing, feedback, and improvements

## 🔗 Links

- [Original MaplePad Repository](https://github.com/mackieks/MaplePad)
- [Charlie Cole's Pop'n Music Controller](https://github.com/charcole/Dreamcast-PopnMusic)
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [Dreamcast Maple Bus Documentation](https://segaretro.org/Maple)

---

**Built with ❤️ for the Dreamcast community**