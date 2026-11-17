#pragma once

// UI theme from docs/design/stijlgids.md ("Stijlgids Modern", version 2).
// Colors, fonts and sizes live here so every screen looks the same.
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
LV_FONT_DECLARE(font_space_grotesk_600_20);
LV_FONT_DECLARE(font_space_grotesk_600_18);
LV_FONT_DECLARE(font_space_grotesk_500_20);
LV_FONT_DECLARE(font_inter_600_20);
LV_FONT_DECLARE(font_inter_600_18);
LV_FONT_DECLARE(font_inter_600_16);
LV_FONT_DECLARE(font_inter_600_15);
LV_FONT_DECLARE(font_inter_500_21);

namespace theme {

inline lv_color_t color(uint32_t rgb) { return lv_color_hex(rgb); }

// Day colors.
constexpr uint32_t kBackground = 0x0A0E13;     // Background of all screens
constexpr uint32_t kSurface = 0x121922;        // Cards, panels, round buttons
constexpr uint32_t kSurface2 = 0x1A2330;       // Badges, list dividers, progress track
constexpr uint32_t kLine = 0x1F2A36;           // Track of scroll bar and brightness bar
constexpr uint32_t kLineStrong = 0x2E3B4A;     // All range rings, button borders
constexpr uint32_t kText = 0xE8EEF5;           // Default text
constexpr uint32_t kTextWhite = 0xFFFFFF;      // Big numbers, titles, active page dot
constexpr uint32_t kText2 = 0xC9D3DE;          // Subtitles, date, time and count on the radar
constexpr uint32_t kMuted = 0x7C8B9C;          // Labels, ring labels, help text
constexpr uint32_t kInactive = 0x3A4756;       // Inactive page dots
constexpr uint32_t kAccent = 0x4CC9F0;         // Sweep, north arrow, home ring, primary buttons
constexpr uint32_t kAccentSurface = 0x16303B;  // Selected location profile
constexpr uint32_t kWarning = 0xFFC94A;        // Attention (estimated location)
constexpr uint32_t kWarningSurface = 0x2A2412;  // Background of the warning pill
constexpr uint32_t kError = 0xFF6B6B;          // No Wi-Fi, delete
constexpr uint32_t kSuccess = 0x7BE07B;        // Connected, postcode found
constexpr uint32_t kTextOnAccent = 0x0A0E13;   // Text on an accent button

// Altitude colors of the aircraft icon, index 0 to 4 (see altitudeColor()).
constexpr uint32_t kAltitudeDay[5] = {0xFF8A3D, 0xFFC94A, 0x7BE07B, 0x3FC8FF, 0xA99BFF};
constexpr uint32_t kAltitudeNight[5] = {0x8A5130, 0x8A7436, 0x467A49, 0x2C6B85, 0x5F5896};

// Night mode colors.
constexpr uint32_t kNightBackground = 0x000000;
constexpr uint32_t kNightRing = 0x171E26;   // All rings
constexpr uint32_t kNightText = 0x8E97A3;   // Callsign, time
constexpr uint32_t kNightMuted = 0x6E7785;  // Ring labels, N, moon
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

// Fonts. Space Grotesk for digits and titles, Inter for other text. Smallest
// text is 15 px and small text is always Inter 600; Inter 500 is only used for
// the date on the clock. The 116 and 96 px fonts only contain "0-9 : degree -".
// Sizes in the design that are not listed here (28, 24, 21, 19, 17) use the
// nearest size below.
inline const lv_font_t* const kFontClock = &font_space_grotesk_600_116;        // Digits and ':'
inline const lv_font_t* const kFontTemperature = &font_space_grotesk_600_96;   // Digits, degree, '-'
inline const lv_font_t* const kFontCallsignLarge = &font_space_grotesk_600_46;  // Detail card
inline const lv_font_t* const kFontTitle = &font_space_grotesk_600_26;         // Status titles, detail values
inline const lv_font_t* const kFontScreenTitle = &font_space_grotesk_600_22;   // "Instellingen", "In de buurt"
inline const lv_font_t* const kFontDistance = &font_space_grotesk_600_20;      // Distance in the list
inline const lv_font_t* const kFontNorth = &font_space_grotesk_600_18;         // N indicator
inline const lv_font_t* const kFontCorner = &font_space_grotesk_500_20;        // Time and count on the radar

inline const lv_font_t* const kFontValue = &font_inter_600_20;     // Values on the weather screen
inline const lv_font_t* const kFontCallsign = &font_inter_600_18;  // Callsigns (radar and list), airline, profile name
inline const lv_font_t* const kFontButton = &font_inter_600_16;    // Buttons
inline const lv_font_t* const kFontSmall = &font_inter_600_15;     // Labels, ring labels, subtitles, help text
inline const lv_font_t* const kFontDate = &font_inter_500_21;      // Date on the clock

// Screen geometry, round 480 x 480 panel with the center at (240, 240).
constexpr int kScreenSize = 480;
constexpr int kCenter = 240;
constexpr int kContentRadius = 180;  // The radar stays inside this radius
constexpr int kEdgeRadius = 240;     // Outer edge of the screen
constexpr int kEdgeMargin = 10;      // Minimum distance to the round edge

// The band between kContentRadius and kEdgeRadius is only for: the N
// indicator (top), aircraft count (top left), time (top right) and the page
// dots (bottom).
constexpr int kMinTouchSize = 48;  // Touch targets are at least 48 x 48 px

// Line widths are whole pixels (LVGL has no 1.5 px lines).
constexpr int kRingWidthInner = 1;  // Rings 1 to 3
constexpr int kRingWidthOuter = 2;  // Ring 4
constexpr int kSweepLineWidth = 2;
constexpr int kCardRadius = 18;
constexpr int kPanelRadius = 20;  // Panels are 20 to 22
constexpr int kButtonHeight = 48;
constexpr int kButtonRadius = kButtonHeight / 2;  // Pill shape

// Page dots: three 48 x 48 touch targets side by side, top left at (168, 428).
constexpr int kPageDotsX = 168;
constexpr int kPageDotsY = 428;
constexpr int kPageDotActiveWidth = 20;  // Pill, white
constexpr int kPageDotHeight = 8;
constexpr int kPageDotRadius = 4;

// Radar screen. Rings at radius 45, 90, 135 and 180 px: 10, 20, 30 and 40 km
// at the 40 km range, so 4.5 px per km.
constexpr int kRingCount = 4;
constexpr int kRingRadius[kRingCount] = {45, 90, 135, 180};
constexpr int kRingLabelRightX = 234;                     // Labels right aligned, just left of the north axis
constexpr int kRingLabelTopY[kRingCount] = {0, 153, 108, 64};  // Ring 1 (10 km) has no label
constexpr int kNorthLabelTopY = 24;
constexpr int kNorthArrowTopY = 50;  // 12 x 9 px triangle in accent
constexpr int kNorthArrowWidth = 12;
constexpr int kNorthArrowHeight = 9;
constexpr int kHomeDotSize = 8;
constexpr int kHomeRingSize = 20;  // Diameter, 2 px wide, in accent
constexpr int kHomeRingWidth = 2;
constexpr int kRadarSweepSeconds = 6;
constexpr int kRadarSweepSecondsNight = 14;
constexpr int kAircraftIconSize = 26;  // With callsign
constexpr int kAircraftIconSizeNoLabel = 22;
constexpr int kCallsignOffsetX = 16;     // From the center of the icon
constexpr int kCallsignFlipX = 320;      // From this x the label goes left of the icon
constexpr int kRadarLabelCount = 4;      // Only the nearest aircraft get a callsign
constexpr int kRadarLabelCountNight = 3;
constexpr int kCornerBoxWidth = 60;   // Aircraft count and time boxes
constexpr int kCornerBoxHeight = 24;
constexpr int kCountBoxX = 65;
constexpr int kTimeBoxX = 355;
constexpr int kCornerBoxY = 83;

}  // namespace theme
