// COMPLETE src/display.c FILE
// Replace your entire display.c file with this

#include "display.h"
#include "ssd1306.h"
#include "ssd1309.h"  
#include "ssd1331.h"
#include "font.h"
#include "maple.h"

// External variables
extern tFont Font;  // From font.c
extern uint16_t color;

// Framebuffer for monochrome displays (SSD1306/SSD1309)
// This MUST be defined here, not as extern
uint8_t frameBuffer[SSD1309_FRAMEBUFFER_SIZE];  // 1024 bytes for 128x64

// Remove the conflicting extern declaration for oledType
// We'll access it via the flashData[21] macro directly

// Updated OLED detection logic
uint8_t detect_oled_type(void) {
    // Check OLED_PIN (22) for display type selection
    uint8_t pin_state = gpio_get(OLED_PIN);
    
    if (pin_state == 0) {
        return DISPLAY_SSD1306;
    } else {
        return DISPLAY_SSD1331;  // Default to SSD1331 for backward compatibility
    }
}

// Enhanced display initialization with SSD1309 support
void displayInit() {
    flashData[21] = detect_oled_type();  // Set oledType directly
    
    switch(flashData[21]) {  // Use flashData[21] instead of oledType
        case DISPLAY_SSD1306:
            ssd1306_init();
            break;
        case DISPLAY_SSD1331:
            ssd1331_init();
            break;
        case DISPLAY_SSD1309:
            ssd1309_init();
            break;
        default:
            // Fallback to SSD1306
            ssd1306_init();
            flashData[21] = DISPLAY_SSD1306;
            break;
    }
}

void updateDisplay() {
    switch(flashData[21]) {
        case DISPLAY_SSD1306:
            updateSSD1306();
            break;
        case DISPLAY_SSD1331:
            updateSSD1331();
            break;
        case DISPLAY_SSD1309:
            updateSSD1309();
            break;
        default:
            updateSSD1306();
            break;
    }
}

void clearDisplay() {
    switch(flashData[21]) {
        case DISPLAY_SSD1306:
            clearSSD1306();
            break;
        case DISPLAY_SSD1331:
            clearSSD1331();
            break;
        case DISPLAY_SSD1309:
            clearSSD1309();
            break;
        default:
            clearSSD1306();
            break;
    }
}

void splashDisplay() {
    switch(flashData[21]) {
        case DISPLAY_SSD1306:
            splashSSD1306();
            break;
        case DISPLAY_SSD1331:
            splashSSD1331();
            break;
        case DISPLAY_SSD1309:
            splashSSD1309();
            break;
        default:
            splashSSD1306();
            break;
    }
}

// Unified pixel setting function
void setDisplayPixel(int x, int y, bool on) {
    switch(flashData[21]) {
        case DISPLAY_SSD1306:
            setPixelSSD1306(x, y, on);
            break;
        case DISPLAY_SSD1331:
            // SSD1331 uses different parameters (color instead of bool)
            setPixelSSD1331(x, y, on ? color : 0);
            break;
        case DISPLAY_SSD1309:
            setPixelSSD1309(x, y, on);
            break;
        default:
            setPixelSSD1306(x, y, on);
            break;
    }
}

// Get display type string for menu display
const char* getDisplayTypeString() {
    switch(flashData[21]) {
        case DISPLAY_SSD1306:
            return "SSD1306";
        case DISPLAY_SSD1331:
            return "SSD1331";
        case DISPLAY_SSD1309:
            return "SSD1309";
        default:
            return "Unknown";
    }
}

// Check if display supports color (for menu options)
bool displaySupportsColor() {
    return (flashData[21] == DISPLAY_SSD1331);
}

// Get display dimensions
void getDisplayDimensions(int *width, int *height) {
    switch(flashData[21]) {
        case DISPLAY_SSD1306:
            *width = SSD1306_LCDWIDTH;
            *height = SSD1306_LCDHEIGHT;
            break;
        case DISPLAY_SSD1331:
            *width = OLED_W;  // 96
            *height = OLED_H; // 64
            break;
        case DISPLAY_SSD1309:
            *width = SSD1309_LCDWIDTH;
            *height = SSD1309_LCDHEIGHT;
            break;
        default:
            *width = 128;
            *height = 64;
            break;
    }
}

// Find character in font array
tImage* findChar(uint8_t c) {
    for (int i = 0; i < Font.length; i++) {
        if (Font.chars[i].code == c) {
            return Font.chars[i].image;
        }
    }
    // Return space character if not found
    for (int i = 0; i < Font.length; i++) {
        if (Font.chars[i].code == 0x20) { // space character
            return Font.chars[i].image;
        }
    }
    return NULL;
}

// Put a single letter/character on display
void putLetter(int x, int y, uint8_t letter, uint16_t color) {
    tImage* char_image = findChar(letter);
    if (!char_image) return;
    
    switch(flashData[21]) {
        case DISPLAY_SSD1306:
        case DISPLAY_SSD1309:
            // For monochrome displays
            for (int cy = 0; cy < char_image->height; cy++) {
                for (int cx = 0; cx < char_image->width; cx++) {
                    int byte_idx = cy;
                    int bit_idx = cx;
                    
                    if (byte_idx < char_image->dataSize) {
                        uint8_t pixel_data = char_image->data[byte_idx];
                        bool pixel_on = (pixel_data & (1 << bit_idx)) != 0;
                        
                        if (pixel_on) {
                            setDisplayPixel(x + cx, y + cy, true);
                        }
                    }
                }
            }
            break;
            
        case DISPLAY_SSD1331:
            // For color display (SSD1331)
            for (int cy = 0; cy < char_image->height; cy++) {
                for (int cx = 0; cx < char_image->width; cx++) {
                    int byte_idx = cy;
                    int bit_idx = cx;
                    
                    if (byte_idx < char_image->dataSize) {
                        uint8_t pixel_data = char_image->data[byte_idx];
                        bool pixel_on = (pixel_data & (1 << bit_idx)) != 0;
                        
                        if (pixel_on) {
                            setPixelSSD1331(x + cx, y + cy, color);
                        }
                    }
                }
            }
            break;
    }
}

// Put a string on display
void putString(char* str, int x, int y, uint16_t color) {
    if (!str) return;
    
    int current_x = x;
    int char_spacing = 7; // Adjust based on your font width + spacing
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            // Handle newline
            current_x = x;
            y += 12; // Move to next line (adjust based on font height)
            continue;
        }
        
        putLetter(current_x, y, str[i], color);
        current_x += char_spacing;
        
        // Word wrap if needed (optional)
        int display_width;
        int display_height;
        getDisplayDimensions(&display_width, &display_height);
        
        if (current_x >= display_width - char_spacing) {
            current_x = x;
            y += 12;
        }
    }
}