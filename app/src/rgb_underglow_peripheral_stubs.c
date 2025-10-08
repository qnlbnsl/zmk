/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/endpoints_types.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>
#include <zmk/hid_indicators_types.h>
#include <zmk/hid_indicators.h>
#include <zmk/ble.h>
#include <zmk/split/bluetooth/service.h>
#include <zmk/split/bluetooth/peripheral_layers.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Stub implementations for RGB underglow status indicators on peripheral devices
// These functions provide safe defaults for status indicators on peripherals
// Peripherals receive actual layer state and HID indicators from the central device via BLE events

// External variables that need to be declared for peripheral HID indicators
#if IS_ENABLED(CONFIG_ZMK_SPLIT_PERIPHERAL_HID_INDICATORS)
extern zmk_hid_indicators_t peripheral_hid_indicators;
#endif

// Note: peripheral layer state functions (set_peripheral_layers_state, peripheral_layer_active, 
// peripheral_highest_layer_active) are already implemented in peripheral_layers.c
// This stub file only provides stubs for functions that don't exist elsewhere

#if IS_ENABLED(CONFIG_ZMK_SPLIT) && !IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

// Stub for endpoints functionality
// Peripherals don't manage endpoints - they just connect to the central
struct zmk_endpoint_instance zmk_endpoints_selected(void) {
    // Return a default BLE endpoint for peripheral devices
    // This is used by RGB status indicators to show connection type
    return (struct zmk_endpoint_instance){
        .transport = ZMK_TRANSPORT_BLE,
        .ble = {.profile_index = 0}
    };
}

bool zmk_endpoints_preferred_transport_is_active(void) {
    // Peripherals always use their configured transport (BLE to central)
    // Return true to indicate the transport is active
    return true;
}

// Stub for HID indicators functionality
// Peripherals receive HID indicator state from central via BLE
#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
zmk_hid_indicators_t zmk_hid_indicators_get_current_profile(void) {
#if IS_ENABLED(CONFIG_ZMK_SPLIT_PERIPHERAL_HID_INDICATORS)
    // Return the real HID indicator state received from central
    return peripheral_hid_indicators;
#else
    // If peripheral HID indicators not enabled, return no indicators
    return 0;
#endif
}
#endif

// Stub for BLE functionality
// Peripherals connect to the central, not to the host
#if IS_ENABLED(CONFIG_ZMK_BLE)
int zmk_ble_active_profile_index(void) {
    // Peripherals always use profile 0 to connect to central
    return 0;
}

int8_t zmk_ble_profile_status(uint8_t index) {
    // Return connected for profile 0, disconnected for others
    // 0 = disconnected, 1 = paired, 2 = connected
    return (index == 0) ? 2 : 0;
}
#endif

#endif // IS_ENABLED(CONFIG_ZMK_SPLIT) && !IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
