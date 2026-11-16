#pragma once

#include <stdint.h>

// Display driver for the round 480x480 ST7701S panel on the Waveshare
// ESP32-S3-Touch-LCD-2.1, wired up to LVGL.

constexpr int kDisplayWidth = 480;
constexpr int kDisplayHeight = 480;

// Initialises I2C, the I/O expander, the panel, the backlight and LVGL.
// Returns false if the panel or the LVGL draw buffers could not be set up.
bool displayInit();

// Sets the backlight brightness, 0 (off) to 100 (full).
void displaySetBacklight(uint8_t percent);

// Runs LVGL timers and redraws. Call this regularly from loop().
void displayUpdate();
