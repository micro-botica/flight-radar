#pragma once

#include <lvgl.h>

#include <string>

// Content for the setup portal: a QR code to join the access point and the
// network name as text. Draws into parent, normally navStatusLayer().
void showSetupScreen(lv_obj_t* parent, const std::string& apName);
