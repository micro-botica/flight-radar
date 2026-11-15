#pragma once

// Contract between the data side (ADS-B client and parser) and the UI side
// (radar, detail card, list). The data side fills the raw fields and calls
// aircraftDerive(). The UI only reads.
//
// Units are SI-friendly and fixed here, so the UI converts for display:
//   altitude: feet, vertical rate: feet per minute, ground speed: knots,
//   track: degrees true (0 to 359), distance: kilometers, bearing: degrees.

#include <math.h>
#include <stdint.h>

// Below this vertical rate (ft/min, absolute) an aircraft counts as level.
constexpr int16_t AIRCRAFT_LEVEL_THRESHOLD_FPM = 100;

enum class VerticalTrend : uint8_t {
  Level,
  Climbing,
  Descending,
};

struct Aircraft {
  // Identity. Empty string when unknown.
  char icao[7];          // 24-bit hex address, e.g. "484A12"
  char callsign[9];      // up to 8 characters
  char registration[11]; // e.g. "PH-BXA"
  char type[5];          // ICAO type code, e.g. "B738"
  char airline[32];      // operator name, empty when unknown

  // Position and movement, from the data source.
  float lat;             // degrees
  float lon;             // degrees
  int32_t altitudeFt;    // barometric altitude, feet
  int16_t verticalRateFpm; // feet per minute, positive is climbing
  int16_t groundSpeedKt; // knots
  int16_t trackDeg;      // heading over ground, 0 to 359

  bool hasPosition;      // false when lat/lon are not valid
  bool onGround;

  // Derived for the UI, relative to the active profile location.
  float distanceKm;      // great-circle distance
  int16_t bearingDeg;    // direction from the profile to the aircraft, 0 to 359
  VerticalTrend trend;
};

constexpr float AIRCRAFT_EARTH_RADIUS_KM = 6371.0f;

// Fills distanceKm, bearingDeg and trend from the raw fields.
// profileLat and profileLon are the active profile location in degrees.
inline void aircraftDerive(Aircraft &a, float profileLat, float profileLon) {
  if (a.verticalRateFpm > AIRCRAFT_LEVEL_THRESHOLD_FPM) {
    a.trend = VerticalTrend::Climbing;
  } else if (a.verticalRateFpm < -AIRCRAFT_LEVEL_THRESHOLD_FPM) {
    a.trend = VerticalTrend::Descending;
  } else {
    a.trend = VerticalTrend::Level;
  }

  if (!a.hasPosition) {
    a.distanceKm = 0.0f;
    a.bearingDeg = 0;
    return;
  }

  const float rad = (float)M_PI / 180.0f;
  const float phi1 = profileLat * rad;
  const float phi2 = a.lat * rad;
  const float dPhi = (a.lat - profileLat) * rad;
  const float dLambda = (a.lon - profileLon) * rad;

  // Haversine distance.
  const float s1 = sinf(dPhi / 2.0f);
  const float s2 = sinf(dLambda / 2.0f);
  const float h = s1 * s1 + cosf(phi1) * cosf(phi2) * s2 * s2;
  a.distanceKm = 2.0f * AIRCRAFT_EARTH_RADIUS_KM * asinf(sqrtf(h));

  // Initial bearing from the profile to the aircraft.
  const float y = sinf(dLambda) * cosf(phi2);
  const float x = cosf(phi1) * sinf(phi2) - sinf(phi1) * cosf(phi2) * cosf(dLambda);
  float deg = atan2f(y, x) / rad;
  if (deg < 0.0f) deg += 360.0f;
  a.bearingDeg = (int16_t)deg % 360;
}
