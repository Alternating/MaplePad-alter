/*
 * MaplePad with Xbox 360 Controller Support
 * Dreamcast controller emulator for Raspberry Pi Pico (RP2040/RP2350)
 * © Charlie Cole 2021
 *
 * Modified by Mackie Kannard-Smith (mackieks / YveltalGriffin) 2022
 * Enhanced for RP2350 with SD card, SSD1309 support, and Xbox 360 USB Host
 */

#include "maple.h"
#include "display.h"
#include "sdcard.h"
#include "menu.h"
#include "xbox360_usb.h"

// RP2350 primary target with RP2040 backward compatibility
#ifdef PICO_RP2040
#define FLASH_OFFSET (128 * 1024) // Smaller offset for RP2040 compatibility
#define MAX_FLASH_SIZE (2 * 1024 * 1024) // 2MB flash on RP2040
#else
// Default to RP2350 (primary target)
#define FLASH_OFFSET (256 * 1024) // Larger offset for RP2350's expanded flash
#define MAX_FLASH_SIZE (4 * 1024 * 1024) // 4MB flash on RP2350
#endif

// Maple Bus Defines and Funcs
#define SHOULD_SEND 1  // Set to zero to sniff two devices sending signals to each other
#define SHOULD_PRINT 0 // Nice for debugging but can cause timing issues

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

#define ADDRESS_DREAMCAST 0
#define ADDRESS_CONTROLLER 0x20
#define ADDRESS_SUBPERIPHERAL0 0x01
#define ADDRESS_SUBPERIPHERAL1 0x02

// Global variable definitions
uint8_t MemoryCard[2048] = {0};     // VMU memory card storage
uint8_t flashData[64] = {0};        // Flash configuration data, initialized to zero
uint16_t color = 0xFFFF;            // Display color (white)
bool sd_card_available = false;

// Maple communication variables
static PIO maple_tx_pio = pio0;
static PIO maple_rx_pio = pio1;
static uint maple_tx_sm = 0;
static uint maple_rx_sm = 0;

// Controller input source selection
typedef enum {
    INPUT_SOURCE_NONE = 0,
    INPUT_SOURCE_XBOX360_USB,
    INPUT_SOURCE_INTERNAL_TEST  // For testing without controller
} input_source_t;

static input_source_t current_input_source = INPUT_SOURCE_NONE;

// Function prototypes
void initialize_peripherals(void);
void initialize_maple_bus(void);
void initialize_usb_host(void);
bool save_vmu_to_sd(uint8_t page);
bool load_vmu_from_sd(uint8_t page);
void rp2350_optimizations(void);
void handle_maple_communication(void);
void update_input_source(void);
void send_dreamcast_controller_data(dreamcast_state_t* state);

// Flash memory functions
void readFlash(void) {
    // Read flash memory configuration
    #ifdef PICO_HW
    // RP2350-optimized flash reading with RP2040 fallback
    const uint8_t *flash_contents = (const uint8_t *)(XIP_BASE + FLASH_OFFSET);
    
    // Verify flash bounds for safety
    if (FLASH_OFFSET + sizeof(flashData) + sizeof(MemoryCard) > MAX_FLASH_SIZE) {
        printf("Warning: Flash offset exceeds available flash size\n");
        memset(flashData, 0, sizeof(flashData));
        memset(MemoryCard, 0, sizeof(MemoryCard));
        return;
    }
    
    memcpy(flashData, flash_contents, sizeof(flashData));
    memcpy(MemoryCard, flash_contents + sizeof(flashData), sizeof(MemoryCard));
    printf("Flash data read successfully from offset 0x%X\n", FLASH_OFFSET);
    #else
    // Initialize with defaults for non-hardware builds
    memset(flashData, 0, sizeof(flashData));
    memset(MemoryCard, 0, sizeof(MemoryCard));
    printf("Flash read placeholder - using defaults\n");
    #endif
}

void updateFlashData(void) {
    // Write flash memory configuration - optimized for RP2350
    #ifdef PICO_HW
    // Disable interrupts during flash write
    uint32_t interrupts = save_and_disable_interrupts();
    
    // Verify flash bounds
    if (FLASH_OFFSET + FLASH_SECTOR_SIZE > MAX_FLASH_SIZE) {
        printf("Error: Flash write would exceed available flash\n");
        restore_interrupts(interrupts);
        return;
    }
    
    // Erase flash sector
    flash_range_erase(FLASH_OFFSET, FLASH_SECTOR_SIZE);
    
    // Prepare data to write (RP2350 can handle larger sectors efficiently)
    uint8_t write_buffer[FLASH_SECTOR_SIZE];
    memset(write_buffer, 0, FLASH_SECTOR_SIZE);
    memcpy(write_buffer, flashData, sizeof(flashData));
    memcpy(write_buffer + sizeof(flashData), MemoryCard, sizeof(MemoryCard));
    
    // Write to flash
    flash_range_program(FLASH_OFFSET, write_buffer, FLASH_SECTOR_SIZE);
    
    // Restore interrupts
    restore_interrupts(interrupts);
    printf("Flash data written successfully to RP2350 flash\n");
    #else
    printf("Flash write placeholder - data saved to memory\n");
    #endif
}

// Initialize Maple bus PIO communication (RP2350 optimized)
void initialize_maple_bus(void) {
    printf("Initializing Maple bus communication (RP2350 enhanced)...\n");
    
    // Initialize Maple bus pins
    gpio_init(MAPLE_A);
    gpio_init(MAPLE_B);
    
    // Set pins as inputs initially (will be controlled by PIO)
    gpio_set_dir(MAPLE_A, GPIO_IN);
    gpio_set_dir(MAPLE_B, GPIO_IN);
    
    // Add pull-ups to prevent floating
    gpio_pull_up(MAPLE_A);
    gpio_pull_up(MAPLE_B);
    
    // RP2350 has enhanced PIO capabilities
    // - More PIO state machines available
    // - Higher clock speeds for PIO
    // - Better interrupt handling
    
    // TODO: Initialize PIO programs for Maple communication
    // RP2350 can handle more complex PIO programs with better performance
    // This is where the maple.pio TX/RX programs would be loaded
    // and state machines configured for MAPLE_A and MAPLE_B pins
    
    printf("Maple bus pins GP%d and GP%d initialized for RP2350\n", MAPLE_A, MAPLE_B);
}

// Initialize USB Host for Xbox 360 controllers
void initialize_usb_host(void) {
    printf("Initializing USB Host for Xbox 360 controllers...\n");
    
    if (xbox360_init()) {
        printf("USB Host initialized successfully\n");
    } else {
        printf("USB Host initialization failed\n");
    }
}

// Enhanced peripheral initialization
void initialize_peripherals(void) {
    printf("Initializing peripherals...\n");
    
    // Initialize display
    displayInit();
    printf("Display initialized\n");
    
    // Initialize SD card
    sd_card_available = sd_init();
    if (sd_card_available) {
        printf("SD card initialized successfully\n");
    } else {
        printf("SD card initialization failed\n");
    }
    
    // Read flash configuration
    readFlash();
    
    // Initialize Maple bus
    initialize_maple_bus();
    
    // Initialize USB Host for Xbox 360 controllers
    initialize_usb_host();
    
    // Initialize OLED detection pin
    gpio_init(OLED_PIN);
    gpio_set_dir(OLED_PIN, GPIO_IN);
    gpio_pull_up(OLED_PIN);
    
    // Initialize page button for VMU page cycling
    gpio_init(PAGE_BUTTON);
    gpio_set_dir(PAGE_BUTTON, GPIO_IN);
    gpio_pull_up(PAGE_BUTTON);
    
    printf("All peripherals initialized\n");
}

// VMU save/load functions
bool save_vmu_to_sd(uint8_t page) {
    if (!sd_card_available) {
        printf("SD card not available\n");
        return false;
    }
    
    uint32_t block_addr = 100 + (page * 4); // Each VMU page uses 4 blocks
    
    // Write 4 blocks (2048 bytes) for the VMU page
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
    if (!sd_card_available) {
        printf("SD card not available\n");
        return false;
    }
    
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

// RP2350 optimizations and features (primary target)
void rp2350_optimizations(void) {
    // RP2350 enhanced features
    printf("Applying RP2350 optimizations...\n");
    
    // Higher clock speeds available on RP2350
    // set_sys_clock_khz(150000, true); // 150MHz (uncomment when stable)
    
    // ARM Cortex-M33 specific features
    // - Hardware floating point unit
    // - TrustZone support (if needed)
    // - Enhanced DSP instructions
    
    // Additional GPIO pins (26-29) available on RP2350
    // PSRAM support if equipped
    
    printf("RP2350 optimizations configured\n");
}

#ifdef PICO_RP2040
void rp2040_compatibility(void) {
    printf("Running in RP2040 compatibility mode\n");
    // Ensure we don't use RP2350-specific features
    // Lower clock speeds and reduced memory usage
}
#endif

// Update input source based on connected devices
void update_input_source(void) {
    static input_source_t last_source = INPUT_SOURCE_NONE;
    
    // Check for Xbox 360 controller
    if (xbox360_is_connected()) {
        current_input_source = INPUT_SOURCE_XBOX360_USB;
    } else {
        current_input_source = INPUT_SOURCE_NONE;
    }
    
    // Log source changes
    if (current_input_source != last_source) {
        switch (current_input_source) {
            case INPUT_SOURCE_XBOX360_USB:
                printf("Input source: Xbox 360 Controller (USB)\n");
                break;
            case INPUT_SOURCE_NONE:
                printf("Input source: None (no controller detected)\n");
                break;
            default:
                printf("Input source: Unknown\n");
                break;
        }
        last_source = current_input_source;
    }
}

// Send controller data to Dreamcast via Maple bus
void send_dreamcast_controller_data(dreamcast_state_t* state) {
    // TODO: Implement actual Maple bus transmission using PIO
    // This function will encode the controller state into Maple bus format
    // and transmit it to the Dreamcast
    
    // For now, just log the data being sent (debug)
    #ifdef MAPLE_DEBUG
    printf("Sending to DC: btns=%04X, LT=%d, RT=%d, X=%d, Y=%d\n",
           state->buttons, state->left_trigger, state->right_trigger,
           state->stick_x, state->stick_y);
    #endif
    
    // Placeholder for PIO transmission
    // The actual implementation would:
    // 1. Format data according to Maple bus protocol
    // 2. Load data into PIO TX FIFO
    // 3. Trigger PIO state machine to send data
}

// Page cycling function using PAGE_BUTTON
void check_page_button(void) {
    static uint32_t last_page_press = 0;
    static bool button_was_pressed = false;
    uint32_t current_time = time_us_32();
    
    bool button_pressed = !gpio_get(PAGE_BUTTON);
    
    // Check for button press with debounce
    if (button_pressed && !button_was_pressed && (current_time - last_page_press) > 500000) {
        currentPage++;
        if (currentPage > 8) currentPage = 1; // Cycle through pages 1-8
        
        // Update display to show page change
        clearDisplay();
        putString("VMU Page:", 0, 0, color);
        char page_str[16];
        sprintf(page_str, "%d", currentPage);
        putString(page_str, 0, 1, color);
        updateDisplay();
        
        last_page_press = current_time;
        printf("Switched to VMU page %d\n", currentPage);
    }
    
    button_was_pressed = button_pressed;
}

// Main Maple bus communication handler with Xbox 360 input
void handle_maple_communication(void) {
    static uint32_t last_status_update = 0;
    static uint32_t last_controller_update = 0;
    uint32_t current_time = time_us_32();
    
    // Service USB Host stack for Xbox 360 controllers
    xbox360_task();
    
    // Update input source detection
    update_input_source();
    
    // Handle controller input at 60Hz (16.67ms intervals)
    if ((current_time - last_controller_update) > 16670) {
        if (current_input_source == INPUT_SOURCE_XBOX360_USB) {
            dreamcast_state_t* dc_state = xbox360_get_dreamcast_state();
            if (dc_state) {
                // Send controller data to Dreamcast
                send_dreamcast_controller_data(dc_state);
            }
        }
        last_controller_update = current_time;
    }
    
    // Update display at 1Hz (1 second intervals)
    if ((current_time - last_status_update) > 1000000) {
        clearDisplay();
        putString("MaplePad", 0, 0, color);
        
        // Show current VMU page
        char page_str[16];
        sprintf(page_str, "Page: %d", currentPage);
        putString(page_str, 0, 1, color);
        
        // Show input source
        switch (current_input_source) {
            case INPUT_SOURCE_XBOX360_USB:
                putString("Xbox360: OK", 0, 2, color);
                break;
            case INPUT_SOURCE_NONE:
                putString("No Controller", 0, 2, color);
                break;
            default:
                putString("Input: Unknown", 0, 2, color);
                break;
        }
        
        // Show SD card status
        if (sd_card_available) {
            putString("SD: OK", 0, 3, color);
        } else {
            putString("SD: --", 0, 3, color);
        }
        
        updateDisplay();
        last_status_update = current_time;
    }
    
    // Check for page button presses
    check_page_button();
    
    // TODO: Handle incoming Maple bus communication from Dreamcast
    // This would include:
    // - Device info requests
    // - Controller data requests  
    // - VMU memory operations
    // - Rumble commands
}

// Main function
int main() {
    stdio_init_all();
    
    printf("MaplePad with Xbox 360 Controller Support Starting...\n");
    printf("Firmware Version: %02X\n", CURRENT_FW_VERSION);
    
#ifdef PICO_RP2040
    printf("Running on RP2040 (compatibility mode)\n");
    rp2040_compatibility();
#else
    printf("Running on RP2350 (primary target)\n");
    rp2350_optimizations();
#endif
    
    // Initialize all peripherals
    initialize_peripherals();
    
    // Show startup splash
    clearDisplay();
    putString("MaplePad", 0, 0, color);
    putString("Xbox360 Ready", 0, 1, color);
    char version_str[16];
    sprintf(version_str, "v%02X", CURRENT_FW_VERSION);
    putString(version_str, 0, 2, color);
    putString("Insert Controller", 0, 3, color);
    updateDisplay();
    sleep_ms(3000);
    
    printf("Initialization complete. Starting Maple communication with Xbox 360 support...\n");
    
    // Main application loop - Xbox 360 to Dreamcast bridge
    while (true) {
        handle_maple_communication();
        
        // Small delay to prevent excessive CPU usage
        sleep_us(100);
    }
    
    return 0;
}