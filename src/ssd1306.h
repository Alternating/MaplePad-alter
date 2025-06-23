// UPDATED src/ssd1306.h - Conflict-Free Pin Assignment
// Update these pin definitions in your ssd1306.h

#pragma once

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// SSD1306 defines
#define SSD1306_ADDRESS 0x3C
#define SSD1306_I2C i2c0  // Using I2C0 instead of I2C1

// UPDATED CONFLICT-FREE I2C PINS
#define I2C_SDA 12   // Was 2 (GP12 when not used for Maple Bus)
#define I2C_SCL 13   // Was 3 (GP13 when not used for other purposes)

// Alternative I2C pins if GP12/13 still conflict:
// #define I2C_SDA 8    // Alternative option
// #define I2C_SCL 9    // Alternative option

// value in KHz
#define I2C_CLOCK 3000

#define SSD1306_LCDWIDTH 128
#define SSD1306_LCDHEIGHT 64
#define SSD1306_FRAMEBUFFER_SIZE (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8)

// SSD1306 Commands (unchanged)
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_ACTIVATE_SCROLL 0x2F

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_SEGREMAP0 0xA0
#define SSD1306_SEGREMAP127 0xA1
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB

// Function prototypes (unchanged)
void ssd1306SendCommand(uint8_t cmd);
void ssd1306SendCommandBuffer(uint8_t *inbuf, int len);
void ssd1306_init();
void updateSSD1306();
void clearSSD1306();
void splashSSD1306();
void setPixelSSD1306(int x, int y, bool on);