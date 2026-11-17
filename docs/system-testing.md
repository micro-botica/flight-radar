# System tests

Checks that can only be done on the real board (or on a full build) and that are still open. Every feature that is added gets an entry here, with what to test, how, and the expected result.

How to use this file:

- Add a section per feature in the same pull request that adds the feature.
- Link the issue that tracks the open checks.
- When a check passes on the board, remove it from this file and tick the item in the issue.
- Remove the section when all its checks are verified.

## Preparation (once the board is available)

| Check | How | Expected |
|---|---|---|
| Firmware builds | `cd firmware && pio run`, with and without `include/secrets.h` | Build succeeds both times |
| Native tests | `cd firmware && pio test -e native` (or `pio run -e native` if that is how the env is set up) | All tests pass |
| Board boots | Flash with `pio run -t upload`, then `pio device monitor` | Serial output appears, no reboot loop |

## Wi-Fi manager (issues #10, #92, #94)

Logic is covered by mock tests (#94). The checks below need a real radio and router.

| Check | How | Expected |
|---|---|---|
| Strongest known network at startup | Configure two known networks that are both in range, then reset the board | It connects to the one with the best RSSI |
| Reconnect with backoff | Switch the router off for 2 minutes, then on again. Watch serial. | Retries wait 2 s, 4 s, and so on up to 60 s. It reconnects after the router is back and the backoff resets. |
| Roaming after rescan | Connect to the weaker network, then make the stronger one available. Wait for the rescan. | It switches to the stronger network |
| Fallback to `secrets.h` | Set no networks, define `WIFI_SSID` and `WIFI_PASSWORD` | It connects with those credentials |
| Build without `secrets.h` | Remove `include/secrets.h` and build | Build succeeds and the firmware runs |
| `WifiManager::status()` | Print `status()` to serial in each situation above | State, SSID, RSSI and reconnect count match reality |

## UI theme and fonts (issue #76, style guide version 2)

The theme and fonts compile on a PC against LVGL 8.4 with the project's `lv_conf.h`, but they have not been built with `pio run` or shown on the board. Open checks are tracked in #102.

| Check | How | Expected |
|---|---|---|
| Firmware builds with the fonts | `cd firmware && pio run` | Build succeeds, flash size stays well below 16 MB |
| Fonts render | Show a test label per font in `theme.h` (for example "Aa 0123 12:34 18°") | Glyphs are sharp and complete. The clock and temperature fonts show `0-9 : ° -` and nothing else. |
| Smallest text is readable | Show ring labels and help text in `kFontSmall` (Inter 600 15 px) at arm's length | Readable without effort |
| Colors match the style guide | Fill test squares with the day colors and compare with the style guide on the display | Colors look the same as in the design |
| Radar radius | Draw the four rings from `kRingRadius` around (240, 240) | Outer ring at 180 px, inner rings 1 px and outer ring 2 px, all in Line strong |

## Screen navigation (issue #77)

The navigation rules are covered by native tests (`navigation_logic.h`). `ui/navigation.cpp` has not been compiled or run: LVGL and the board were not available. Open checks are tracked in #104.

| Check | How | Expected |
|---|---|---|
| Firmware builds | `cd firmware && pio run` | Build succeeds |
| Horizontal swipe | Swipe left and right on each screen | Radar, clock and weather follow each other with a slide animation. Nothing happens past the first and last screen. |
| Page dots | Look at the bottom while switching, tap each dot | The active dot is a white pill, the others small grey dots. Tapping a dot jumps to that screen. Touch targets are 48 x 48 px. |
| Vertical swipe | On the radar swipe up, then down, then down again, then up | Up opens the list, down returns. Down opens settings, up returns. Clock and weather ignore vertical swipes. |
| Dots hidden | Open the list and settings | No page dots on those screens |
| Status layer | Call `navShowStatus(true)` from `main.cpp` temporarily | A full-screen layer covers screen and dots, and swipes and dot taps do nothing. `navShowStatus(false)` brings the screen back. |
| Swipe reliability | Swipe quickly and from different spots | Gestures are recognised reliably and do not trigger on taps |

## Display and touch

No open checks yet. Add a section when a display or touch issue needs the board.
