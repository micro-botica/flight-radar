#include "navigation.h"

#include "theme.h"

namespace {

constexpr int kScreenCount = 5;
constexpr uint32_t kAnimMs = 250;

lv_obj_t* screens[kScreenCount] = {};
lv_obj_t* dotTargets[kPageCount] = {};
lv_obj_t* dots[kPageCount] = {};
lv_obj_t* dotsRow = nullptr;
lv_obj_t* statusLayer = nullptr;
Screen current = Screen::Radar;

lv_obj_t* screenObj(Screen s) { return screens[static_cast<int>(s)]; }

void updateDots() {
  const int active = navPageIndex(current);
  if (active < 0) {
    lv_obj_add_flag(dotsRow, LV_OBJ_FLAG_HIDDEN);
    return;
  }
  lv_obj_clear_flag(dotsRow, LV_OBJ_FLAG_HIDDEN);
  for (int i = 0; i < kPageCount; i++) {
    const bool on = i == active;
    lv_obj_set_width(dots[i], on ? theme::kPageDotActiveWidth : theme::kPageDotHeight);
    lv_obj_set_style_bg_color(
        dots[i], theme::color(on ? theme::kTextWhite : theme::kInactive), 0);
  }
}

lv_scr_load_anim_t animFor(Screen from, Screen to) {
  const int a = navPageIndex(from);
  const int b = navPageIndex(to);
  if (a >= 0 && b >= 0) {
    return b > a ? LV_SCR_LOAD_ANIM_MOVE_LEFT : LV_SCR_LOAD_ANIM_MOVE_RIGHT;
  }
  // Vertical moves: the new screen slides in from the side the finger moves from.
  if (to == Screen::List || from == Screen::Settings) return LV_SCR_LOAD_ANIM_MOVE_TOP;
  return LV_SCR_LOAD_ANIM_MOVE_BOTTOM;
}

void onGesture(lv_event_t*) {
  lv_indev_t* indev = lv_indev_get_act();
  if (indev == nullptr) return;
  Swipe swipe = Swipe::None;
  switch (lv_indev_get_gesture_dir(indev)) {
    case LV_DIR_LEFT: swipe = Swipe::Left; break;
    case LV_DIR_RIGHT: swipe = Swipe::Right; break;
    case LV_DIR_TOP: swipe = Swipe::Up; break;
    case LV_DIR_BOTTOM: swipe = Swipe::Down; break;
    default: return;
  }
  navGoTo(navTarget(current, swipe));
}

void onDotClicked(lv_event_t* e) {
  const int index = static_cast<int>(reinterpret_cast<intptr_t>(lv_event_get_user_data(e)));
  navGoTo(navPageScreen(index, current));
}

const char* screenName(Screen s) {
  switch (s) {
    case Screen::Radar: return "Radar";
    case Screen::Clock: return "Clock";
    case Screen::Weather: return "Weather";
    case Screen::List: return "List";
    case Screen::Settings: return "Settings";
  }
  return "";
}

lv_obj_t* createPlaceholder(Screen s) {
  lv_obj_t* scr = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr, theme::color(theme::kBackground), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(scr, 0, 0);
  lv_obj_set_style_radius(scr, 0, 0);
  lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(scr, onGesture, LV_EVENT_GESTURE, nullptr);

  lv_obj_t* label = lv_label_create(scr);
  lv_label_set_text(label, screenName(s));
  lv_obj_set_style_text_color(label, theme::color(theme::kMuted), 0);
  lv_obj_set_style_text_font(label, theme::kFontTitle, 0);
  lv_obj_add_flag(label, LV_OBJ_FLAG_GESTURE_BUBBLE);
  lv_obj_center(label);
  return scr;
}

void createDots() {
  lv_obj_t* top = lv_layer_top();
  dotsRow = lv_obj_create(top);
  lv_obj_remove_style_all(dotsRow);
  lv_obj_set_size(dotsRow, 3 * theme::kMinTouchSize, theme::kMinTouchSize);
  lv_obj_set_pos(dotsRow, theme::kPageDotsX, theme::kPageDotsY);
  lv_obj_clear_flag(dotsRow, LV_OBJ_FLAG_CLICKABLE);

  for (int i = 0; i < kPageCount; i++) {
    lv_obj_t* target = lv_obj_create(dotsRow);
    lv_obj_remove_style_all(target);
    lv_obj_set_size(target, theme::kMinTouchSize, theme::kMinTouchSize);
    lv_obj_set_pos(target, i * theme::kMinTouchSize, 0);
    lv_obj_add_flag(target, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(target, onDotClicked, LV_EVENT_CLICKED,
                        reinterpret_cast<void*>(static_cast<intptr_t>(i)));
    dotTargets[i] = target;

    lv_obj_t* dot = lv_obj_create(target);
    lv_obj_remove_style_all(dot);
    lv_obj_set_height(dot, theme::kPageDotHeight);
    lv_obj_set_style_radius(dot, theme::kPageDotRadius, 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_clear_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(dot);
    dots[i] = dot;
  }
}

}  // namespace

void navInit() {
  for (int i = 0; i < kScreenCount; i++) {
    screens[i] = createPlaceholder(static_cast<Screen>(i));
  }
  createDots();

  // Created last so it covers the dots. Clickable, so it swallows touches.
  statusLayer = lv_obj_create(lv_layer_top());
  lv_obj_remove_style_all(statusLayer);
  lv_obj_set_size(statusLayer, theme::kScreenSize, theme::kScreenSize);
  lv_obj_set_style_bg_color(statusLayer, theme::color(theme::kBackground), 0);
  lv_obj_set_style_bg_opa(statusLayer, LV_OPA_COVER, 0);
  lv_obj_add_flag(statusLayer, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(statusLayer, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(statusLayer, LV_OBJ_FLAG_HIDDEN);

  current = Screen::Radar;
  lv_scr_load(screenObj(current));
  updateDots();
}

void navGoTo(Screen screen) {
  if (screen == current) return;
  const lv_scr_load_anim_t anim = animFor(current, screen);
  current = screen;
  lv_scr_load_anim(screenObj(screen), anim, kAnimMs, 0, false);
  updateDots();
}

Screen navCurrent() { return current; }

lv_obj_t* navStatusLayer() { return statusLayer; }

void navShowStatus(bool show) {
  if (show) {
    lv_obj_clear_flag(statusLayer, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(statusLayer, LV_OBJ_FLAG_HIDDEN);
  }
}
