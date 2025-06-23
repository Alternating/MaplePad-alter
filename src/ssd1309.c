// # FILE: src/ssd1309.c (NEW FILE)
#include "ssd1309.h"
#include "font.h"

extern uint8_t frameBuffer[SSD1309_FRAMEBUFFER_SIZE];

void ssd1309SendCommand(uint8_t cmd) {
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(SSD1309_I2C, SSD1309_ADDRESS, buf, 2, false);
}

void ssd1309SendCommandBuffer(uint8_t *inbuf, int len) {
    for (int i = 0; i < len; i++) {
        ssd1309SendCommand(inbuf[i]);
    }
}

void ssd1309_init() {
    // Initialize I2C
    i2c_init(SSD1309_I2C, I2C_CLOCK * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    sleep_ms(100);

    // SSD1309 initialization sequence
    uint8_t init_sequence[] = {
        SSD1309_DISPLAYOFF,           // 0xAE
        SSD1309_SETDISPLAYCLOCKDIV,   // 0xD5
        0x80,                         // Clock divide ratio/frequency
        SSD1309_SETMULTIPLEX,         // 0xA8
        0x3F,                         // 1/64 duty
        SSD1309_SETDISPLAYOFFSET,     // 0xD3
        0x00,                         // No offset
        SSD1309_SETSTARTLINE | 0x00,  // Line #0
        SSD1309_SETDCDC,              // 0xAD
        0x8B,                         // Built-in DC-DC enabled (different from SSD1306)
        SSD1309_MEMORYMODE,           // 0x20
        0x00,                         // Horizontal addressing mode
        SSD1309_SEGREMAP127,          // 0xA1
        SSD1309_COMSCANDEC,           // 0xC8
        SSD1309_SETCOMPINS,           // 0xDA
        0x12,                         // Alt COM pin config, disable COM left/right remap
        SSD1309_SETCONTRAST,          // 0x81
        0x9F,                         // Contrast
        SSD1309_SETPRECHARGE,         // 0xD9
        0x25,                         // Pre-charge period (different from SSD1306)
        SSD1309_SETVCOMDETECT,        // 0xDB
        0x34,                         // VCOMH deselect level
        SSD1309_DISPLAYALLON_RESUME,  // 0xA4
        SSD1309_NORMALDISPLAY,        // 0xA6
        SSD1309_DISPLAYON             // 0xAF
    };

    ssd1309SendCommandBuffer(init_sequence, sizeof(init_sequence));
    
    clearSSD1309();
    updateSSD1309();
}

void updateSSD1309() {
    uint8_t payload[] = {SSD1309_PAGEADDR, 0, 0xFF, SSD1309_COLUMNADDR, 0, SSD1309_LCDWIDTH - 1};
    ssd1309SendCommandBuffer(payload, sizeof(payload));

    // Send framebuffer data
    uint8_t *buf = frameBuffer;
    for (int i = 0; i < SSD1309_FRAMEBUFFER_SIZE; i += 16) {
        uint8_t data_buf[17];
        data_buf[0] = 0x40; // Data continuation
        memcpy(&data_buf[1], &buf[i], 16);
        i2c_write_blocking(SSD1309_I2C, SSD1309_ADDRESS, data_buf, 17, false);
    }
}

void clearSSD1309() {
    memset(frameBuffer, 0, SSD1309_FRAMEBUFFER_SIZE);
}

void splashSSD1309() {
    clearSSD1309();
    
    // Simple splash screen - can be customized
    char splash_text[] = "MaplePad";
    char splash_text2[] = "RP2350";
    
    // These would use your existing font rendering functions
    // putString(splash_text, 2, 2, 1);
    // putString(splash_text2, 3, 4, 1);
    
    updateSSD1309();
    sleep_ms(2000);
}

void setPixelSSD1309(int x, int y, bool on) {
    if (x >= SSD1309_LCDWIDTH || y >= SSD1309_LCDHEIGHT || x < 0 || y < 0) {
        return;
    }

    int byte_idx = x + (y / 8) * SSD1309_LCDWIDTH;
    int bit_idx = y & 7;

    if (on) {
        frameBuffer[byte_idx] |= (1 << bit_idx);
    } else {
        frameBuffer[byte_idx] &= ~(1 << bit_idx);
    }
}