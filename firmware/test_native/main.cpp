// Host-side tests for WifiManager using a fake driver and clock.
#include <stdio.h>

#include "../src/data/wifi_manager.h"
#include "../src/setup/portal_core.h"
#include "../src/setup/portal_pages.h"

static int failures = 0;
#define CHECK(cond)                                                  \
  do {                                                               \
    if (!(cond)) {                                                   \
      printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
      failures++;                                                    \
    }                                                                \
  } while (0)

struct FakeClock : WifiClock {
  uint32_t now = 0;
  uint32_t millis() override { return now; }
};

struct ScanEntry {
  std::string ssid;
  int rssi;
};

struct FakeDriver : WifiDriver {
  std::vector<ScanEntry> air;  // networks in range
  bool scanRunning = false;
  bool connected = false;
  bool autoJoin = true;  // joining a network in range succeeds at once
  int currentRssi = -50;
  int scansStarted = 0;
  std::string lastSsid, lastPassword;

  void init() override {}
  void disconnect() override { connected = false; }
  void startScan() override { scansStarted++; }
  int scanComplete() override {
    return scanRunning ? kScanRunning : (int)air.size();
  }
  std::string scanSsid(int i) override { return air[i].ssid; }
  int scanRssi(int i) override { return air[i].rssi; }
  void scanDelete() override {}
  void connect(const std::string& ssid, const std::string& pw) override {
    lastSsid = ssid;
    lastPassword = pw;
    if (autoJoin) connected = true;
  }
  bool isConnected() override { return connected; }
  int rssi() override { return currentRssi; }
  std::string localIp() override { return "192.168.1.50"; }
  void log(const std::string&) override {}
};

struct Fixture {
  FakeDriver driver;
  FakeClock clock;
  WifiManager wifi{driver, clock};
};

static void testConnectsToStrongestKnown() {
  puts("connects to the strongest known network");
  Fixture f;
  f.driver.air = {{"home", -70}, {"office", -40}, {"stranger", -20}};
  f.wifi.setNetworks({{"home", "hp"}, {"office", "op"}});
  f.wifi.begin();
  f.wifi.update();  // scan result -> connect
  CHECK(f.driver.lastSsid == "office");
  CHECK(f.driver.lastPassword == "op");
  f.wifi.update();  // connected
  WifiStatus s = f.wifi.status();
  CHECK(s.state == WifiState::Connected);
  CHECK(s.ssid == "office");
  CHECK(s.rssi == -50);
  CHECK(s.reconnects == 0);
  CHECK(f.wifi.isConnected());
}

static void testWaitsForScan() {
  puts("waits while the scan is running");
  Fixture f;
  f.driver.air = {{"home", -60}};
  f.driver.scanRunning = true;
  f.wifi.setNetworks({{"home", "hp"}});
  f.wifi.begin();
  f.wifi.update();
  CHECK(f.wifi.status().state == WifiState::Scanning);
  f.driver.scanRunning = false;
  f.wifi.update();
  CHECK(f.wifi.status().state == WifiState::Connecting);
}

static void testBackoffDoublesToMax() {
  puts("backoff goes from 2 s up to 60 s and resets after connecting");
  Fixture f;
  f.driver.air = {};  // router off
  f.wifi.setNetworks({{"home", "hp"}});
  f.wifi.begin();
  f.wifi.update();
  CHECK(f.wifi.status().state == WifiState::NoNetwork);

  const uint32_t expected[] = {2000, 4000, 8000, 16000, 32000, 60000, 60000};
  for (uint32_t delay : expected) {
    int before = f.driver.scansStarted;
    f.clock.now += delay - 1;
    f.wifi.update();
    CHECK(f.driver.scansStarted == before);  // not yet
    f.clock.now += 1;
    f.wifi.update();
    CHECK(f.driver.scansStarted == before + 1);  // retried after `delay`
    f.wifi.update();  // scan finds nothing again
    CHECK(f.wifi.status().state == WifiState::NoNetwork);
  }
  CHECK(f.wifi.status().reconnects == 7);

  // Router comes back: connect and reset the backoff.
  f.driver.air = {{"home", -55}};
  f.clock.now += 60000;
  f.wifi.update();  // rescan starts
  f.wifi.update();  // connect
  f.wifi.update();  // connected
  CHECK(f.wifi.status().state == WifiState::Connected);

  // Lose it again: first retry is back at 2 s.
  f.driver.connected = false;
  f.wifi.update();
  CHECK(f.wifi.status().state == WifiState::Disconnected);
  int before = f.driver.scansStarted;
  f.clock.now += 2000;
  f.wifi.update();
  CHECK(f.driver.scansStarted == before + 1);
}

static void testReconnectsAfterRouterCycle() {
  puts("reconnects after the router is switched off and on");
  Fixture f;
  f.driver.air = {{"home", -55}};
  f.wifi.setNetworks({{"home", "hp"}});
  f.wifi.begin();
  f.wifi.update();
  f.wifi.update();
  CHECK(f.wifi.isConnected());

  f.driver.air = {};
  f.driver.connected = false;
  f.wifi.update();
  CHECK(f.wifi.status().state == WifiState::Disconnected);
  f.clock.now += 2000;
  f.wifi.update();  // rescan
  f.wifi.update();  // nothing found
  CHECK(f.wifi.status().state == WifiState::NoNetwork);

  f.driver.air = {{"home", -55}};
  f.clock.now += 4000;
  f.wifi.update();  // rescan
  f.wifi.update();  // connect
  f.wifi.update();  // connected
  CHECK(f.wifi.isConnected());
  CHECK(f.wifi.status().reconnects == 2);
}

static void testConnectTimeout() {
  puts("a join attempt that never succeeds times out");
  Fixture f;
  f.driver.autoJoin = false;
  f.driver.air = {{"home", -55}};
  f.wifi.setNetworks({{"home", "hp"}});
  f.wifi.begin();
  f.wifi.update();
  CHECK(f.wifi.status().state == WifiState::Connecting);
  f.clock.now += WifiManager::kConnectTimeoutMs;
  f.wifi.update();
  CHECK(f.wifi.status().state == WifiState::Connecting);  // not yet (> only)
  f.clock.now += 1;
  f.wifi.update();
  CHECK(f.wifi.status().state == WifiState::Disconnected);
}

static void testRoamsToStrongerAfterRescan() {
  puts("picks up a stronger known network after a rescan");
  Fixture f;
  f.driver.air = {{"home", -80}};
  f.wifi.setNetworks({{"home", "hp"}, {"office", "op"}});
  f.wifi.begin();
  f.wifi.update();
  f.wifi.update();
  CHECK(f.wifi.status().ssid == "home");

  // Drop the link; at the rescan "office" is now stronger.
  f.driver.air = {{"home", -80}, {"office", -45}};
  f.driver.connected = false;
  f.wifi.update();
  f.clock.now += 2000;
  f.wifi.update();  // rescan
  f.wifi.update();  // connect
  f.wifi.update();  // connected
  CHECK(f.wifi.status().ssid == "office");
  CHECK(f.wifi.isConnected());
}

static void testFallbackToSecrets() {
  puts("falls back to WIFI_SSID and WIFI_PASSWORD when no networks are set");
  Fixture f;
  f.driver.air = {{"fallback-ssid", -60}};
  f.wifi.begin();
  f.wifi.update();
  CHECK(f.driver.lastSsid == "fallback-ssid");
  CHECK(f.driver.lastPassword == "fallback-pass");
}

static void testConfiguredNetworksBeatFallback() {
  puts("configured networks take precedence over the fallback");
  Fixture f;
  f.driver.air = {{"fallback-ssid", -30}, {"home", -70}};
  f.wifi.setNetworks({{"home", "hp"}});
  f.wifi.begin();
  f.wifi.update();
  CHECK(f.driver.lastSsid == "home");
}

static void testStatusBeforeBeginAndRssi() {
  puts("status() before begin and RSSI updates while connected");
  Fixture f;
  WifiStatus s = f.wifi.status();
  CHECK(s.state == WifiState::Idle);
  CHECK(s.ssid.empty());
  CHECK(s.reconnects == 0);

  f.driver.air = {{"home", -55}};
  f.wifi.setNetworks({{"home", "hp"}});
  f.wifi.begin();
  CHECK(f.wifi.status().state == WifiState::Scanning);
  f.wifi.update();
  f.wifi.update();
  f.driver.currentRssi = -63;
  f.wifi.update();
  CHECK(f.wifi.status().rssi == -63);
}

static void testPortalApName() {
  const uint8_t mac[6] = {0x10, 0x20, 0x30, 0x40, 0xAB, 0x0F};
  CHECK(portal::apName(mac) == "Radar-AB0F");
}

static void testPortalQrPayload() {
  CHECK(portal::wifiQrPayload("Radar-AB0F") == "WIFI:T:nopass;S:Radar-AB0F;;");
  CHECK(portal::escapeWifiField("a;b:c") == "a\\;b\\:c");
}

static void testPortalCleanScan() {
  std::vector<PortalNetwork> raw = {
      {"Home", -70}, {"", -40}, {"Cafe", -50}, {"Home", -60}};
  auto out = portal::cleanScan(raw);
  CHECK(out.size() == 2);
  CHECK(out[0].ssid == "Cafe");
  CHECK(out[1].ssid == "Home" && out[1].rssi == -60);
  CHECK(portal::signalBars(-50) == 4 && portal::signalBars(-90) == 0);
}

static void testPortalPostcode() {
  CHECK(portal::normalizePostcode("1234ab") == "1234 AB");
  CHECK(portal::normalizePostcode(" 1234 AB ") == "1234 AB");
  CHECK(portal::normalizePostcode("0123 AB").empty());
  CHECK(portal::normalizePostcode("12345").empty());
  CHECK(portal::normalizePostcode("1234 A1").empty());
}

static void testPortalValidate() {
  SetupResult out;
  CHECK(portal::validate({"Net", "password1", "1234ab", ""}, out).empty());
  CHECK(out.postcode == "1234 AB" && out.profileName == "Home");
  CHECK(portal::validate({"", "", "1234 AB", "x"}, out) != "");
  CHECK(portal::validate({"Net", "short", "1234 AB", "x"}, out) != "");
  CHECK(portal::validate({"Net", "", "1234 AB", "x"}, out).empty());  // open
  CHECK(portal::validate({"Net", "", "nope", "x"}, out) != "");
}

static void testPortalPageEscapes() {
  std::string page = portal::setupPage({{"<b>\"x", -50}}, "");
  CHECK(page.find("<b>\"x") == std::string::npos);
  CHECK(page.find("&lt;b&gt;&quot;x") != std::string::npos);
  CHECK(portal::donePage("A&B").find("A&amp;B") != std::string::npos);
}

int main() {
  testConnectsToStrongestKnown();
  testWaitsForScan();
  testBackoffDoublesToMax();
  testReconnectsAfterRouterCycle();
  testConnectTimeout();
  testRoamsToStrongerAfterRescan();
  testFallbackToSecrets();
  testConfiguredNetworksBeatFallback();
  testStatusBeforeBeginAndRssi();
  testPortalApName();
  testPortalQrPayload();
  testPortalCleanScan();
  testPortalPostcode();
  testPortalValidate();
  testPortalPageEscapes();
  if (failures) {
    printf("%d check(s) failed\n", failures);
    return 1;
  }
  puts("all tests passed");
  return 0;
}
