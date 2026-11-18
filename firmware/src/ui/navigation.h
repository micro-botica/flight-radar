#pragma once

#include <lvgl.h>

#include "navigation_logic.h"

// Screen navigation: placeholder screens, swipe gestures and page dots.
// Call navInit() once after displayInit().

// Creates the screens, the page dots and the status layer, and loads the radar.
void navInit();

// Switches to a screen with the matching slide animation.
void navGoTo(Screen screen);

Screen navCurrent();

// Container for status screens (no Wi-Fi, no location, ...). It covers the
// whole display on top of every screen and the page dots, and blocks swipes
// while it is shown. Add your content to it, then call navShowStatus().
lv_obj_t* navStatusLayer();
void navShowStatus(bool show);
