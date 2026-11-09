# Flight Radar

Een live vliegtuigradar op een rond 2,1 inch touchscreen. Het scherm haalt via wifi ADS-B-data op van vliegtuigen rond je thuislocatie en toont ze als stipjes op een klassieke radar, met daarnaast de tijd en het actuele weer.

> **Status:** in opbouw. We zitten in de kick-off-fase; er is nog geen werkende firmware.

## Features

Gepland voor de eerste versie:

- Live vliegtuigen binnen een instelbare straal rond je locatie
- Radarweergave met afstandsringen, noordindicator en een draaiende sweep
- Vliegtuigiconen in de vliegrichting, met callsign
- Tik op een vliegtuig voor details: type, hoogte, snelheid en afstand
- Klok, automatisch gesynchroniseerd via internet
- Actueel weer voor je locatie
- Instellen via wifi zonder code aan te passen, en updates over wifi

Ideeën voor later staan als optionele milestones op het [projectbord](https://github.com/orgs/micro-botica/projects/1).

## Hardware

Het hele project draait op één bord, zonder extra sensoren of modules:

**[Waveshare ESP32-S3-Touch-LCD-2.1](https://docs.waveshare.com/ESP32-S3-Touch-LCD-2.1?variant=ESP32-S3-Touch-LCD-2.1)**

| Onderdeel | Specificatie |
|---|---|
| Microcontroller | ESP32-S3, dual core 240 MHz, wifi en Bluetooth LE |
| Geheugen | 16 MB flash, 8 MB PSRAM |
| Display | 2,1 inch rond IPS, 480x480, driver ST7701S |
| Touch | Capacitief, controller CST820 |
| Overig | RTC (PCF85063A), IMU (QMI8658), buzzer, microSD-slot |

Te koop bij onder andere [TinyTronics](https://www.tinytronics.nl/nl/development-boards/microcontroller-boards/met-wi-fi/waveshare-esp32-s3-met-2.1-inch-rond-ips-display-met-touchscreen-en-imu). De behuizing wordt 3D-geprint; de bestanden komen in `enclosure/`.

## Techniek

- **Framework:** Arduino via [PlatformIO](https://platformio.org/)
- **UI:** [LVGL](https://lvgl.io/), ook te draaien als simulator op de pc
- **Vliegtuigdata:** een gratis ADS-B-API (keuze volgt in de kick-off)
- **Weerdata:** keuze volgt in de kick-off

## Projectstructuur

```
flight-radar/
├── firmware/            # PlatformIO-project voor het bord
│   ├── src/
│   │   ├── data/        # wifi, API-clients, parsing
│   │   ├── ui/          # LVGL-schermen, radar, touch
│   │   └── model/       # Aircraft-model en geo-berekeningen
│   └── include/         # o.a. secrets.example.h
├── simulator/           # LVGL op de pc met nepdata
├── enclosure/           # 3D-printbestanden van de behuizing
└── docs/
    └── decisions.md     # waarom we welke keuzes maakten
```

Deze structuur wordt opgebouwd tijdens milestone 1.

## Aan de slag

De bouw- en flashinstructies volgen zodra het PlatformIO-project staat. In grote lijnen wordt het:

1. Installeer [VS Code](https://code.visualstudio.com/) met de PlatformIO-extensie.
2. Clone deze repository.
3. Kopieer `firmware/include/secrets.example.h` naar `secrets.h` en vul je wifi-gegevens in.
4. Sluit het bord aan via USB-C en klik op **Upload** in PlatformIO.

## Roadmap

We werken in milestones, die je terugvindt op het [projectbord](https://github.com/orgs/micro-botica/projects/1) en bij de [milestones](https://github.com/micro-botica/flight-radar/milestones?sort=title&direction=asc):

| # | Milestone | Inhoud |
|---|---|---|
| 0 | Kick-off | Databron, datamodel en taakverdeling kiezen |
| 1 | Hallo scherm | Display, touch en CI-build werkend |
| 2 | Data binnen | Wifi, vliegtuigdata en tijdsynchronisatie |
| 3 | Radarweergave | Vliegtuigen op de radar, eerst met nepdata |
| 4 | Klok en weer | Tijd en weer op het scherm |
| 5 | Interactie | Details, zoomen, kleuren en nachtmodus |
| 6 | Afwerking | Instelpagina, updates over wifi en behuizing |
| 7 t/m 14 | Optioneel | Ideeën voor doorontwikkeling |

## Samenwerken

- Elke wijziging gaat via een eigen branch en een pull request naar `main`.
- Koppel je PR aan een issue met `Closes #nr` in de beschrijving, dan schuift het issue op het bord vanzelf mee.
- De ander kijkt de PR na voordat hij gemerged wordt.
- Wifi-wachtwoorden en API-keys komen nooit in Git, alleen in je lokale `secrets.h`.

## Makers

Gemaakt door **Maarten** ([@Remotev2](https://github.com/Remotev2)) en **Mick** ([@MVelden1](https://github.com/MVelden1)), met hulp van [Claude](https://claude.ai) van Anthropic bij het opzetten, plannen en programmeren.

Geïnspireerd door het [ESP32 Plane Radar](https://makerworld.com/nl/models/2872376-esp32-plane-radar-live-ads-b-on-a-round-display)-project op MakerWorld.

## Licentie

[MIT](LICENSE)
