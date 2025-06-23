// # FILE: src/display.h (UPDATE EXISTING or CREATE NEW)
#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"

// Display type constants
#define DISPLAY_SSD1306 0
#define DISPLAY_SSD1331 1  
#define DISPLAY_SSD1309 2

// OLED detection pin
#define OLED_PIN 22

// Function prototypes
void displayInit(void);
void updateDisplay(void);
void clearDisplay(void);
void splashDisplay(void);
void setDisplayPixel(int x, int y, bool on);
const char* getDisplayTypeString(void);
bool displaySupportsColor(void);
void getDisplayDimensions(int *width, int *height);
uint8_t detect_oled_type(void);

// Your existing display functions (from your current codebase)
void putLetter(int x, int y, uint8_t letter, uint16_t color);
void putString(char* str, int x, int y, uint16_t color);