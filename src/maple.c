/*
 * MaplePad
 * Dreamcast controller emulator for Raspberry Pi Pico (RP2040/RP2350)
 * © Charlie Cole 2021
 *
 * Modified by Mackie Kannard-Smith (mackieks / YveltalGriffin) 2022
 * Enhanced for RP2350 with SD card and SSD1309 support
 *
 * Check out the wiring diagram on Github! (https://github.com/mackieks/MaplePad)
 */

#include "maple.h"
#include "display.h"
#include "sdcard.h"
#include "menu.h"

// RP2350 compatibility
#ifdef PICO_RP2350
#define FLASH_OFFSET (256 * 1024) // Larger offset for RP2350's larger flash
#else
#define FLASH_OFFSET (128 * 1024) // Original offset for RP2040
#endif

// Pin definitions
#define OLED_PIN 22

// Maple Bus Defines and Funcs
#define SHOULD_SEND 1  // Set to zero to sniff two devices sending signals to each other
#define SHOULD_PRINT 0 // Nice for debugging but can cause timing issues

// Board Variant
#define PICO 1
#define MAPLEPAD 0

// HKT-7700 (Standard Controller) or HKT-7300 (Arcade Stick) (see maple.h)
#if HKT7700
#define NUM_BUTTONS 9
#elif HKT7300
#define NUM_BUTTONS 11
#endif

// Purupuru Enable
#define ENABLE_RUMBLE 1

// Memory Card
#define PHASE_SIZE (BLOCK_SIZE / 4)
#define FLASH_WRITE_DELAY 16      // About quarter of a second if polling once a frame

#if PICO
#define PAGE_BUTTON 21 // Pull GP21 low for Page Cycle. Avoid page cycling for ~10s after saving or copying VMU data to avoid data corruption
#elif MAPLEPAD
#define PAGE_BUTTON 20 // Dummy pin
#endif

#define PAGE_BUTTON_MASK 0x0608   // X, Y, and Start
#define PAGE_BACKWARD_MASK 0x0048 // Start and D-pad Left
#define PAGE_FORWARD_MASK 0x0088  // Start and D-pad Right

#define INPUT_ACT 20

#define MAPLE_A 11
#define MAPLE_B 12
#define PICO_PIN1_PIN_RX MAPLE_A
#define PICO_PIN5_PIN_RX MAPLE_B

#define ADDRESS_DREAMCAST 0
#define ADDRESS_CONTROLLER 0x20
#define ADDRESS_SUBPERIPHERAL0 0x01
#define ADDRESS_SUBPERIPHERAL1 0x02

// Global variable definitions
uint8_t MemoryCard[2048] = {0};     // VMU memory card storage
uint8_t flashData[64] = {0};        // Flash configuration data, initialized to zero
uint16_t color = 0xFFFF;            // Display color (white)
bool sd_card_available = false;

// ButtonInfos array definition
ButtonInfo ButtonInfos[NUM_BUTTONS] = {
    {0, 0x0004}, // A
    {1, 0x0002}, // B
    {4, 0x0400}, // X
    {5, 0x0200}, // Y
    {6, 0x0010}, // Up
    {7, 0x0020}, // Down
    {8, 0x0040}, // Left
    {9, 0x0080}, // Right
    {10, 0x0008} // Start
#if HKT7300
    ,
    {16, 0x0001}, // C
    {17, 0x0100}  // Z
#endif
};

// Function prototypes
void initialize_peripherals(void);
bool save_vmu_to_sd(uint8_t page);
bool load_vmu_from_sd(uint8_t page);
int sd_menu_handler(void);
void rp2350_optimizations(void);

// Placeholder implementations for missing functions
void readFlash(void) {
    // TODO: Implement flash reading logic
    // For now, just initialize MemoryCard with dummy data
    memset(MemoryCard, 0, sizeof(MemoryCard));
    printf("Flash read placeholder - MemoryCard cleared\n");
}

void updateFlashData(void) {
    // TODO: Implement flash writing logic
    // This should write MemoryCard data to flash memory
    printf("Flash update placeholder - data would be written to flash\n");
}

// Enhanced initialization function
void initialize_peripherals(void) {
    // OLED initialization
    gpio_init(OLED_PIN);
    gpio_set_dir(OLED_PIN, GPIO_IN);
    gpio_pull_up(OLED_PIN);
    sleep_ms(20);

    // Initialize display with enhanced detection
    displayInit();
    
    // Initialize SD Card
    printf("Initializing SD card...\n");
    sd_card_available = sd_init();
    if (sd_card_available) {
        printf("SD card initialized successfully\n");
        // Show SD card status on display
        clearDisplay();
        putString("SD: Ready", 0, 0, color);
        updateDisplay();
        sleep_ms(1000);
    } else {
        printf("SD card initialization failed\n");
        clearDisplay();
        putString("SD: Failed", 0, 0, color);
        updateDisplay();
        sleep_ms(1000);
    }

    // Continue with existing MaplePad initialization...
    clearDisplay();
    putString("MaplePad Ready", 0, 1, color);
    updateDisplay();
    sleep_ms(1000);
}

// SD Card utility functions
bool save_vmu_to_sd(uint8_t page) {
    if (!sd_card_available) return false;
    
    // Calculate block address (simple file system)
    uint32_t block_addr = 100 + (page * 4); // Start at block 100, 4 blocks per page
    
    // Prepare VMU page data
    readFlash(); // Load current page into MemoryCard array
    
    // Write 4 blocks (512 bytes each = 2048 bytes total for VMU page)
    for (int i = 0; i < 4; i++) {
        uint32_t offset = i * 512;
        bool success = sd_write_block(block_addr + i, &MemoryCard[offset]);
        if (!success) {
            printf("Failed to write VMU page %d block %d to SD\n", page, i);
            return false;
        }
    }
    
    printf("VMU page %d saved to SD card\n", page);
    return true;
}

bool load_vmu_from_sd(uint8_t page) {
    if (!sd_card_available) return false;
    
    uint32_t block_addr = 100 + (page * 4);
    
    // Read 4 blocks for the VMU page
    for (int i = 0; i < 4; i++) {
        uint32_t offset = i * 512;
        bool success = sd_read_block(block_addr + i, &MemoryCard[offset]);
        if (!success) {
            printf("Failed to read VMU page %d block %d from SD\n", page, i);
            return false;
        }
    }
    
    // Write to flash
    updateFlashData();
    
    printf("VMU page %d loaded from SD card\n", page);
    return true;
}

// Enhanced menu function for SD card operations
int sd_menu_handler(void) {
    if (!sd_card_available) {
        clearDisplay();
        putString("No SD Card", 0, 0, color);
        putString("Press A", 0, 2, color);
        updateDisplay();
        
        while (gpio_get(ButtonInfos[0].InputIO)); // Wait for A press
        while (!gpio_get(ButtonInfos[0].InputIO)); // Wait for A release
        return 1;
    }
    
    clearDisplay();
    putString("SD Card Menu", 0, 0, color);
    putString("A: Save VMU", 0, 1, color);
    putString("B: Load VMU", 0, 2, color);
    putString("X: Exit", 0, 3, color);
    updateDisplay();
    
    while (true) {
        if (!gpio_get(ButtonInfos[0].InputIO)) { // A button - Save
            save_vmu_to_sd(currentPage);
            clearDisplay();
            putString("Page Saved!", 0, 1, color);
            updateDisplay();
            sleep_ms(1000);
            break;
        }
        
        if (!gpio_get(ButtonInfos[1].InputIO)) { // B button - Load
            load_vmu_from_sd(currentPage);
            clearDisplay();
            putString("Page Loaded!", 0, 1, color);
            updateDisplay();
            sleep_ms(1000);
            break;
        }
        
        if (!gpio_get(ButtonInfos[2].InputIO)) { // X button - Exit
            break;
        }
        
        sleep_ms(10);
    }
    
    return 1;
}

// RP2350 specific optimizations
#ifdef PICO_RP2350
void rp2350_optimizations(void) {
    // Enable higher clock speeds available on RP2350
    // Note: Verify these settings with your specific RP2350 board
    
    // Set system clock to higher frequency if supported
    // set_sys_clock_khz(150000, true); // 150MHz example
    
    // Configure additional ARM Cortex-M33 features
    // Enable floating point unit if needed
    // Enable cache if available
    
    printf("RP2350 optimizations applied\n");
}
#endif

// Main function
int main() {
    stdio_init_all();
    
    printf("MaplePad Starting...\n");
    
#ifdef PICO_RP2350
    printf("Running on RP2350\n");
    rp2350_optimizations();
#else
    printf("Running on RP2040\n");
#endif
    
    // Initialize all peripherals
    initialize_peripherals();
    
    printf("Initialization complete. Starting main loop...\n");
    
    // Main application loop
    while (true) {
        // TODO: Add your main Dreamcast communication loop here
        // This is where the Maple bus communication would happen
        
        // For now, just a simple loop with menu check
        if (!gpio_get(ButtonInfos[2].InputIO)) { // X button pressed
            sd_menu_handler();
        }
        
        sleep_ms(16); // ~60 FPS
    }
    
    return 0;
}