#include "wifi_manager.h"

#include <WiFi.h>

#if __has_include("secrets.h")
#include "secrets.h"
#endif

void WifiManager::setNetworks(const std::vector<WifiNetwork>& networks) {
  networks_ = networks;
}

void WifiManager::begin() {
#if defined(WIFI_SSID)
  // Development fallback: use secrets.h when no profiles are configured.
  if (networks_.empty()) {
    networks_.push_back({WIFI_SSID, WIFI_PASSWORD});
  }
#endif
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(false);  // we handle reconnecting ourselves
  startScan();
}

void WifiManager::startScan() {
  WiFi.disconnect();
  WiFi.scanDelete();
  WiFi.scanNetworks(true /* async */);
  status_.state = WifiState::Scanning;
  status_.ssid = "";
  stateSince_ = millis();
}

// Pick the known network with the strongest signal from the scan results.
const WifiNetwork* WifiManager::strongestKnown(int count, int& bestIndex) const {
  const WifiNetwork* best = nullptr;
  int bestRssi = -1000;
  bestIndex = -1;
  for (int i = 0; i < count; i++) {
    for (const auto& known : networks_) {
      if (WiFi.SSID(i) == known.ssid && WiFi.RSSI(i) > bestRssi) {
        bestRssi = WiFi.RSSI(i);
        best = &known;
        bestIndex = i;
      }
    }
  }
  return best;
}

void WifiManager::handleScanResult() {
  int count = WiFi.scanComplete();
  if (count == WIFI_SCAN_RUNNING) return;

  int bestIndex = -1;
  const WifiNetwork* best =
      count > 0 ? strongestKnown(count, bestIndex) : nullptr;
  WiFi.scanDelete();

  if (best) {
    startConnect(*best);
  } else {
    Serial.println("WiFi: no known network found");
    status_.state = WifiState::NoNetwork;
    stateSince_ = millis();
  }
}

void WifiManager::startConnect(const WifiNetwork& network) {
  Serial.printf("WiFi: connecting to %s\n", network.ssid.c_str());
  current_ = &network;
  WiFi.begin(network.ssid.c_str(), network.password.c_str());
  status_.state = WifiState::Connecting;
  status_.ssid = network.ssid;
  stateSince_ = millis();
}

void WifiManager::update() {
  uint32_t now = millis();
  switch (status_.state) {
    case WifiState::Idle:
      break;

    case WifiState::Scanning:
      handleScanResult();
      break;

    case WifiState::Connecting:
      if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("WiFi: connected, IP %s\n",
                      WiFi.localIP().toString().c_str());
        status_.state = WifiState::Connected;
        retryDelayMs_ = kMinRetryMs;
      } else if (now - stateSince_ > kConnectTimeoutMs) {
        Serial.println("WiFi: connect timed out");
        status_.state = WifiState::Disconnected;
        stateSince_ = now;
      }
      break;

    case WifiState::Connected:
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi: connection lost");
        status_.state = WifiState::Disconnected;
        stateSince_ = now;
      } else {
        status_.rssi = WiFi.RSSI();
      }
      break;

    case WifiState::Disconnected:
    case WifiState::NoNetwork:
      // Retry with exponential backoff. Rescanning also covers the case
      // where a stronger or different known network is now in range.
      if (now - stateSince_ >= retryDelayMs_) {
        retryDelayMs_ = retryDelayMs_ * 2 > kMaxRetryMs ? kMaxRetryMs : retryDelayMs_ * 2;
        status_.reconnects++;
        startScan();
      }
      break;
  }
}

WifiStatus WifiManager::status() const { return status_; }
