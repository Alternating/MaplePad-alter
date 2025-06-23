// # FILE: src/sdcard.c (NEW FILE)
#include "sdcard.h"

static sd_card_type_t card_type = CARD_TYPE_UNKNOWN;
static bool sd_initialized = false;

bool sd_init(void) {
    // Initialize SPI
    spi_init(SD_SPI_PORT, SD_SPEED_HZ);
    
    // Set up GPIO pins
    gpio_set_function(SD_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_MISO_PIN, GPIO_FUNC_SPI);
    
    // CS pin as GPIO output
    gpio_init(SD_CS_PIN);
    gpio_set_dir(SD_CS_PIN, GPIO_OUT);
    gpio_put(SD_CS_PIN, 1); // Deselect card initially
    
    sleep_ms(100); // Wait for card to power up
    
    // Send 80 clock cycles with CS high to put card in SPI mode
    sd_cs_deselect();
    for (int i = 0; i < 10; i++) {
        sd_spi_transfer(0xFF);
    }
    
    // Reset card to idle state
    sd_cs_select();
    uint8_t response = sd_send_command(CMD0, 0);
    sd_cs_deselect();
    
    if (response != R1_IDLE_STATE) {
        printf("SD: CMD0 failed, response: 0x%02X\n", response);
        return false;
    }
    
    // Check interface condition (CMD8)
    sd_cs_select();
    response = sd_send_command(CMD8, 0x1AA);
    if (response == R1_IDLE_STATE) {
        // SD Card v2.0 or later
        uint8_t ocr[4];
        for (int i = 0; i < 4; i++) {
            ocr[i] = sd_spi_transfer(0xFF);
        }
        
        if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
            // Valid response, continue with ACMD41
            card_type = CARD_TYPE_SD2;
        } else {
            sd_cs_deselect();
            return false;
        }
    } else {
        // SD Card v1.x or MMC
        card_type = CARD_TYPE_SD1;
    }
    sd_cs_deselect();
    
    // Initialize card with ACMD41 (for SD cards) or CMD1 (for MMC)
    uint32_t timeout = 1000; // 1 second timeout
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    
    do {
        if (card_type == CARD_TYPE_SD1 || card_type == CARD_TYPE_SD2) {
            // Send ACMD41 for SD cards
            sd_cs_select();
            sd_send_command(CMD55, 0);
            response = sd_send_command(CMD41, card_type == CARD_TYPE_SD2 ? 0x40000000 : 0);
            sd_cs_deselect();
        } else {
            // Send CMD1 for MMC
            sd_cs_select();
            response = sd_send_command(CMD1, 0);
            sd_cs_deselect();
        }
        
        if (to_ms_since_boot(get_absolute_time()) - start_time > timeout) {
            printf("SD: Initialization timeout\n");
            return false;
        }
        
        sleep_ms(10);
    } while (response != 0x00);
    
    // Check if it's SDHC/SDXC
    if (card_type == CARD_TYPE_SD2) {
        sd_cs_select();
        response = sd_send_command(CMD58, 0);
        if (response == 0x00) {
            uint8_t ocr[4];
            for (int i = 0; i < 4; i++) {
                ocr[i] = sd_spi_transfer(0xFF);
            }
            if (ocr[0] & 0x40) {
                card_type = CARD_TYPE_SDHC;
            }
        }
        sd_cs_deselect();
    }
    
    // Set block length to 512 bytes for standard capacity cards
    if (card_type != CARD_TYPE_SDHC) {
        sd_cs_select();
        response = sd_send_command(CMD16, 512);
        sd_cs_deselect();
        if (response != 0x00) {
            printf("SD: CMD16 failed\n");
            return false;
        }
    }
    
    // Increase SPI speed for data transfer
    spi_set_baudrate(SD_SPI_PORT, 10000000); // 10 MHz
    
    sd_initialized = true;
    printf("SD: Card initialized successfully, type: %d\n", card_type);
    return true;
}

bool sd_read_block(uint32_t block_addr, uint8_t *buffer) {
    if (!sd_initialized) return false;
    
    // Convert block address for standard capacity cards
    if (card_type != CARD_TYPE_SDHC) {
        block_addr *= 512;
    }
    
    sd_cs_select();
    uint8_t response = sd_send_command(CMD17, block_addr);
    
    if (response != 0x00) {
        sd_cs_deselect();
        return false;
    }
    
    // Wait for data token
    uint32_t timeout = 1000;
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    uint8_t token;
    
    do {
        token = sd_spi_transfer(0xFF);
        if (to_ms_since_boot(get_absolute_time()) - start_time > timeout) {
            sd_cs_deselect();
            return false;
        }
    } while (token == 0xFF);
    
    if (token != 0xFE) {
        sd_cs_deselect();
        return false;
    }
    
    // Read data
    for (int i = 0; i < 512; i++) {
        buffer[i] = sd_spi_transfer(0xFF);
    }
    
    // Read CRC (but ignore it)
    sd_spi_transfer(0xFF);
    sd_spi_transfer(0xFF);
    
    sd_cs_deselect();
    return true;
}

bool sd_write_block(uint32_t block_addr, const uint8_t *buffer) {
    if (!sd_initialized) return false;
    
    // Convert block address for standard capacity cards
    if (card_type != CARD_TYPE_SDHC) {
        block_addr *= 512;
    }
    
    sd_cs_select();
    uint8_t response = sd_send_command(CMD24, block_addr);
    
    if (response != 0x00) {
        sd_cs_deselect();
        return false;
    }
    
    // Send data token
    sd_spi_transfer(0xFE);
    
    // Send data
    for (int i = 0; i < 512; i++) {
        sd_spi_transfer(buffer[i]);
    }
    
    // Send dummy CRC
    sd_spi_transfer(0xFF);
    sd_spi_transfer(0xFF);
    
    // Check data response
    uint8_t data_response = sd_spi_transfer(0xFF) & 0x1F;
    if (data_response != 0x05) {
        sd_cs_deselect();
        return false;
    }
    
    // Wait for write to complete
    if (!sd_wait_ready(1000)) {
        sd_cs_deselect();
        return false;
    }
    
    sd_cs_deselect();
    return true;
}

void sd_deinit(void) {
    sd_initialized = false;
    spi_deinit(SD_SPI_PORT);
}

// Internal functions
static uint8_t sd_send_command(uint8_t cmd, uint32_t arg) {
    // Calculate CRC for CMD0 and CMD8
    uint8_t crc = 0xFF;
    if (cmd == CMD0) crc = 0x95;
    if (cmd == CMD8) crc = 0x87;
    
    // Send command packet
    sd_spi_transfer(0x40 | cmd);
    sd_spi_transfer((arg >> 24) & 0xFF);
    sd_spi_transfer((arg >> 16) & 0xFF);
    sd_spi_transfer((arg >> 8) & 0xFF);
    sd_spi_transfer(arg & 0xFF);
    sd_spi_transfer(crc);
    
    // Wait for response
    uint8_t response;
    int attempts = 0;
    do {
        response = sd_spi_transfer(0xFF);
        attempts++;
    } while ((response & 0x80) && attempts < 8);
    
    return response;
}

static bool sd_wait_ready(uint32_t timeout_ms) {
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    
    do {
        if (sd_spi_transfer(0xFF) == 0xFF) {
            return true;
        }
        if (to_ms_since_boot(get_absolute_time()) - start_time > timeout_ms) {
            return false;
        }
    } while (true);
}

static void sd_cs_select(void) {
    gpio_put(SD_CS_PIN, 0);
    sleep_us(1);
}

static void sd_cs_deselect(void) {
    gpio_put(SD_CS_PIN, 1);
    sleep_us(1);
}

static uint8_t sd_spi_transfer(uint8_t data) {
    uint8_t rx_data;
    spi_write_read_blocking(SD_SPI_PORT, &data, &rx_data, 1);
    return rx_data;
}

static void sd_spi_transfer_bulk(const uint8_t *tx_data, uint8_t *rx_data, size_t len) {
    spi_write_read_blocking(SD_SPI_PORT, tx_data, rx_data, len);
}