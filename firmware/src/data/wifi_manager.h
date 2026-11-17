#pragma once

#include <stdint.h>

#include <string>
#include <vector>

// Hardware access used by WifiManager. The device uses ArduinoWifiDriver,
// the native tests use a fake, so the logic can run on a PC.
class WifiDriver {
 public:
  static constexpr int kScanRunning = -1;
  static constexpr int kScanFailed = -2;

  virtual ~WifiDriver() = default;
  virtual void init() = 0;  // station mode, no automatic reconnect
  virtual void disconnect() = 0;
  virtual void startScan() = 0;  // asynchronous
  virtual int scanComplete() = 0;  // count, kScanRunning or kScanFailed
  virtual std::string scanSsid(int index) = 0;
  virtual int scanRssi(int index) = 0;
  virtual void scanDelete() = 0;
  virtual void connect(const std::string& ssid, const std::string& password) = 0;
  virtual bool isConnected() = 0;
  virtual int rssi() = 0;  // of the current connection
  virtual std::string localIp() = 0;
  virtual void log(const std::string& message) = 0;
};

class WifiClock {
 public:
  virtual ~WifiClock() = default;
  virtual uint32_t millis() = 0;
};

// A known Wi-Fi network, taken from a location profile.
struct WifiNetwork {
  std::string ssid;
  std::string password;
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
  std::string ssid;          // network we are connected or connecting to
  int8_t rssi = 0;           // dBm, only valid when Connected
  uint32_t reconnects = 0;   // number of reconnect attempts since start
};

class WifiManager {
 public:
  static constexpr uint32_t kConnectTimeoutMs = 15000;
  static constexpr uint32_t kMinRetryMs = 2000;
  static constexpr uint32_t kMaxRetryMs = 60000;

  WifiManager(WifiDriver& driver, WifiClock& clock)
      : driver_(driver), clock_(clock) {}

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
  const WifiNetwork* strongestKnown(int count) const;

  WifiDriver& driver_;
  WifiClock& clock_;
  std::vector<WifiNetwork> networks_;
  WifiStatus status_;
  uint32_t stateSince_ = 0;
  uint32_t retryDelayMs_ = kMinRetryMs;
};
