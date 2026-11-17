#include "setup_screen.h"

#include <lvgl.h>

#include "../setup/portal_core.h"

void showSetupScreen(const std::string& apName) {
  lv_obj_t* screen = lv_scr_act();
  lv_obj_clean(screen);
  lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

  lv_obj_t* title = lv_label_create(screen);
  lv_label_set_text(title, "Set up with your phone");
  lv_obj_set_style_text_color(title, lv_color_hex(0x00FF66), 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 50);

  std::string payload = portal::wifiQrPayload(apName);
  // White background with a quiet zone so phone cameras read it reliably.
  lv_obj_t* qr = lv_qrcode_create(screen, 220, lv_color_black(),
                                  lv_color_white());
  lv_qrcode_update(qr, payload.c_str(), payload.size());
  lv_obj_set_style_border_color(qr, lv_color_white(), 0);
  lv_obj_set_style_border_width(qr, 8, 0);
  lv_obj_align(qr, LV_ALIGN_CENTER, 0, -10);

  lv_obj_t* name = lv_label_create(screen);
  lv_label_set_text_fmt(name, "Wi-Fi: %s", apName.c_str());
  lv_obj_set_style_text_color(name, lv_color_white(), 0);
  lv_obj_set_style_text_font(name, &lv_font_montserrat_28, 0);
  lv_obj_align(name, LV_ALIGN_BOTTOM_MID, 0, -80);
}
