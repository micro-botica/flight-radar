#include <Arduino.h>
#include <lvgl.h>

#include "data/arduino_wifi_driver.h"
#include "data/wifi_manager.h"
#include "ui/display.h"
#include "ui/navigation.h"

static ArduinoWifiDriver wifiDriver;
static ArduinoWifiClock wifiClock;
static WifiManager wifi(wifiDriver, wifiClock);

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

  // Known networks will come from the location profiles; until then
  // begin() falls back to secrets.h.
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
  displayUpdate();
  static uint32_t lastLog = 0;
  if (millis() - lastLog >= 5000) {
    lastLog = millis();
    Serial.printf("alive, uptime %lu s, wifi %d\n", millis() / 1000,
                  (int)wifi.status().state);
  }
  delay(5);
}
