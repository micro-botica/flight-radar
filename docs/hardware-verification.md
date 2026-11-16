# Hardware verification

Things that are built but can only be verified on the real board (Waveshare ESP32-S3-Touch-LCD-2.1). Work through this list when the board arrives, tick the boxes and close the linked issue.

Add a section here whenever an issue ends up with "needs the board" items. Remove the section once everything in it has been verified.

## Before you start

- [ ] `pio run` builds the firmware, with and without `firmware/include/secrets.h`
- [ ] The board boots and shows output on the serial monitor

## Wi-Fi manager (issues #10 and #92)

Code: `firmware/src/data/wifi_manager.{h,cpp}`. Logic tests that do not need the board are tracked in #94.

- [ ] Connects to the strongest known network at startup
- [ ] Reconnects after the router is switched off and on, with backoff going from 2 s up to 60 s
- [ ] Picks up a stronger known network after a rescan
- [ ] Falls back to `WIFI_SSID` and `WIFI_PASSWORD` from `secrets.h` when no networks are set
- [ ] `WifiManager::status()` returns the expected state, SSID, RSSI and reconnect count on the device

## Display and touch

Add the items from the display and touch issues here when they are ready for hardware testing.
