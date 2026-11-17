#pragma once

// Location profile: one place (for example home or office) with the Wi-Fi
// network that belongs to it and the area the radar shows.

#include <Arduino.h>

#include <vector>

constexpr size_t PROFILE_MAX_COUNT = 8;
constexpr uint8_t PROFILE_RANGES_KM[] = {20, 40, 80};
constexpr uint8_t PROFILE_DEFAULT_RANGE_KM = 40;

struct Profile {
  String name;       // shown on the clock screen, e.g. "Home"
  String ssid;       // Wi-Fi network that belongs to this place
  String password;   // password of that network
  String postcode;   // used to look up the coordinates
  float lat = 0.0f;  // degrees
  float lon = 0.0f;  // degrees
  uint8_t rangeKm = PROFILE_DEFAULT_RANGE_KM;  // 20, 40 or 80
};

// True when rangeKm is one of the supported ranges.
inline bool profileRangeValid(uint8_t rangeKm) {
  for (uint8_t r : PROFILE_RANGES_KM) {
    if (r == rangeKm) return true;
  }
  return false;
}

// Index of the first profile whose Wi-Fi network is ssid, or -1.
inline int profileIndexForSsid(const std::vector<Profile>& profiles,
                               const String& ssid) {
  if (ssid.length() == 0) return -1;
  for (size_t i = 0; i < profiles.size(); i++) {
    if (profiles[i].ssid == ssid) return (int)i;
  }
  return -1;
}
