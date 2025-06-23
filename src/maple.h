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

#define HKT7700 0 // "Seed" (standard controller)
#define HKT7300 1 // Arcade stick

#if HKT7700
#define NUM_BUTTONS 9
#elif HKT7300
#define NUM_BUTTONS 11
#endif

#define CURRENT_FW_VERSION VER_1_5

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

// Constants that were missing
#define BLOCK_SIZE 512

// External variable declarations (variables are defined in maple.c)
extern uint8_t MemoryCard[];
extern uint8_t flashData[];
extern uint16_t color;
extern bool sd_card_available;

// Flash data accessors (these reference flashData defined in maple.c)
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
#define triggerMode flashData[22] // 1 = analog, 0 = digital
#define xDeadzone flashData[23]
#define xAntiDeadzone flashData[24]
#define yDeadzone flashData[25]
#define yAntiDeadzone flashData[26]
#define lDeadzone flashData[27]
#define lAntiDeadzone flashData[28]
#define rDeadzone flashData[29]
#define rAntiDeadzone flashData[30]
#define autoResetEnable flashData[31]
#define autoResetTimer flashData[32] // units are 2s, max value 8.5 minutes
#define version flashData[33]

void updateFlashData();

typedef struct PacketHeader_s {
  int8_t Command;
  uint8_t Destination;
  uint8_t Origin;
  uint8_t NumWords;
} PacketHeader;

typedef struct PacketDeviceInfo_s {
  uint Func;        // Nb. Big endian
  uint FuncData[3]; // Nb. Big endian
  int8_t AreaCode;
  uint8_t ConnectorDirection;
  char ProductName[30];
  char ProductLicense[60];
  uint16_t StandbyPower;
  uint16_t MaxPower;
} PacketDeviceInfo;

typedef struct PacketAllDeviceInfo_s {
  uint Func;        // Nb. Big endian
  uint FuncData[3]; // Nb. Big endian
  int8_t AreaCode;
  uint8_t ConnectorDirection;
  char ProductName[30];
  char ProductLicense[60];
  uint16_t StandbyPower;
  uint16_t MaxPower;
  char FreeDeviceStatus[80];
} PacketAllDeviceInfo;

typedef struct PacketMemoryInfo_s {
  uint Func; // Nb. Big endian
  uint16_t TotalSize;
  uint16_t ParitionNumber;
  uint16_t SystemArea;
  uint16_t FATArea;
  uint16_t NumFATBlocks;
  uint16_t FileInfoArea;
  uint16_t NumInfoBlocks;
  uint8_t VolumeIcon;
  uint8_t Reserved;
  uint16_t SaveArea;
  uint16_t NumSaveBlocks;
  uint Reserved32;
  uint16_t Reserved16;
} PacketMemoryInfo;

typedef struct PacketLCDInfo_s {
  uint Func;            // Nb. Big endian
  uint8_t dX;           // Number of X-axis dots
  uint8_t dY;           // Number of Y-axis dots
  uint8_t GradContrast; // Upper nybble Gradation (bits/dot), lower nybble contrast (0 to 16 steps)
  uint8_t Reserved;
} PacketLCDInfo;

typedef struct PacketPuruPuruInfo_s {
  uint Func;     // Nb. Big endian
  uint8_t BT;          // Button data
  uint8_t Reserved[3]; // Reserved (0)
} PacketTimerCondition;

typedef struct PacketBlockRead_s {
  uint Func; // Nb. Big endian
  uint Address;
  uint8_t Data[BLOCK_SIZE];
} PacketBlockRead;

typedef struct PuruPuruBlockReadPacket_s {
  uint Func; // Nb. Big endian
  uint Address;
  uint8_t Data[4];
} PuruPuruBlockReadPacket;

typedef struct TimerBlockReadPacket_s {
  uint Func; // Nb. Big endian
  uint8_t Date[8];
} TimerBlockReadPacket;

// ButtonInfo structure (define it only once, here in maple.h)
typedef struct ButtonInfo_s {
    int InputIO;
    int DCButtonMask;
} ButtonInfo;

extern ButtonInfo ButtonInfos[];

// Menu structure (define it only once, here in maple.h)  
typedef struct menu_s menu;

struct menu_s {
    char name[14];
    int type; // 0: submenu, 1: boolean toggle, 2: function, 3: inert
    bool visible;
    bool selected;
    bool on;
    bool enabled; // control for hidden menu items (ssd1306)
    int (*run)(struct menu_s *self);
};

// Function declarations for functions implemented in maple.c
void readFlash(void);
void updateFlashData(void);
void initialize_peripherals(void);

// Display function declarations (should be in display.h but declaring here for safety)
void clearDisplay(void);
void putString(char* str, int x, int y, uint16_t color);
void updateDisplay(void);
void displayInit(void);

// SD card function declarations (should be in sdcard.h but declaring here for safety)
bool sd_init(void);
bool sd_write_block(uint32_t block_addr, const uint8_t* data);
bool sd_read_block(uint32_t block_addr, uint8_t* data);