#include <Arduino.h>
#include <lvgl.h>

#include "data/wifi_manager.h"
#include "ui/display.h"

// Temporary hello screen for the display bring-up: a circle with a text.
static void showHelloScreen() {
  lv_obj_t* screen = lv_scr_act();
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

static WifiManager wifi;

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
  showHelloScreen();
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
