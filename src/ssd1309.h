// # FILE: src/ssd1309.h (NEW FILE)
#pragma once

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// SSD1309 defines
#define SSD1309_ADDRESS 0x3C
#define SSD1309_I2C i2c1
#define I2C_SDA 2
#define I2C_SCL 3

// value in KHz
#define I2C_CLOCK 3000

#define SSD1309_LCDWIDTH 128
#define SSD1309_LCDHEIGHT 64
#define SSD1309_FRAMEBUFFER_SIZE (SSD1309_LCDWIDTH * SSD1309_LCDHEIGHT / 8)

// SSD1309 Commands (similar to SSD1306 but with some differences)
#define SSD1309_SETLOWCOLUMN 0x00
#define SSD1309_SETHIGHCOLUMN 0x10

#define SSD1309_MEMORYMODE 0x20
#define SSD1309_COLUMNADDR 0x21
#define SSD1309_PAGEADDR 0x22
#define SSD1309_DEACTIVATE_SCROLL 0x2E
#define SSD1309_ACTIVATE_SCROLL 0x2F

#define SSD1309_SETSTARTLINE 0x40

#define SSD1309_SETCONTRAST 0x81
#define SSD1309_CHARGEPUMP 0x8D

#define SSD1309_SEGREMAP0 0xA0
#define SSD1309_SEGREMAP127 0xA1
#define SSD1309_DISPLAYALLON_RESUME 0xA4
#define SSD1309_DISPLAYALLON 0xA5
#define SSD1309_NORMALDISPLAY 0xA6
#define SSD1309_INVERTDISPLAY 0xA7
#define SSD1309_SETMULTIPLEX 0xA8
#define SSD1309_DISPLAYOFF 0xAE
#define SSD1309_DISPLAYON 0xAF

#define SSD1309_COMSCANINC 0xC0
#define SSD1309_COMSCANDEC 0xC8

#define SSD1309_SETDISPLAYOFFSET 0xD3
#define SSD1309_SETDISPLAYCLOCKDIV 0xD5
#define SSD1309_SETPRECHARGE 0xD9
#define SSD1309_SETCOMPINS 0xDA
#define SSD1309_SETVCOMDETECT 0xDB

// SSD1309 specific commands
#define SSD1309_SETDCDC 0xAD
#define SSD1309_FADEOUT 0x23
#define SSD1309_BREATHE 0x23

void ssd1309SendCommand(uint8_t cmd);

void ssd1309SendCommandBuffer(uint8_t *inbuf, int len);

void ssd1309_init();

void updateSSD1309();

void clearSSD1309();

void splashSSD1309();

void setPixelSSD1309(int x, int y, bool on);