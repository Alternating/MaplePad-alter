// # FILE: src/pins.h (NEW FILE) or ADD TO src/maple.h
// Updated pin definitions for RP2350 with Adafruit SD Card breakout
// Add these to your maple.h or create a new pins.h file

#pragma once

// Board detection
#ifdef PICO_RP2350
#define BOARD_RP2350 1
#define BOARD_RP2040 0
#else
#define BOARD_RP2350 0
#define BOARD_RP2040 1
#endif

// Original MaplePad pins (unchanged)
#define MAPLE_A 11
#define MAPLE_B 12
#define INPUT_ACT 20
#define PAGE_BUTTON 21
#define OLED_PIN 22

// SSD1306/SSD1309 I2C pins (shared)
#define I2C_SDA 2
#define I2C_SCL 3

// SSD1331 SPI pins
#define SSD1331_SCK 2
#define SSD1331_MOSI 3
#define SSD1331_DC 14
#define SSD1331_RST 15

// Adafruit SD Card SPI pins
// Using SPI1 to avoid conflicts with OLED SPI
#define SD_SPI_PORT spi1
#define SD_SCK_PIN 10
#define SD_MOSI_PIN 11
#define SD_MISO_PIN 12
#define SD_CS_PIN 13

// Note: If pin 13 conflicts with SSD1331_RST, you can change SD_CS_PIN
// Alternative SD CS pin options: 15, 16, 17, 18, 19
// Just make sure to update the hardware connections accordingly

// Alternative pin configuration if there are conflicts:
/*
// Alternative SD Card pins (if pin 13 conflicts)
#define SD_CS_PIN 15    // Use GP15 instead of GP13

// Or use different SPI pins entirely:
#define SD_SCK_PIN 14
#define SD_MOSI_PIN 15
#define SD_MISO_PIN 16
#define SD_CS_PIN 17
*/

// RP2350 specific pins and features
#ifdef PICO_RP2350
// Additional GPIO pins available on RP2350
#define RP2350_EXTRA_GPIO_START 26
#define RP2350_EXTRA_GPIO_END 29

// PSRAM pins (if using RP2350 with PSRAM)
#ifdef PICO_PSRAM
#define PSRAM_CS_PIN 47
#define PSRAM_SCK_PIN 46
#define PSRAM_D0_PIN 40
#define PSRAM_D1_PIN 41
#define PSRAM_D2_PIN 42
#define PSRAM_D3_PIN 43
#endif

#endif

// Pin validation macros
#define IS_VALID_GPIO(pin) ((pin) >= 0 && (pin) <= 29)
#define IS_SPI_PIN(pin) ((pin) == 0 || (pin) == 1 || (pin) == 2 || (pin) == 3 || \
                         (pin) == 4 || (pin) == 5 || (pin) == 6 || (pin) == 7 || \
                         (pin) == 10 || (pin) == 11 || (pin) == 12 || (pin) == 13 || \
                         (pin) == 14 || (pin) == 15 || (pin) == 16 || (pin) == 17 || \
                         (pin) == 18 || (pin) == 19)

#define IS_I2C_PIN(pin) ((pin) == 0 || (pin) == 1 || (pin) == 2 || (pin) == 3 || \
                         (pin) == 4 || (pin) == 5 || (pin) == 6 || (pin) == 7 || \
                         (pin) == 8 || (pin) == 9 || (pin) == 10 || (pin) == 11 || \
                         (pin) == 12 || (pin) == 13 || (pin) == 14 || (pin) == 15 || \
                         (pin) == 16 || (pin) == 17 || (pin) == 18 || (pin) == 19 || \
                         (pin) == 20 || (pin) == 21 || (pin) == 26 || (pin) == 27)

// Function prototypes for pin management
void validate_pin_assignments(void);
void print_pin_configuration(void);
bool check_pin_conflicts(void);