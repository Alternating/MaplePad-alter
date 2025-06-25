/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// defined by compiler flags for flexibility
#ifndef CFG_TUSB_MCU
#define CFG_TUSB_MCU OPT_MCU_RP2040
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS OPT_OS_PICO
#endif

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG 0
#endif

// Enable Device and/or Host
#define CFG_TUD_ENABLED 0  // Device mode disabled
#define CFG_TUH_ENABLED 1  // Host mode enabled

// Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUD_MAX_SPEED OPT_MODE_DEFAULT_SPEED

//--------------------------------------------------------------------
// DEVICE CONFIGURATION (disabled for our use case)
//--------------------------------------------------------------------
#define CFG_TUD_ENDPOINT0_SIZE 64

// Device class configuration
#define CFG_TUD_CDC 0
#define CFG_TUD_MSC 0
#define CFG_TUD_HID 0
#define CFG_TUD_MIDI 0
#define CFG_TUD_VENDOR 0

//--------------------------------------------------------------------
// HOST CONFIGURATION (enabled for Xbox 360 controllers)
//--------------------------------------------------------------------

// Size of buffer to hold descriptors and other data used for enumeration
#define CFG_TUH_ENUMERATION_BUFSIZE 512

#define CFG_TUH_HUB 1  // Hub support
#define CFG_TUH_CDC 0  // CDC Host disabled
#define CFG_TUH_MSC 0  // MSC Host disabled
#define CFG_TUH_VENDOR 0  // Vendor Host disabled

// Max device support (excluding hub device)
#ifndef CFG_TUH_DEVICE_MAX
#define CFG_TUH_DEVICE_MAX (CFG_TUH_HUB ? 4 : 1)
#endif

//--------------------------------------------------------------------
// HID Host Configuration (for Xbox 360 controllers)
//--------------------------------------------------------------------

// Number of HID devices
#ifndef CFG_TUH_HID
#define CFG_TUH_HID 4
#endif

// Size of buffer to hold HID report descriptor
#define CFG_TUH_HID_REPORT_BUFSIZE 512

// Max number of reports that can be received/sent at the same time
#ifndef CFG_TUH_HID_REPORT_MAX
#define CFG_TUH_HID_REPORT_MAX 8
#endif

// Enable/disable HID class driver debug
#define CFG_TUH_HID_DEBUG 0

//--------------------------------------------------------------------
// USB Host Controller Configuration
//--------------------------------------------------------------------

// Enable Host mode
#define CFG_TUH_ENABLED 1

// Host controller debug
#define CFG_TUH_DEBUG 0

// HCD (Host Controller Driver) debug
#define CFG_TUH_HCD_DEBUG 0

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */