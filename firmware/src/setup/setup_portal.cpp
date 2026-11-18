#ifdef ARDUINO

#include "setup_portal.h"

#include <Arduino.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>

#include "portal_pages.h"

static DNSServer dns;
static WebServer server(80);
static std::vector<PortalNetwork> lastScan;
static SetupResult pending;
static bool finished = false;

void SetupPortal::begin(DoneCallback onDone) {
  onDone_ = onDone;
  finished = false;

  WiFi.mode(WIFI_AP_STA);  // STA stays on so we can scan while in AP mode
  uint8_t mac[6];
  WiFi.macAddress(mac);
  apName_ = portal::apName(mac);
  WiFi.softAP(apName_.c_str());  // open network, no password
  delay(100);

  // Answer every DNS lookup with our own address: phones then detect the
  // captive portal and open the setup page by themselves.
  dns.start(53, "*", WiFi.softAPIP());

  server.on("/", [this]() { handleRoot(); });
  server.on("/save", HTTP_POST, [this]() { handleSave(); });
  server.onNotFound([this]() { handleRedirect(); });  // probe URLs of iOS/Android/Windows
  server.begin();

  refreshScan();
  active_ = true;
  Serial.printf("Setup portal: %s at %s\n", apName_.c_str(),
                WiFi.softAPIP().toString().c_str());
}

void SetupPortal::refreshScan() {
  int n = WiFi.scanNetworks(false /* blocking, only at setup */);
  std::vector<PortalNetwork> raw;
  for (int i = 0; i < n; i++) {
    raw.push_back({WiFi.SSID(i).c_str(), WiFi.RSSI(i)});
  }
  WiFi.scanDelete();
  lastScan = portal::cleanScan(raw);
}

void SetupPortal::handleRoot() {
  if (server.method() == HTTP_GET && server.hasArg("rescan")) refreshScan();
  server.send(200, "text/html", portal::setupPage(lastScan, "").c_str());
}

void SetupPortal::handleSave() {
  SetupResult in{server.arg("ssid").c_str(), server.arg("password").c_str(),
                 server.arg("postcode").c_str(), server.arg("name").c_str()};
  SetupResult clean;
  std::string error = portal::validate(in, clean);
  if (!error.empty()) {
    server.send(400, "text/html", portal::setupPage(lastScan, error).c_str());
    return;
  }
  server.send(200, "text/html", portal::donePage(clean.ssid).c_str());
  pending = clean;
  finished = true;  // acted on in update(), after the response went out
}

void SetupPortal::handleRedirect() {
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", "");
}

void SetupPortal::update() {
  if (!active_) return;
  dns.processNextRequest();
  server.handleClient();
  if (finished) {
    finished = false;
    delay(500);  // let the phone receive the done page
    server.stop();
    dns.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    active_ = false;
    if (onDone_) onDone_(pending);
  }
}

#endif  // ARDUINO
