/*
 * Xbox 360 Controller USB Host Integration
 * USB-C interface for Xbox 360 controllers/dongles
 * Translates Xbox 360 input to Dreamcast Maple bus commands
 */

#ifndef USB_XBOX360_H
#define USB_XBOX360_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "host/usbh.h"
#include "class/hid/hid_host.h"

// Xbox 360 Controller USB identifiers
#define XBOX360_VID_MICROSOFT    0x045E
#define XBOX360_PID_CONTROLLER   0x028E
#define XBOX360_PID_WIRELESS     0x0719
#define XBOX360_PID_CHATPAD      0x0291

// Xbox 360 button mappings (USB HID format)
#define XBOX360_BTN_DPAD_UP      0x0001
#define XBOX360_BTN_DPAD_DOWN    0x0002
#define XBOX360_BTN_DPAD_LEFT    0x0004
#define XBOX360_BTN_DPAD_RIGHT   0x0008
#define XBOX360_BTN_START        0x0010
#define XBOX360_BTN_BACK         0x0020
#define XBOX360_BTN_LSTICK       0x0040
#define XBOX360_BTN_RSTICK       0x0080
#define XBOX360_BTN_LB           0x0100
#define XBOX360_BTN_RB           0x0200
#define XBOX360_BTN_GUIDE        0x0400
#define XBOX360_BTN_A            0x1000
#define XBOX360_BTN_B            0x2000
#define XBOX360_BTN_X            0x4000
#define XBOX360_BTN_Y            0x8000

// Dreamcast button mappings (for translation)
#define DC_BTN_C             0x0001
#define DC_BTN_B             0x0002
#define DC_BTN_A             0x0004
#define DC_BTN_START         0x0008
#define DC_BTN_DPAD_UP       0x0010
#define DC_BTN_DPAD_DOWN     0x0020
#define DC_BTN_DPAD_LEFT     0x0040
#define DC_BTN_DPAD_RIGHT    0x0080
#define DC_BTN_Z             0x0100
#define DC_BTN_Y             0x0200
#define DC_BTN_X             0x0400
#define DC_BTN_D             0x0800

// Xbox 360 Controller input report structure
typedef struct {
    uint8_t  report_id;
    uint8_t  report_size;
    uint16_t buttons;        // Button state bitfield
    uint8_t  left_trigger;   // 0-255
    uint8_t  right_trigger;  // 0-255
    int16_t  left_stick_x;   // -32768 to 32767
    int16_t  left_stick_y;   // -32768 to 32767
    int16_t  right_stick_x;  // -32768 to 32767
    int16_t  right_stick_y;  // -32768 to 32767
} __attribute__((packed)) xbox360_report_t;

// Forward declaration - dreamcast_state_t is defined in maple.h
struct dreamcast_state_s;
typedef struct dreamcast_state_s dreamcast_state_t;

// USB Host controller state
typedef struct {
    bool     connected;
    bool     ready;
    uint8_t  dev_addr;
    uint8_t  instance;
    uint16_t vid;
    uint16_t pid;
    xbox360_report_t last_report;
    dreamcast_state_t* dc_state;  // Use pointer instead of embedded struct
    uint32_t last_report_time;
} xbox360_controller_t;

// Global controller state
extern xbox360_controller_t xbox_controller;

// Function prototypes
bool xbox360_init(void);
void xbox360_task(void);
bool xbox360_is_connected(void);
dreamcast_state_t* xbox360_get_dreamcast_state(void);
void xbox360_update_dreamcast_mapping(void);

// USB Host callbacks
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len);
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance);
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len);

// Button mapping functions
uint16_t xbox360_to_dreamcast_buttons(uint16_t xbox_buttons);
uint8_t xbox360_to_dreamcast_trigger(uint8_t xbox_trigger);
uint8_t xbox360_to_dreamcast_stick(int16_t xbox_stick_value);

// Deadzone and calibration
void xbox360_apply_deadzone(int16_t* stick_x, int16_t* stick_y);
void xbox360_calibrate_triggers(uint8_t* left_trigger, uint8_t* right_trigger);

#endif // USB_XBOX360_H