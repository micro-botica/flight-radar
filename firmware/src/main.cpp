#include <Arduino.h>
#include <Preferences.h>
#include <lvgl.h>

#if __has_include("secrets.h")
#include "secrets.h"
#endif

#include "data/arduino_wifi_driver.h"
#include "data/wifi_manager.h"
#include "setup/setup_portal.h"
#include "ui/display.h"
#include "ui/setup_screen.h"

// Temporary hello screen for the display bring-up: a circle with a text.
static void showHelloScreen() {
  lv_obj_t* screen = lv_scr_act();
  lv_obj_clean(screen);
  lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

  lv_obj_t* circle = lv_obj_create(screen);
  lv_obj_set_size(circle, 360, 360);
  lv_obj_center(circle);
  lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_opa(circle, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_color(circle, lv_color_hex(0x00FF66), 0);
  lv_obj_set_style_border_width(circle, 4, 0);

  lv_obj_t* label = lv_label_create(screen);
  lv_label_set_text(label, "Flight Radar");
  lv_obj_set_style_text_color(label, lv_color_hex(0x00FF66), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
  lv_obj_center(label);

  // Test button for the touch bring-up: it turns green while pressed.
  lv_obj_t* button = lv_btn_create(screen);
  lv_obj_set_size(button, 160, 60);
  lv_obj_align(button, LV_ALIGN_CENTER, 0, 90);
  lv_obj_set_style_bg_color(button, lv_color_hex(0x333333), 0);
  lv_obj_set_style_bg_color(button, lv_color_hex(0x00FF66), LV_STATE_PRESSED);
  lv_obj_t* buttonLabel = lv_label_create(button);
  lv_label_set_text(buttonLabel, "Touch");
  lv_obj_center(buttonLabel);
}

static ArduinoWifiDriver wifiDriver;
static ArduinoWifiClock wifiClock;
static WifiManager wifi(wifiDriver, wifiClock);

static SetupPortal setupPortal;
static bool showSetup = false;

// Setup result storage. Temporary: issue #69 (location profiles in NVS)
// replaces this with proper profiles.
static const char* kPrefsNamespace = "setup";

static void startWifiWith(const SetupResult& r) {
  wifi.setNetworks({{r.ssid, r.password}});
  wifi.begin();
}

static void saveSetup(const SetupResult& r) {
  Preferences prefs;
  prefs.begin(kPrefsNamespace, false);
  prefs.putString("ssid", r.ssid.c_str());
  prefs.putString("pass", r.password.c_str());
  prefs.putString("zip", r.postcode.c_str());
  prefs.putString("name", r.profileName.c_str());
  prefs.end();
}

// Returns true when a saved setup was found.
static bool loadSetup(SetupResult& r) {
  Preferences prefs;
  prefs.begin(kPrefsNamespace, true);
  r.ssid = prefs.getString("ssid", "").c_str();
  r.password = prefs.getString("pass", "").c_str();
  r.postcode = prefs.getString("zip", "").c_str();
  r.profileName = prefs.getString("name", "").c_str();
  prefs.end();
  return !r.ssid.empty();
}

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

  // Known networks will come from the location profiles; until then they
  // come from the first-time setup, or from secrets.h for development.
  SetupResult saved;
  bool haveNetwork = loadSetup(saved);
#if defined(WIFI_SSID)
  bool hasFallback = true;
#else
  bool hasFallback = false;
#endif
  if (haveNetwork) {
    startWifiWith(saved);
  } else if (hasFallback) {
    wifi.begin();
  } else {
    showSetup = true;  // no known Wi-Fi: first-time setup via phone
  }

  if (!displayInit()) {
    Serial.println("Display init failed");
    return;
  }
  if (showSetup) {
    setupPortal.begin([](const SetupResult& r) {
      saveSetup(r);
      startWifiWith(r);
      showHelloScreen();  // replaced by the radar screen later
    });
    showSetupScreen(setupPortal.apName());
  } else {
    showHelloScreen();
  }
  lv_timer_handler();  // Draw the first frame before the backlight comes on.
  displaySetBacklight(100);
  Serial.println("Display ready");
}

void loop() {
  wifi.update();
  setupPortal.update();
  displayUpdate();
  static uint32_t lastLog = 0;
  if (millis() - lastLog >= 5000) {
    lastLog = millis();
    Serial.printf("alive, uptime %lu s, wifi %d\n", millis() / 1000,
                  (int)wifi.status().state);
  }
  delay(5);
}
