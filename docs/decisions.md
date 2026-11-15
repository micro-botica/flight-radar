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
