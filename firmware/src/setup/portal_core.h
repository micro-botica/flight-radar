#pragma once

// Pure logic of the first-time setup portal (no hardware, runs on a PC).
// The device wraps it in SetupPortal (setup_portal.cpp), the native tests
// call it directly.

#include <stdint.h>

#include <algorithm>
#include <string>
#include <vector>

struct PortalNetwork {
  std::string ssid;
  int rssi = -100;
};

// What the user entered on the phone.
struct SetupResult {
  std::string ssid;
  std::string password;
  std::string postcode;
  std::string profileName;
};

namespace portal {

// "Radar-XXXX" with the last two bytes of the MAC address.
inline std::string apName(const uint8_t mac[6]) {
  static const char* kHex = "0123456789ABCDEF";
  std::string name = "Radar-";
  for (int i = 4; i < 6; i++) {
    name += kHex[mac[i] >> 4];
    name += kHex[mac[i] & 0x0F];
  }
  return name;
}

// Escapes a value for the WIFI: QR payload (\ ; , : " are special).
inline std::string escapeWifiField(const std::string& in) {
  std::string out;
  for (char c : in) {
    if (c == '\\' || c == ';' || c == ',' || c == ':' || c == '"') out += '\\';
    out += c;
  }
  return out;
}

// Payload of the QR code that makes a phone join our open access point.
inline std::string wifiQrPayload(const std::string& ssid) {
  return "WIFI:T:nopass;S:" + escapeWifiField(ssid) + ";;";
}

// Signal bars 0..4 for the scan list.
inline int signalBars(int rssi) {
  if (rssi >= -55) return 4;
  if (rssi >= -65) return 3;
  if (rssi >= -75) return 2;
  if (rssi >= -85) return 1;
  return 0;
}

// Removes hidden and duplicate networks (keeping the strongest) and sorts
// by signal strength, strongest first.
inline std::vector<PortalNetwork> cleanScan(std::vector<PortalNetwork> in) {
  std::vector<PortalNetwork> out;
  for (const auto& n : in) {
    if (n.ssid.empty()) continue;
    auto it = std::find_if(out.begin(), out.end(), [&](const PortalNetwork& o) {
      return o.ssid == n.ssid;
    });
    if (it == out.end()) {
      out.push_back(n);
    } else if (n.rssi > it->rssi) {
      it->rssi = n.rssi;
    }
  }
  std::stable_sort(out.begin(), out.end(),
                   [](const PortalNetwork& a, const PortalNetwork& b) {
                     return a.rssi > b.rssi;
                   });
  return out;
}

inline std::string trim(const std::string& s) {
  size_t b = s.find_first_not_of(" \t\r\n");
  if (b == std::string::npos) return "";
  size_t e = s.find_last_not_of(" \t\r\n");
  return s.substr(b, e - b + 1);
}

// Dutch postcode: 4 digits (not starting with 0), optional space, 2 letters.
// Returns the normalised form "1234 AB", or "" when invalid.
inline std::string normalizePostcode(const std::string& raw) {
  std::string s;
  for (char c : raw) {
    if (c != ' ') s += c;
  }
  if (s.size() != 6) return "";
  if (s[0] < '1' || s[0] > '9') return "";
  for (int i = 1; i < 4; i++) {
    if (s[i] < '0' || s[i] > '9') return "";
  }
  std::string letters;
  for (int i = 4; i < 6; i++) {
    char c = s[i];
    if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
    if (c < 'A' || c > 'Z') return "";
    letters += c;
  }
  return s.substr(0, 4) + " " + letters;
}

// Validates the submitted form. Returns "" when fine, else a short error
// message for the user. On success `out` holds the cleaned values.
inline std::string validate(const SetupResult& in, SetupResult& out) {
  out = in;
  out.ssid = trim(in.ssid);
  out.profileName = trim(in.profileName);
  if (out.ssid.empty() || out.ssid.size() > 32) return "Choose a Wi-Fi network.";
  if (!out.password.empty() &&
      (out.password.size() < 8 || out.password.size() > 63)) {
    return "The password must be 8 to 63 characters.";
  }
  out.postcode = normalizePostcode(in.postcode);
  if (out.postcode.empty()) return "Enter a valid postcode, like 1234 AB.";
  if (out.profileName.empty()) out.profileName = "Home";
  if (out.profileName.size() > 24) return "The profile name is too long.";
  return "";
}

}  // namespace portal
