# Flight Radar

A live aircraft radar on a round 2.1 inch touchscreen. Over Wi-Fi the display fetches ADS-B data for aircraft around your home location and shows them as blips on a classic radar, alongside the time and the current weather.

> **Status:** work in progress. We are in the kick-off phase; there is no working firmware yet.

## Features

Planned for the first version:

- Live aircraft within an adjustable radius around your location
- Radar view with range rings, a north indicator and a rotating sweep
- Aircraft icons pointing in their direction of flight, with callsign
- Tap an aircraft for details: type, altitude, speed and distance
- Clock, automatically synchronised over the internet
- Current weather for your location
- Configuration over Wi-Fi without touching the code, plus over-the-air updates

Ideas for later are listed as optional milestones on the [project board](https://github.com/orgs/micro-botica/projects/1).

## Hardware

The whole project runs on a single board, with no extra sensors or modules:

**[Waveshare ESP32-S3-Touch-LCD-2.1](https://docs.waveshare.com/ESP32-S3-Touch-LCD-2.1?variant=ESP32-S3-Touch-LCD-2.1)**

| Component | Specification |
|---|---|
| Microcontroller | ESP32-S3, dual core 240 MHz, Wi-Fi and Bluetooth LE |
| Memory | 16 MB flash, 8 MB PSRAM |
| Display | 2.1 inch round IPS, 480x480, ST7701S driver |
| Touch | Capacitive, CST820 controller |
| Other | RTC (PCF85063A), IMU (QMI8658), buzzer, microSD slot |

Available from, among others, [TinyTronics](https://www.tinytronics.nl/nl/development-boards/microcontroller-boards/met-wi-fi/waveshare-esp32-s3-met-2.1-inch-rond-ips-display-met-touchscreen-en-imu). The enclosure will be 3D printed; the files will go in `enclosure/`.

## Tech stack

- **Framework:** Arduino via [PlatformIO](https://platformio.org/)
- **UI:** [LVGL](https://lvgl.io/), which can also run as a simulator on a PC
- **Aircraft data:** a free ADS-B API (to be chosen during the kick-off)
- **Weather data:** to be chosen during the kick-off

## Project structure

```
flight-radar/
├── firmware/            # PlatformIO project for the board
│   ├── src/
│   │   ├── data/        # Wi-Fi, API clients, parsing
│   │   ├── ui/          # LVGL screens, radar, touch
│   │   └── model/       # Aircraft model and geo calculations
│   └── include/         # e.g. secrets.example.h
├── simulator/           # LVGL on the PC with mock data
├── enclosure/           # 3D print files for the enclosure
└── docs/
    └── decisions.md     # why we made which choices
```

This structure will be set up during milestone 1.

## Getting started

Build and flash instructions will follow once the PlatformIO project is in place. Roughly, it will look like this:

1. Install [VS Code](https://code.visualstudio.com/) with the PlatformIO extension.
2. Clone this repository.
3. Copy `firmware/include/secrets.example.h` to `secrets.h` and fill in your Wi-Fi details.
4. Connect the board over USB-C and click **Upload** in PlatformIO.

## Roadmap

We work in milestones, which you can find on the [project board](https://github.com/orgs/micro-botica/projects/1) and under [milestones](https://github.com/micro-botica/flight-radar/milestones?sort=title&direction=asc):

| # | Milestone | Scope |
|---|---|---|
| 0 | Kick-off | Choose data source, data model and division of work |
| 1 | Hello screen | Display, touch and CI build working |
| 2 | Data in | Wi-Fi, aircraft data and time synchronisation |
| 3 | Radar view | Aircraft on the radar, starting with mock data |
| 4 | Clock and weather | Time and weather on the screen |
| 5 | Interaction | Details, zooming, colours and night mode |
| 6 | Finishing touches | Settings page, over-the-air updates and enclosure |
| 7 to 14 | Optional | Ideas for further development |

## Contributing

- Every change goes through its own branch and a pull request to `main`.
- Link your PR to an issue with `Closes #nr` in the description, so the issue moves along the board automatically.
- The other person reviews the PR before it is merged.
- Wi-Fi passwords and API keys never go into Git, only into your local `secrets.h`.

## Makers

Made by **Maarten** ([@Remotev2](https://github.com/Remotev2)) and **Mick** ([@MVelden1](https://github.com/MVelden1)), with help from [Claude](https://claude.ai) by Anthropic for the setup, planning and programming.

Inspired by the [ESP32 Plane Radar](https://makerworld.com/nl/models/2872376-esp32-plane-radar-live-ads-b-on-a-round-display) project on MakerWorld.

## License

[MIT](LICENSE)
