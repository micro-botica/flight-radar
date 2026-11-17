#pragma once

// UI theme from docs/design/stijlgids.md ("Stijlgids Modern"). Colors, fonts
// and sizes live here so every screen looks the same.
//
// Colors are plain 0xRRGGBB values. lv_color_hex() is not constexpr in LVGL 8,
// so use theme::color(theme::kText) where an lv_color_t is needed.

#include <lvgl.h>

#include <stdint.h>

LV_FONT_DECLARE(font_space_grotesk_600_116);
LV_FONT_DECLARE(font_space_grotesk_600_96);
LV_FONT_DECLARE(font_space_grotesk_600_46);
LV_FONT_DECLARE(font_space_grotesk_600_26);
LV_FONT_DECLARE(font_space_grotesk_600_22);
LV_FONT_DECLARE(font_space_grotesk_600_18);
LV_FONT_DECLARE(font_space_grotesk_500_17);
LV_FONT_DECLARE(font_inter_600_20);
LV_FONT_DECLARE(font_inter_600_18);
LV_FONT_DECLARE(font_inter_600_15);
LV_FONT_DECLARE(font_inter_600_13);
LV_FONT_DECLARE(font_inter_500_21);
LV_FONT_DECLARE(font_inter_500_15);
LV_FONT_DECLARE(font_inter_500_13);

namespace theme {

inline lv_color_t color(uint32_t rgb) { return lv_color_hex(rgb); }

// Day colors.
constexpr uint32_t kBackground = 0x0A0E13;   // Background of all screens
constexpr uint32_t kSurface = 0x121922;      // Cards, panels, secondary buttons
constexpr uint32_t kSurface2 = 0x1A2330;     // Badges, list dividers, progress track
constexpr uint32_t kLine = 0x1F2A36;         // Inner range rings, round screen edge
constexpr uint32_t kLineStrong = 0x2E3B4A;   // Outer ring (40 km), button borders
constexpr uint32_t kText = 0xE8EEF5;         // Default text
constexpr uint32_t kTextWhite = 0xFFFFFF;    // Big numbers, titles, active page dot
constexpr uint32_t kText2 = 0xC9D3DE;        // Subtitles, date, corner info
constexpr uint32_t kMuted = 0x7C8B9C;        // Labels, ring labels, help text
constexpr uint32_t kInactive = 0x3A4756;     // Inactive page dots
constexpr uint32_t kAccent = 0x4CC9F0;       // Sweep, north arrow, home ring, primary buttons
constexpr uint32_t kAccentSurface = 0x16303B;  // "Active" badge, selected chips
constexpr uint32_t kWarning = 0xFFC94A;      // Attention (estimated location)
constexpr uint32_t kWarningSurface = 0x2A2412;
constexpr uint32_t kError = 0xFF6B6B;        // No Wi-Fi, delete
constexpr uint32_t kErrorSurface = 0x3A1A1D;
constexpr uint32_t kSuccess = 0x7BE07B;      // Connected, postcode found
constexpr uint32_t kSuccessSurface = 0x10241C;
constexpr uint32_t kTextOnAccent = 0x0A0E13;  // Text on accent and error buttons

// Altitude colors of the aircraft icon, index 0 to 4 (see altitudeColor()).
constexpr uint32_t kAltitudeDay[5] = {0xFF8A3D, 0xFFC94A, 0x7BE07B, 0x3FC8FF, 0xA99BFF};
constexpr uint32_t kAltitudeNight[5] = {0x8A5130, 0x8A7436, 0x467A49, 0x2C6B85, 0x5F5896};

// Night mode colors.
constexpr uint32_t kNightBackground = 0x000000;
constexpr uint32_t kNightRingInner = 0x11161C;
constexpr uint32_t kNightRingOuter = 0x171E26;
constexpr uint32_t kNightText = 0x8E97A3;   // Callsign, time
constexpr uint32_t kNightMuted = 0x6E7785;  // Ring labels, N
constexpr uint32_t kNightSweepLine = 0x1D3A4A;
constexpr uint32_t kNightDotActive = 0x4A5462;
constexpr uint32_t kNightDotInactive = 0x1F262E;

// Maps an altitude in meters to the 0 to 4 index of the altitude colors:
// below 1 km, 1 to 3 km, 3 to 6 km, 6 to 9 km, above 9 km.
constexpr int altitudeBand(int altitudeMeters) {
  return altitudeMeters < 1000 ? 0
         : altitudeMeters < 3000 ? 1
         : altitudeMeters < 6000 ? 2
         : altitudeMeters < 9000 ? 3
                                 : 4;
}

inline lv_color_t altitudeColor(int altitudeMeters, bool night) {
  const int band = altitudeBand(altitudeMeters);
  return color(night ? kAltitudeNight[band] : kAltitudeDay[band]);
}

// Fonts. Space Grotesk for digits and titles, Inter for other text. The
// 116 and 96 px fonts only contain "0-9 : degree - space". Smallest text is
// 13 px.
inline const lv_font_t* const kFontClock = &font_space_grotesk_600_116;   // Digits and ':'
inline const lv_font_t* const kFontTemperature = &font_space_grotesk_600_96;  // Digits, degree, '-'
inline const lv_font_t* const kFontCallsignLarge = &font_space_grotesk_600_46;  // Detail card
inline const lv_font_t* const kFontTitle = &font_space_grotesk_600_26;    // Status titles, values
inline const lv_font_t* const kFontScreenTitle = &font_space_grotesk_600_22;
inline const lv_font_t* const kFontNorth = &font_space_grotesk_600_18;
inline const lv_font_t* const kFontCorner = &font_space_grotesk_500_17;   // Count and time

inline const lv_font_t* const kFontValueLarge = &font_inter_600_20;
inline const lv_font_t* const kFontCallsignList = &font_inter_600_18;
inline const lv_font_t* const kFontCallsignRadar = &font_inter_600_15;
inline const lv_font_t* const kFontBadge = &font_inter_600_13;
inline const lv_font_t* const kFontDate = &font_inter_500_21;
inline const lv_font_t* const kFontBody = &font_inter_500_15;
inline const lv_font_t* const kFontRingLabel = &font_inter_500_13;

// Screen geometry, round 480 x 480 panel with the center at (240, 240).
constexpr int kScreenSize = 480;
constexpr int kCenter = 240;
constexpr int kContentRadius = 200;  // All content stays inside this radius
constexpr int kEdgeRadius = 240;     // Outer edge of the screen

// The band between kContentRadius and kEdgeRadius is only for: the N
// indicator (top), aircraft count (top left), time (top right) and the page
// dots (bottom).
constexpr int kMinTouchSize = 48;  // Touch targets are at least 48 x 48 px

constexpr int kRingWidthPx = 2;  // Style guide says 1.5 px, LVGL line widths are whole pixels
constexpr int kCardRadius = 18;
constexpr int kPanelRadius = 20;    // Panels are 20 to 22
constexpr int kButtonHeight = 48;
constexpr int kStatusButtonHeight = 52;
constexpr int kButtonRadius = kButtonHeight / 2;  // Pill shape

// Page dots: three 48 x 48 touch targets side by side, top left at (168, 428).
constexpr int kPageDotsX = 168;
constexpr int kPageDotsY = 428;
constexpr int kPageDotActiveWidth = 20;  // Pill, white
constexpr int kPageDotHeight = 8;
constexpr int kPageDotRadius = 4;

// Radar screen.
constexpr int kRingRadiusStep = 50;  // 10 km per ring at the 40 km range (5 px per km)
constexpr int kRingCount = 4;
constexpr int kRadarSweepSeconds = 6;
constexpr int kRadarSweepSecondsNight = 14;
constexpr int kAircraftIconSize = 26;       // 22 without label
constexpr int kAircraftIconSizeNoLabel = 22;
constexpr int kRadarLabelCount = 6;         // Nearest aircraft that get a label
constexpr int kRadarLabelCountNight = 3;

}  // namespace theme
