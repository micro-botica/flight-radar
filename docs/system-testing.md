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

## First-time setup via phone (issues #24, #99)

The portal logic (access point name, QR payload, scan list, validation, HTML escaping) is covered by native tests. The firmware was not compiled and not run on a board, so everything below is open. Tracked in #99.

| Check | How | Expected |
|---|---|---|
| Firmware builds | `cd firmware && pio run` without `include/secrets.h` | Build succeeds (LVGL QR code enabled, `Preferences`, `WebServer`, `DNSServer` found) |
| Access point starts | Flash a board with no saved setup and no `secrets.h`. | Access point `Radar-XXXX` (last 2 MAC bytes) appears, open network |
| QR code and name | Look at the display | QR code and the text `Wi-Fi: Radar-XXXX` are readable |
| QR joins the network | Scan the QR with iOS and Android camera | Phone joins `Radar-XXXX` |
| Portal opens by itself | After joining, wait a few seconds | Setup page opens automatically on iOS and Android. If not, `192.168.4.1` works. |
| Step 1 scan list | Open the page | Networks sorted by signal, with bars and dBm, no duplicates |
| Step 2 password | Type a password, toggle show | Text and dots toggle |
| Step 3 and validation | Enter a bad postcode, then a good one | Error shown for bad, accepted for good |
| Step 4 and connect | Save | Done page shows, AP stops, radar connects to the chosen Wi-Fi |
| Survives reboot | Reset the board | It connects without showing the portal |
| Wrong password | Save a wrong password | Known gap: no way back into setup yet. Erase the NVS namespace `setup` (to be handled with #58 or #78) |

Setup data is stored in NVS namespace `setup` as a temporary measure until location profiles (#69) land.

## Display and touch

No open checks yet. Add a section when a display or touch issue needs the board.
