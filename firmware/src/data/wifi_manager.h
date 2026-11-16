#pragma once

#include <Arduino.h>

#include <vector>

// A known Wi-Fi network, taken from a location profile.
struct WifiNetwork {
  String ssid;
  String password;
};

enum class WifiState {
  Idle,          // begin() not called yet
  Scanning,      // looking for known networks
  Connecting,    // join attempt in progress
  Connected,     // got an IP address
  Disconnected,  // lost the connection, waiting to retry
  NoNetwork      // scan found no known network, waiting to rescan
};

// Snapshot of the Wi-Fi status, meant to be read by the UI.
struct WifiStatus {
  WifiState state = WifiState::Idle;
  String ssid;               // network we are connected or connecting to
  int8_t rssi = 0;           // dBm, only valid when Connected
  uint32_t reconnects = 0;   // number of reconnect attempts since start
};

class WifiManager {
 public:
  // Replace the list of known networks (from the location profiles).
  void setNetworks(const std::vector<WifiNetwork>& networks);

  // Start Wi-Fi. If secrets.h defines WIFI_SSID and the network list is
  // empty, that network is used as a development fallback.
  void begin();

  // Call regularly from loop(). Non-blocking.
  void update();

  WifiStatus status() const;
  bool isConnected() const { return status_.state == WifiState::Connected; }

 private:
  void startScan();
  void handleScanResult();
  void startConnect(const WifiNetwork& network);
  const WifiNetwork* strongestKnown(int count, int& bestIndex) const;

  std::vector<WifiNetwork> networks_;
  WifiStatus status_;
  const WifiNetwork* current_ = nullptr;
  uint32_t stateSince_ = 0;
  uint32_t retryDelayMs_ = kMinRetryMs;

  static constexpr uint32_t kConnectTimeoutMs = 15000;
  static constexpr uint32_t kMinRetryMs = 2000;
  static constexpr uint32_t kMaxRetryMs = 60000;
};
