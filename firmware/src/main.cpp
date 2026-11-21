#include <Arduino.h>
#include <lvgl.h>

#include "data/arduino_wifi_driver.h"
#include "data/profile_store.h"
#include "data/profile_web.h"
#include "data/wifi_manager.h"
#include "ui/display.h"
#include "ui/navigation.h"

static ArduinoWifiDriver wifiDriver;
static ArduinoWifiClock wifiClock;
static WifiManager wifi(wifiDriver, wifiClock);
static ProfileStore profiles;
static ProfileWeb profileWeb(profiles);

void setup() {
  Serial.begin(115200);
  // Give the USB CDC serial port time to enumerate before the first message.
  delay(1500);

  Serial.println("Flight Radar starting");
  Serial.printf("Chip: %s, rev %d, %d MHz\n", ESP.getChipModel(),
                ESP.getChipRevision(), ESP.getCpuFreqMHz());
  Serial.printf("Flash: %u MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  if (psramFound()) {
    Serial.printf("PSRAM: %u MB\n", ESP.getPsramSize() / (1024 * 1024));
  } else {
    Serial.println("PSRAM: not found");
  }

  // Known networks come from the location profiles. With no profiles,
  // begin() falls back to secrets.h.
  profiles.begin();
  profiles.onChange([](int index, const Profile& p) {
    // The radar, clock and weather will subscribe here to jump to the new
    // location (name, coordinates and range).
    Serial.printf("Profile: switched to %d (%s), range %u km\n", index,
                  p.name.c_str(), p.rangeKm);
  });
  wifi.setNetworks(profiles.networks());
  profileWeb.onProfilesChanged([]() { wifi.setNetworks(profiles.networks()); });
  wifi.begin();

  if (!displayInit()) {
    Serial.println("Display init failed");
    return;
  }
  navInit();
  lv_timer_handler();  // Draw the first frame before the backlight comes on.
  displaySetBacklight(100);
  Serial.println("Display ready");
}

void loop() {
  wifi.update();
  // The active profile follows the connected network.
  profiles.onWifiChanged(wifi.isConnected() ? String(wifi.status().ssid.c_str()) : String());
  profileWeb.update(wifi.isConnected());
  displayUpdate();
  static uint32_t lastLog = 0;
  if (millis() - lastLog >= 5000) {
    lastLog = millis();
    Serial.printf("alive, uptime %lu s, wifi %d\n", millis() / 1000,
                  (int)wifi.status().state);
  }
  delay(5);
}
