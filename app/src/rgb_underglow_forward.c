/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * RGB underglow forwarding for a split central without its own LED strip (e.g. a dongle).
 *
 * &rgb_ug is a global behavior: the central converts relative commands (toggle, brightness up,
 * next effect, ...) to absolute ones using its own underglow state, runs them locally and sends
 * them to every peripheral. Without CONFIG_ZMK_RGB_UNDERGLOW the central has no &rgb_ug driver,
 * so the binding is dropped before it is forwarded and the peripherals' LEDs can never be
 * controlled. This file provides a shadow state so the behavior driver can be built on such a
 * central: it tracks what the peripherals were told and drives no LEDs itself.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zmk/rgb_underglow.h>

#define HUE_MAX 360
#define SAT_MAX 100

static struct {
    bool on;
    int effect;
    struct zmk_led_hsb color;
} shadow = {
    .on = IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW_FORWARD_ON_START),
    .effect = CONFIG_ZMK_RGB_UNDERGLOW_FORWARD_EFF_START,
    .color = {.h = CONFIG_ZMK_RGB_UNDERGLOW_FORWARD_HUE_START,
              .s = CONFIG_ZMK_RGB_UNDERGLOW_FORWARD_SAT_START,
              .b = CONFIG_ZMK_RGB_UNDERGLOW_FORWARD_BRT_START},
};

int zmk_rgb_underglow_get_state(bool *on_off) {
    *on_off = shadow.on;
    return 0;
}

int zmk_rgb_underglow_on(void) {
    shadow.on = true;
    return 0;
}

int zmk_rgb_underglow_off(void) {
    shadow.on = false;
    return 0;
}

int zmk_rgb_underglow_toggle(void) {
    shadow.on = !shadow.on;
    return 0;
}

int zmk_rgb_underglow_transient_on(void) { return 0; }
int zmk_rgb_underglow_transient_off(void) { return 0; }

int zmk_rgb_underglow_calc_effect(int direction) {
    int n = CONFIG_ZMK_RGB_UNDERGLOW_FORWARD_EFF_COUNT;
    return (shadow.effect + n + direction) % n;
}

int zmk_rgb_underglow_select_effect(int effect) {
    if (effect < 0 || effect >= CONFIG_ZMK_RGB_UNDERGLOW_FORWARD_EFF_COUNT) {
        return -EINVAL;
    }
    shadow.effect = effect;
    return 0;
}

int zmk_rgb_underglow_cycle_effect(int direction) {
    return zmk_rgb_underglow_select_effect(zmk_rgb_underglow_calc_effect(direction));
}

struct zmk_led_hsb zmk_rgb_underglow_calc_hue(int direction) {
    struct zmk_led_hsb color = shadow.color;
    color.h = (color.h + HUE_MAX + direction * CONFIG_ZMK_RGB_UNDERGLOW_FORWARD_HUE_STEP) % HUE_MAX;
    return color;
}

struct zmk_led_hsb zmk_rgb_underglow_calc_sat(int direction) {
    struct zmk_led_hsb color = shadow.color;
    color.s = CLAMP(color.s + direction * CONFIG_ZMK_RGB_UNDERGLOW_FORWARD_SAT_STEP, 0, SAT_MAX);
    return color;
}

struct zmk_led_hsb zmk_rgb_underglow_calc_brt(int direction) {
    struct zmk_led_hsb color = shadow.color;
    color.b = CLAMP(color.b + direction * CONFIG_ZMK_RGB_UNDERGLOW_FORWARD_BRT_STEP, 0, 100);
    return color;
}

int zmk_rgb_underglow_set_hsb(struct zmk_led_hsb color) {
    if (color.h >= HUE_MAX || color.s > SAT_MAX || color.b > 100) {
        return -ENOTSUP;
    }
    shadow.color = color;
    return 0;
}

int zmk_rgb_underglow_change_hue(int direction) {
    return zmk_rgb_underglow_set_hsb(zmk_rgb_underglow_calc_hue(direction));
}

int zmk_rgb_underglow_change_sat(int direction) {
    return zmk_rgb_underglow_set_hsb(zmk_rgb_underglow_calc_sat(direction));
}

int zmk_rgb_underglow_change_brt(int direction) {
    return zmk_rgb_underglow_set_hsb(zmk_rgb_underglow_calc_brt(direction));
}

int zmk_rgb_underglow_change_spd(int direction) { return 0; }

int zmk_rgb_underglow_status(void) { return 0; }
