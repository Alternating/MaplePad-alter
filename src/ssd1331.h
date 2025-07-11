// UPDATED src/ssd1331.h - Conflict-Free Pin Assignment
// Update these pin definitions in your ssd1331.h

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/dma.h"

#define SSD1331_SPI spi1
#define SSD1331_SPEED 50000000

#define SSD1331_SPI spi1  // Using SPI1 for display
#define SSD1331_SPEED 50000000 // 50MHz

// UPDATED CONFLICT-FREE DISPLAY PINS
#define SCK 6    // Was 2 (now conflicts avoided with I2C)
#define MOSI 7   // Was 3 (now conflicts avoided with I2C)
#define DC 8     // Was 14 (moved to avoid SD card conflicts)
#define RST 9    // Was 15 (moved to avoid SD card conflicts)

#define OLED_W 96
#define OLED_H 64

#define OLED_FLIP flashData[18]

// SSD1331 Commands (unchanged)
#define SSD1331_CMD_DRAWLINE 0x21       //!< Draw line
#define SSD1331_CMD_DRAWRECT 0x22       //!< Draw rectangle
#define SSD1331_CMD_FILL 0x26           //!< Fill enable/disable
#define SSD1331_CMD_SETCOLUMN 0x15      //!< Set column address
#define SSD1331_CMD_SETROW 0x75         //!< Set row adress
#define SSD1331_CMD_CONTRASTA 0x81      //!< Set contrast for color A
#define SSD1331_CMD_CONTRASTB 0x82      //!< Set contrast for color B
#define SSD1331_CMD_CONTRASTC 0x83      //!< Set contrast for color C
#define SSD1331_CMD_MASTERCURRENT 0x87  //!< Master current control
#define SSD1331_CMD_SETREMAP 0xA0       //!< Set re-map & data format
#define SSD1331_CMD_STARTLINE 0xA1      //!< Set display start line
#define SSD1331_CMD_DISPLAYOFFSET 0xA2  //!< Set display offset
#define SSD1331_CMD_NORMALDISPLAY 0xA4  //!< Set display to normal mode
#define SSD1331_CMD_DISPLAYALLON 0xA5   //!< Set entire display ON
#define SSD1331_CMD_DISPLAYALLOFF 0xA6  //!< Set entire display OFF
#define SSD1331_CMD_INVERTDISPLAY 0xA7  //!< Invert display
#define SSD1331_CMD_SETMULTIPLEX 0xA8   //!< Set multiplex ratio
#define SSD1331_CMD_SETMASTER 0xAD      //!< Set master configuration
#define SSD1331_CMD_DISPLAYOFF 0xAE     //!< Display OFF (sleep mode)
#define SSD1331_CMD_DISPLAYON 0xAF      //!< Normal Brightness Display ON
#define SSD1331_CMD_POWERMODE 0xB0      //!< Power save mode
#define SSD1331_CMD_PRECHARGE 0xB1      //!< Phase 1 and 2 period adjustment
#define SSD1331_CMD_CLOCKDIV 0xB3       //!< Set display clock divide ratio/oscillator frequency
#define SSD1331_CMD_PRECHARGEA 0x8A     //!< Set second pre-charge speed for color A
#define SSD1331_CMD_PRECHARGEB 0x8B     //!< Set second pre-charge speed for color B
#define SSD1331_CMD_PRECHARGEC 0x8C     //!< Set second pre-charge speed for color C
#define SSD1331_CMD_PRECHARGELEVEL 0xBB //!< Set pre-charge voltage
#define SSD1331_CMD_VCOMH 0xBE          //!< Set Vcomh voltge

// Function prototypes (unchanged)
float cos_32s(float x);
float cos32(float x);
float sin32(float x);
double atan66(double x);
double atan_66s(double x);

void fast_hsv2rgb_32bit(uint16_t, uint8_t, uint8_t, uint8_t *, uint8_t *, uint8_t *);

void ssd1331WriteCommand(const uint8_t data);
void ssd1331WriteCommands(const uint8_t *data, uint num);
void ssd1331WriteData(const uint8_t *data, uint numbytes);
void setPixelSSD1331(const uint8_t x, const uint8_t y, const uint16_t color);
void clearSSD1331(void);
void updateSSD1331(void);
void splashSSD1331(void);
void ssd1331_init();