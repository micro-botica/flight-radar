#include "wifi_manager.h"

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
  driver_.init();
  startScan();
}

void WifiManager::startScan() {
  driver_.disconnect();
  driver_.scanDelete();
  driver_.startScan();
  status_.state = WifiState::Scanning;
  status_.ssid = "";
  stateSince_ = clock_.millis();
}

// Pick the known network with the strongest signal from the scan results.
const WifiNetwork* WifiManager::strongestKnown(int count) const {
  const WifiNetwork* best = nullptr;
  int bestRssi = -1000;
  for (int i = 0; i < count; i++) {
    std::string ssid = driver_.scanSsid(i);
    int rssi = driver_.scanRssi(i);
    for (const auto& known : networks_) {
      if (ssid == known.ssid && rssi > bestRssi) {
        bestRssi = rssi;
        best = &known;
      }
    }
  }
  return best;
}

void WifiManager::handleScanResult() {
  int count = driver_.scanComplete();
  if (count == WifiDriver::kScanRunning) return;

  const WifiNetwork* best = count > 0 ? strongestKnown(count) : nullptr;
  driver_.scanDelete();

  if (best) {
    startConnect(*best);
  } else {
    driver_.log("WiFi: no known network found");
    status_.state = WifiState::NoNetwork;
    stateSince_ = clock_.millis();
  }
}

void WifiManager::startConnect(const WifiNetwork& network) {
  driver_.log("WiFi: connecting to " + network.ssid);
  driver_.connect(network.ssid, network.password);
  status_.state = WifiState::Connecting;
  status_.ssid = network.ssid;
  stateSince_ = clock_.millis();
}

void WifiManager::update() {
  uint32_t now = clock_.millis();
  switch (status_.state) {
    case WifiState::Idle:
      break;

    case WifiState::Scanning:
      handleScanResult();
      break;

    case WifiState::Connecting:
      if (driver_.isConnected()) {
        driver_.log("WiFi: connected, IP " + driver_.localIp());
        status_.state = WifiState::Connected;
        status_.rssi = driver_.rssi();
        retryDelayMs_ = kMinRetryMs;
      } else if (now - stateSince_ > kConnectTimeoutMs) {
        driver_.log("WiFi: connect timed out");
        status_.state = WifiState::Disconnected;
        stateSince_ = now;
      }
      break;

    case WifiState::Connected:
      if (!driver_.isConnected()) {
        driver_.log("WiFi: connection lost");
        status_.state = WifiState::Disconnected;
        stateSince_ = now;
      } else {
        status_.rssi = driver_.rssi();
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
