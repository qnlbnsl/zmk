/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zmk/events/sensor_event.h>
#include <zmk/sensors.h>
#include <zmk/hid_indicators_types.h>

#define ZMK_SPLIT_RUN_BEHAVIOR_DEV_LEN 9

struct sensor_event {
    uint8_t sensor_index;

    uint8_t channel_data_size;
    struct zmk_sensor_channel_data channel_data[ZMK_SENSOR_EVENT_MAX_CHANNELS];
} __packed;

struct zmk_split_run_behavior_data {
    uint8_t position;
    uint8_t source;
    uint8_t state;
    uint32_t param1;
    uint32_t param2;
} __packed;

struct zmk_split_run_behavior_payload {
    struct zmk_split_run_behavior_data data;
    char behavior_dev[ZMK_SPLIT_RUN_BEHAVIOR_DEV_LEN];
} __packed;

struct zmk_split_input_event_payload {
    uint8_t type;
    uint16_t code;
    uint32_t value;
    uint8_t sync;
} __packed;

#if IS_ENABLED(CONFIG_ZMK_SPLIT_PERIPHERAL_HID_INDICATORS)
// HID indicators received from central device and stored on peripheral
extern zmk_hid_indicators_t peripheral_hid_indicators;
#endif
