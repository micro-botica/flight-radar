# Decisions

## 1. ADS-B data source

Status: accepted (proposal by Claude, to be confirmed by Maarten and Mick in issue #1)

### Context

The radar needs live aircraft positions around the device. We refresh every 5 to 10 seconds and show position, altitude, heading, callsign, aircraft type and registration.

### Options compared

| | airplanes.live | adsb.lol | OpenSky |
|---|---|---|---|
| Rate limit | 1 request per second | Not published, dynamic based on load | Daily credits: 400 anonymous, 4,000 registered, 8,000 active feeder |
| API key | No | No (may become required for feeders later) | No when anonymous, account for more credits |
| Terms | Non-commercial, no SLA | Data under ODbL, commercial use unclear | Research and non-commercial |
| Query by location | Point and radius (`/v2/point/lat/lon/nm`) | Point and radius in nautical miles, up to about 250 nm | Bounding box only (`/states/all`) |
| Freshness | Real time | Real time | 10 s anonymous, 5 s logged in |
| Fields | readsb style JSON: hex, callsign, position, altitude, speed, track, vertical rate, squawk, type, registration | Same readsb style JSON | icao24, callsign, position, altitudes, speed, track, vertical rate, squawk, country. No type or registration, many fields can be `null` |

### Decision

1. **Primary: airplanes.live.** Clear published limit, no key, point and radius query that fits a round radar.
2. **Fallback: adsb.lol.** Same JSON format, so one parser serves both.
3. **OpenSky is not used** for the live view. The credit limits do not allow a 5 to 10 second refresh, and it lacks aircraft type and registration.

### Consequences

- The firmware needs one parser for the readsb style JSON format.
- Neither primary nor fallback has an SLA. Terms and limits can change, so the client must handle errors and switch to the fallback.
- We need only 1 request per 5 to 10 seconds, far below the airplanes.live limit.

### To verify

The official pages of airplanes.live and adsb.lol could not be fully fetched during the comparison. Check their current terms of use on the official sites before relying on this decision.

## 2. Ownership per side

Status: accepted (issue #3)

### Context

The work splits into a data side and a UI side. We want clear boundaries so we do not get in each other's way, but Maarten and Mick both work on both sides, depending on who has time.

### Decision

1. **No fixed owner per person.** Either of us can take either side.
2. **Claim by assigning.** Whoever starts an issue assigns it to themselves first. An assigned issue is owned by that person until it is merged or released again.
3. **The boundary is the `Aircraft` list.** The data side produces it, the UI side draws it. Changes to the `Aircraft` struct are agreed in the issue or pull request before they are merged.

### Sides for milestones 1 and 2

| Side | Topics | Issues |
|---|---|---|
| Data | Wi-Fi, time (NTP/RTC), ADS-B fetch, JSON parsing, fake data source, error handling, location and postcode, captive portal | #10, #11, #12, #13, #14, #24, #30, #69, #70, #71 |
| UI | Display with LVGL, touch (CST820), UI theme | #6, #7, #76 |
| Shared | PlatformIO project and CI, then shared | #5, #8 |

### Consequences

- Nobody waits for the other to be free. Pick an unassigned issue and assign yourself.
- Assigning the issues of milestones 1 and 2 happens in GitHub, one by one, when someone picks them up.

## 3. UI theme and layout rules

Status: accepted (issue #76), updated for style guide version 2

### Context

All screens must look the same. The values come from `docs/design/stijlgids.md`, now version 2. Version 2 makes the radar smaller, raises the smallest text to 15 px and uses whole pixel line widths, so the design matches what LVGL can draw.

### Decision

1. **One place for the theme.** Colors, fonts and sizes are constants in `firmware/src/ui/theme.h`. Screens use these constants and never hard-code a color, font or size.
2. **Fonts.** Inter (500, 600) and Space Grotesk (500, 600) from Google Fonts (SIL Open Font License), converted to LVGL fonts in `firmware/src/ui/fonts/`. Only the sizes in the font table of the style guide are included:
   - Space Grotesk 600: 116, 96, 46, 26, 22, 20, 18
   - Space Grotesk 500: 20
   - Inter 600: 20, 18, 16, 15
   - Inter 500: 21 (only the date on the clock)
3. **Large fonts are digits only.** The 116 px and 96 px fonts only contain `0-9`, `:`, `°` and `-`.
4. **Smallest text is 15 px, and small text is Inter 600.** Sizes in the design that are not in the table (28, 24, 21, 19, 17) use the nearest size from the table.
5. **Radar size.** The radar stays within a radius of 180 px from the center (240, 240). Rings at 45, 90, 135 and 180 px, which is 4.5 px per km at the 40 km range.
6. **Edge band.** The band between 180 and 240 px is only for the N indicator (top), aircraft count (top left), time (top right) and the page dots (bottom). Keep at least 10 px from the round edge.
7. **Whole pixel lines.** Inner rings 1 px, outer ring and sweep line 2 px. All rings use Line strong (`#2E3B4A`).
8. **At most 4 callsigns on the radar**, for the 4 nearest aircraft (3 in night mode). A label that touches another label or icon is hidden.
9. **No dashed arcs.** LVGL cannot draw a dashed arc, so the design uses solid lines only.
10. **Touch targets.** At least 48 x 48 px. On the radar, a tap that hits several aircraft selects the nearest one.

### Consequences

- A change to a color or size is made once in `theme.h`.
- Adding a font size means regenerating it and declaring it in `theme.h`.
- Version 1 fonts that are no longer used (Space Grotesk 500 17, Inter 600 13, Inter 500 15 and 13) are removed.
- The sweep fan refreshes a large part of the screen. The expected rate of about 10 frames per second has not been measured yet.
