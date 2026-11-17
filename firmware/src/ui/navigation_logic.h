#pragma once

// Pure screen navigation rules, free of LVGL so they can be tested on a PC.
//
// Layout: radar, clock and weather sit in a row (swipe left and right). The
// list is reached by swiping up on the radar, settings by swiping down.

enum class Screen { Radar, Clock, Weather, List, Settings };

enum class Swipe { None, Left, Right, Up, Down };

constexpr int kPageCount = 3;  // Radar, clock, weather: the screens with dots

// Where a swipe leads from `current`. Returns `current` if the swipe does
// nothing there. A swipe is the direction the finger moves.
constexpr Screen navTarget(Screen current, Swipe swipe) {
  switch (current) {
    case Screen::Radar:
      return swipe == Swipe::Left   ? Screen::Clock
             : swipe == Swipe::Up   ? Screen::List
             : swipe == Swipe::Down ? Screen::Settings
                                    : current;
    case Screen::Clock:
      return swipe == Swipe::Left    ? Screen::Weather
             : swipe == Swipe::Right ? Screen::Radar
                                     : current;
    case Screen::Weather:
      return swipe == Swipe::Right ? Screen::Clock : current;
    case Screen::List:
      return swipe == Swipe::Down ? Screen::Radar : current;
    case Screen::Settings:
      return swipe == Swipe::Up ? Screen::Radar : current;
  }
  return current;
}

// Page dot index (0 to 2) of a screen, or -1 for screens without dots.
constexpr int navPageIndex(Screen screen) {
  return screen == Screen::Radar     ? 0
         : screen == Screen::Clock   ? 1
         : screen == Screen::Weather ? 2
                                     : -1;
}

// Screen behind page dot `index`, or `fallback` when the index is invalid.
constexpr Screen navPageScreen(int index, Screen fallback) {
  return index == 0   ? Screen::Radar
         : index == 1 ? Screen::Clock
         : index == 2 ? Screen::Weather
                      : fallback;
}
