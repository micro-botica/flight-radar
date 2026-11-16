#pragma once

#ifdef ARDUINO

#include <Arduino.h>
#include <WiFi.h>

#include "wifi_manager.h"

// WifiDriver and WifiClock backed by the Arduino ESP32 core.
class ArduinoWifiDriver : public WifiDriver {
 public:
  void init() override {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);  // WifiManager handles reconnecting
  }
  void disconnect() override { WiFi.disconnect(); }
  void startScan() override { WiFi.scanNetworks(true /* async */); }
  int scanComplete() override {
    int n = WiFi.scanComplete();
    if (n == WIFI_SCAN_RUNNING) return kScanRunning;
    if (n == WIFI_SCAN_FAILED) return kScanFailed;
    return n;
  }
  std::string scanSsid(int index) override { return WiFi.SSID(index).c_str(); }
  int scanRssi(int index) override { return WiFi.RSSI(index); }
  void scanDelete() override { WiFi.scanDelete(); }
  void connect(const std::string& ssid, const std::string& password) override {
    WiFi.begin(ssid.c_str(), password.c_str());
  }
  bool isConnected() override { return WiFi.status() == WL_CONNECTED; }
  int rssi() override { return WiFi.RSSI(); }
  std::string localIp() override { return WiFi.localIP().toString().c_str(); }
  void log(const std::string& message) override {
    Serial.println(message.c_str());
  }
};

class ArduinoWifiClock : public WifiClock {
 public:
  uint32_t millis() override { return ::millis(); }
};

#endif  // ARDUINO
