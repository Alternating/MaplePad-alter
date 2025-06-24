// FILE: src/maple.h - UPDATED PIN DEFINITIONS
// Replace your maple.h pin definitions with these

#pragma once

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "format.h"
#include "maple.pio.h"
#include "pico/stdlib.h"

#include "ssd1306.h"
#include "ssd1331.h"

#include "hardware/adc.h"
#include "hardware/flash.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "pico/multicore.h"
#include "pico/time.h"
#include "state_machine.h"

// OPTIMIZED PIN ASSIGNMENTS - NO CONTROLLER INPUTS
#define MAPLE_A 0   // Dedicated Maple bus pin A
#define MAPLE_B 1   // Dedicated Maple bus pin B

// Configuration pins
#define OLED_PIN 12      // Display type detection
#define PAGE_BUTTON 13   // VMU page control

// Legacy compatibility (no longer used for GPIO inputs)
#define INPUT_ACT 14     // Available for other use
#define PICO_PIN1_PIN_RX MAPLE_A
#define PICO_PIN5_PIN_RX MAPLE_B

// Controller mode selection
#define HKT7700 0 // "Seed" (standard controller)
#define HKT7300 1 // Arcade stick

// Constants
#define CURRENT_FW_VERSION VER_1_5
#define BLOCK_SIZE 512

// Version definitions
#define VER_1_0 0x00
#define VER_1_1 0x01
#define VER_1_2 0x02
#define VER_1_3 0x03
#define VER_1_3b 0x04
#define VER_1_4 0x05
#define VER_1_4b 0x06
#define VER_1_4c 0x07
#define VER_1_4d 0x08
#define VER_1_4e 0x09
#define VER_1_5 0x0A
#define VER_1_6 0x0B
#define VER_1_7 0x0C

// External variable declarations
extern uint8_t MemoryCard[];
extern uint8_t flashData[];
extern uint16_t color;
extern bool sd_card_available;

// Flash data accessors
#define xCenter flashData[0]
#define xMin flashData[1]
#define xMax flashData[2]
#define yCenter flashData[3]
#define yMin flashData[4]
#define yMax flashData[5]
#define lMin flashData[6]
#define lMax flashData[7]
#define rMin flashData[8]
#define rMax flashData[9]
#define invertX flashData[10]
#define invertY flashData[11]
#define invertL flashData[12]
#define invertR flashData[13]
#define firstBoot flashData[14]
#define currentPage flashData[15]
#define rumbleEnable flashData[16]
#define vmuEnable flashData[17]
#define oledFlip flashData[18]
#define swapXY flashData[19]
#define swapLR flashData[20]
#define oledType flashData[21]
#define triggerMode flashData[22]
#define xDeadzone flashData[23]
#define xAntiDeadzone flashData[24]
#define yDeadzone flashData[25]
#define yAntiDeadzone flashData[26]
#define lDeadzone flashData[27]
#define lAntiDeadzone flashData[28]
#define rDeadzone flashData[29]
#define rAntiDeadzone flashData[30]
#define autoResetEnable flashData[31]
#define autoResetTimer flashData[32]
#define version flashData[33]

// Function declarations
void updateFlashData();
void readFlash(void);
void initialize_peripherals(void);

// Display function declarations
void clearDisplay(void);
void putString(char* str, int x, int y, uint16_t color);
void updateDisplay(void);
void displayInit(void);

// SD card function declarations
bool sd_init(void);
bool sd_write_block(uint32_t block_addr, const uint8_t* data);
bool sd_read_block(uint32_t block_addr, uint8_t* data);

// Packet structures for Maple communication
typedef struct PacketHeader_s {
  int8_t Command;
  uint8_t Destination;
  uint8_t Origin;
  uint8_t NumWords;
} PacketHeader;

typedef struct PacketDeviceInfo_s {
  uint Func;
  uint FuncData[3];
  int8_t AreaCode;
  uint8_t ConnectorDirection;
  char ProductName[30];
  char ProductLicense[60];
  uint16_t StandbyPower;
  uint16_t MaxPower;
} PacketDeviceInfo;

// Menu structure
typedef struct menu_s menu;
struct menu_s {
    char name[14];
    int type;
    bool visible;
    bool selected;
    bool on;
    bool enabled;
    int (*run)(struct menu_s *self);
};