/*
 * Xbox 360 Controller USB Host Integration - Fixed API Version
 * Compatible with Pico SDK v1.5.1 TinyUSB
 */

#include <string.h>
#include <math.h>
#include <stdlib.h>  // Add this for abs() function
#include "xbox360_usb.h"
#include "maple.h"    // Include maple.h for dreamcast_state_t definition

// Global controller state
xbox360_controller_t xbox_controller = {0};
static dreamcast_state_t dc_state_storage = {0};  // Static storage for the state

// Deadzone settings (configurable)
#define STICK_DEADZONE_THRESHOLD 8000    // Out of 32767
#define TRIGGER_DEADZONE_THRESHOLD 30    // Out of 255

bool xbox360_init(void) {
    printf("Initializing Xbox 360 Controller USB Host...\n");
    
    // Initialize USB controller state
    memset(&xbox_controller, 0, sizeof(xbox_controller));
    xbox_controller.dc_state = &dc_state_storage;  // Point to static storage
    
    // Initialize TinyUSB host stack
    if (!tusb_init()) {
        printf("Failed to initialize TinyUSB host stack\n");
        return false;
    }
    
    printf("Xbox 360 Controller USB Host initialized\n");
    return true;
}

void xbox360_task(void) {
    // Service TinyUSB host stack
    tuh_task();
    
    // Update Dreamcast state if controller is connected
    if (xbox_controller.connected && xbox_controller.ready) {
        xbox360_update_dreamcast_mapping();
    }
}

bool xbox360_is_connected(void) {
    return xbox_controller.connected && xbox_controller.ready;
}

dreamcast_state_t* xbox360_get_dreamcast_state(void) {
    return xbox_controller.dc_state;
}

// USB HID mount callback - called when Xbox 360 controller is connected
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
    printf("HID device mounted: dev_addr=%d, instance=%d\n", dev_addr, instance);
    
    // For now, assume any HID device could be an Xbox 360 controller
    // In a more robust implementation, you'd parse the HID report descriptor
    // or use other methods to identify the device
    
    printf("Assuming Xbox 360 Controller detected!\n");
    
    xbox_controller.connected = true;
    xbox_controller.dev_addr = dev_addr;
    xbox_controller.instance = instance;
    xbox_controller.vid = XBOX360_VID_MICROSOFT;
    xbox_controller.pid = XBOX360_PID_CONTROLLER;
    
    // Request to receive reports
    if (tuh_hid_receive_report(dev_addr, instance)) {
        xbox_controller.ready = true;
        printf("Xbox 360 Controller ready for input\n");
    }
}

// USB HID unmount callback - called when controller is disconnected
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    printf("HID device unmounted: dev_addr=%d, instance=%d\n", dev_addr, instance);
    
    if (xbox_controller.dev_addr == dev_addr && xbox_controller.instance == instance) {
        printf("Xbox 360 Controller disconnected\n");
        memset(&xbox_controller, 0, sizeof(xbox_controller));
    }
}

// USB HID report received callback - called when controller sends input data
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    if (xbox_controller.dev_addr == dev_addr && xbox_controller.instance == instance) {
        
        // Verify report size (minimum for Xbox 360 controller)
        if (len >= 20) {  // Xbox 360 controller reports are typically 20 bytes
            
            // Copy report data (simplified - real Xbox 360 parsing would be more complex)
            memcpy(&xbox_controller.last_report, report, sizeof(xbox360_report_t));
            xbox_controller.last_report_time = time_us_32();
            
            // Update Dreamcast state
            xbox360_update_dreamcast_mapping();
            
            printf("Xbox360 report: len=%d\n", len);
        }
        
        // Continue receiving reports
        tuh_hid_receive_report(dev_addr, instance);
    }
}

// Convert Xbox 360 button layout to Dreamcast controller layout
uint16_t xbox360_to_dreamcast_buttons(uint16_t xbox_buttons) {
    uint16_t dc_buttons = 0;
    
    // Map Xbox 360 buttons to Dreamcast equivalents
    if (xbox_buttons & XBOX360_BTN_A) dc_buttons |= DC_BTN_A;
    if (xbox_buttons & XBOX360_BTN_B) dc_buttons |= DC_BTN_B;
    if (xbox_buttons & XBOX360_BTN_X) dc_buttons |= DC_BTN_X;
    if (xbox_buttons & XBOX360_BTN_Y) dc_buttons |= DC_BTN_Y;
    if (xbox_buttons & XBOX360_BTN_START) dc_buttons |= DC_BTN_START;
    if (xbox_buttons & XBOX360_BTN_DPAD_UP) dc_buttons |= DC_BTN_DPAD_UP;
    if (xbox_buttons & XBOX360_BTN_DPAD_DOWN) dc_buttons |= DC_BTN_DPAD_DOWN;
    if (xbox_buttons & XBOX360_BTN_DPAD_LEFT) dc_buttons |= DC_BTN_DPAD_LEFT;
    if (xbox_buttons & XBOX360_BTN_DPAD_RIGHT) dc_buttons |= DC_BTN_DPAD_RIGHT;
    if (xbox_buttons & XBOX360_BTN_LB) dc_buttons |= DC_BTN_Z;  // Left bumper -> Z
    if (xbox_buttons & XBOX360_BTN_RB) dc_buttons |= DC_BTN_C;  // Right bumper -> C
    
    return dc_buttons;
}

// Convert Xbox 360 trigger (0-255) to Dreamcast trigger (0-255)
uint8_t xbox360_to_dreamcast_trigger(uint8_t xbox_trigger) {
    // Apply deadzone
    if (xbox_trigger < TRIGGER_DEADZONE_THRESHOLD) {
        return 0;
    }
    
    // Scale and return
    return xbox_trigger;
}

// Convert Xbox 360 stick value (-32768 to 32767) to Dreamcast stick (0-255, 128=center)
uint8_t xbox360_to_dreamcast_stick(int16_t xbox_stick_value) {
    // Apply deadzone
    if (abs(xbox_stick_value) < STICK_DEADZONE_THRESHOLD) {
        return 128; // Center position
    }
    
    // Convert from signed 16-bit to unsigned 8-bit
    int32_t scaled = ((int32_t)xbox_stick_value + 32768) / 256;
    
    // Clamp to valid range
    if (scaled < 0) scaled = 0;
    if (scaled > 255) scaled = 255;
    
    return (uint8_t)scaled;
}

// Apply deadzone to stick coordinates
void xbox360_apply_deadzone(int16_t* stick_x, int16_t* stick_y) {
    // Calculate magnitude
    float magnitude = sqrtf((float)(*stick_x * *stick_x) + (float)(*stick_y * *stick_y));
    
    // Apply deadzone
    if (magnitude < STICK_DEADZONE_THRESHOLD) {
        *stick_x = 0;
        *stick_y = 0;
    }
}

// Apply calibration to triggers
void xbox360_calibrate_triggers(uint8_t* left_trigger, uint8_t* right_trigger) {
    // Apply deadzone
    if (*left_trigger < TRIGGER_DEADZONE_THRESHOLD) *left_trigger = 0;
    if (*right_trigger < TRIGGER_DEADZONE_THRESHOLD) *right_trigger = 0;
}

// Update Dreamcast controller state from Xbox 360 input
void xbox360_update_dreamcast_mapping(void) {
    if (!xbox_controller.connected || !xbox_controller.ready || !xbox_controller.dc_state) {
        return;
    }
    
    xbox360_report_t* report = &xbox_controller.last_report;
    dreamcast_state_t* dc_state = xbox_controller.dc_state;
    
    // Map buttons
    dc_state->buttons = xbox360_to_dreamcast_buttons(report->buttons);
    
    // Map triggers
    dc_state->left_trigger = xbox360_to_dreamcast_trigger(report->left_trigger);
    dc_state->right_trigger = xbox360_to_dreamcast_trigger(report->right_trigger);
    
    // Map analog stick (left stick only for basic Dreamcast controller)
    dc_state->stick_x = xbox360_to_dreamcast_stick(report->left_stick_x);
    dc_state->stick_y = xbox360_to_dreamcast_stick(report->left_stick_y);
}