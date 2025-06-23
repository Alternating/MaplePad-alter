// UPDATED src/sdcard.h - Conflict-Free Pin Assignment
// Replace your existing sdcard.h with this version

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// SD Card SPI configuration - CONFLICT-FREE PINS
#define SD_SPI_PORT spi0  // Using SPI0 instead of SPI1
#define SD_SPEED_HZ 1000000  // 1MHz for initialization, can go up to 25MHz later

// NEW CONFLICT-FREE SD CARD PINS
#define SD_SCK_PIN 18   // Was 10 (conflicted with Start button)
#define SD_MOSI_PIN 19  // Was 11 (conflicted with MAPLE_A)  
#define SD_MISO_PIN 16  // Was 12 (conflicted with MAPLE_B)
#define SD_CS_PIN 17    // Was 13 (conflicted with potential display pins)

// SD Card commands (unchanged)
#define CMD0    0   // GO_IDLE_STATE
#define CMD1    1   // SEND_OP_COND (MMC)
#define CMD8    8   // SEND_IF_COND
#define CMD9    9   // SEND_CSD
#define CMD10   10  // SEND_CID
#define CMD12   12  // STOP_TRANSMISSION
#define CMD16   16  // SET_BLOCKLEN
#define CMD17   17  // READ_SINGLE_BLOCK
#define CMD18   18  // READ_MULTIPLE_BLOCK
#define CMD23   23  // SET_BLOCK_COUNT (MMC)
#define CMD24   24  // WRITE_BLOCK
#define CMD25   25  // WRITE_MULTIPLE_BLOCK
#define CMD27   27  // PROGRAM_CSD
#define CMD28   28  // SET_WRITE_PROT
#define CMD29   29  // CLR_WRITE_PROT
#define CMD30   30  // SEND_WRITE_PROT
#define CMD32   32  // ERASE_WR_BLK_START
#define CMD33   33  // ERASE_WR_BLK_END
#define CMD38   38  // ERASE
#define CMD41   41  // SEND_OP_COND (SDC)
#define CMD55   55  // APP_CMD
#define CMD58   58  // READ_OCR

// SD Card response types (unchanged)
#define R1_IDLE_STATE           0x01
#define R1_ERASE_RESET          0x02
#define R1_ILLEGAL_COMMAND      0x04
#define R1_COM_CRC_ERROR        0x08
#define R1_ERASE_SEQUENCE_ERROR 0x10
#define R1_ADDRESS_ERROR        0x20
#define R1_PARAMETER_ERROR      0x40

// SD Card types (unchanged)
typedef enum {
    CARD_TYPE_UNKNOWN = 0,
    CARD_TYPE_MMC,
    CARD_TYPE_SD1,
    CARD_TYPE_SD2,
    CARD_TYPE_SDHC
} sd_card_type_t;

// Function prototypes (unchanged)
bool sd_init(void);
bool sd_read_block(uint32_t block_addr, uint8_t *buffer);
bool sd_write_block(uint32_t block_addr, const uint8_t *buffer);
bool sd_read_multiple_blocks(uint32_t start_block, uint32_t num_blocks, uint8_t *buffer);
bool sd_write_multiple_blocks(uint32_t start_block, uint32_t num_blocks, const uint8_t *buffer);
void sd_deinit(void);

// Internal functions
static uint8_t sd_send_command(uint8_t cmd, uint32_t arg);
static bool sd_wait_ready(uint32_t timeout_ms);
static void sd_cs_select(void);
static void sd_cs_deselect(void);
static uint8_t sd_spi_transfer(uint8_t data);
static void sd_spi_transfer_bulk(const uint8_t *tx_data, uint8_t *rx_data, size_t len);