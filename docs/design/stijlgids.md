# Stijlgids Modern (Flight Radar display)

Bron: canvas "Flight Radar display", pagina "Uitwerking Modern". Scherm: rond IPS, 480 x 480 px, middelpunt (240, 240).

## Kleuren

| Naam | Hex | Gebruik |
|---|---|---|
| Achtergrond | `#0A0E13` | Achtergrond van alle schermen |
| Vlak | `#121922` | Kaarten, panelen, knoppen (secundair) |
| Vlak 2 | `#1A2330` | Badges, scheidingslijnen in lijsten, spoor van voortgangsboog |
| Lijn | `#1F2A36` | Binnenste afstandsringen, rand van ronde schermen |
| Lijn sterk | `#2E3B4A` | Buitenste ring (40 km), randen van knoppen en invoervelden |
| Tekst | `#E8EEF5` | Standaard tekst |
| Tekst wit | `#FFFFFF` | Grote cijfers, titels, actief paginabolletje |
| Tekst 2 | `#C9D3DE` | Ondertitels, datum, hoekinfo (aantal, tijd) |
| Gedempt | `#7C8B9C` | Labels, ringlabels, hulptekst |
| Inactief | `#3A4756` | Inactieve paginabolletjes |
| Accent | `#4CC9F0` | Sweep, noordpijl, thuispunt-ring, primaire knoppen, voortgang |
| Accent vlak | `#16303B` | Achtergrond van badge "Actief" en geselecteerde chips |
| Let op | `#FFC94A` | Waarschuwing (locatie geschat) |
| Let op vlak | `#2A2412` | Achtergrond waarschuwingspil |
| Fout | `#FF6B6B` | Geen wifi, verwijderen |
| Fout vlak | `#3A1A1D` | Achtergrond fout-icoon |
| Succes | `#7BE07B` | Verbonden, postcode gevonden |
| Succes vlak | `#10241C` | Achtergrond succesmelding |

Tekst op een accent- of foutknop: `#0A0E13`.

## Hoogteschaal (kleur van het vliegtuigicoon)

| Hoogte | Dag | Nacht |
|---|---|---|
| onder 1 km | `#FF8A3D` | `#8A5130` |
| 1 tot 3 km | `#FFC94A` | `#8A7436` |
| 3 tot 6 km | `#7BE07B` | `#467A49` |
| 6 tot 9 km | `#3FC8FF` | `#2C6B85` |
| boven 9 km | `#A99BFF` | `#5F5896` |

## Nachtmodus

| Naam | Hex |
|---|---|
| Achtergrond | `#000000` |
| Ring binnen | `#11161C` |
| Ring buiten | `#171E26` |
| Tekst (callsign, tijd) | `#8E97A3` |
| Gedempt (ringlabels, N) | `#6E7785` |
| Sweeplijn | `#1D3A4A` |
| Paginabolletje actief / inactief | `#4A5462` / `#1F262E` |

Nachtmodus: alleen ringen op 20 en 40 km, geen sweep-waaier (alleen de lijn), 14 s per omwenteling, labels alleen bij de 3 dichtstbijzijnde vliegtuigen, iconen 22 px.

## Lettertypen en maten

Space Grotesk voor cijfers en titels, Inter voor overige tekst. Kleinste tekst: 13 px.

| Lettertype | Maten (px) | Waar |
|---|---|---|
| Space Grotesk 600 | 116 | Klok (alleen cijfers en `:`) |
| Space Grotesk 600 | 96 | Temperatuur (cijfers, `°`, `-`) |
| Space Grotesk 600 | 46 | Callsign op detailkaart |
| Space Grotesk 600 | 28, 26, 24, 22, 21 | Statustitels, waarden op detailkaart, schermtitels |
| Space Grotesk 600 | 18 | N-indicator |
| Space Grotesk 500 | 17 | Aantal vliegtuigen en tijd in de hoeken |
| Inter 600 | 20, 19, 18, 16, 15, 14, 13 | Waarden, callsigns (15 op radar, 18 in lijst), knoppen, badges |
| Inter 500 | 21, 19, 18, 15, 14, 13 | Datum, weeromschrijving, ondertitels, ringlabels (13) |

Voorstel om flash te sparen in LVGL (mag samengevoegd worden):

- Space Grotesk 600: 116 en 96 (alleen `0-9 : ° -`), 46, 26, 22, 18
- Space Grotesk 500: 17
- Inter 600: 20, 18, 15, 13
- Inter 500: 21, 15, 13

Bronnen van de fonts: Google Fonts (Inter, Space Grotesk), beide onder de SIL Open Font License.

## Afmetingen en afspraken

- Alle inhoud binnen een straal van 200 px vanaf het midden.
- De rand tussen 200 en 240 px is alleen voor: N-indicator (boven), aantal vliegtuigen (linksboven), tijd (rechtsboven) en paginabolletjes (onder).
- Raakvlakken minimaal 48 x 48 px. Bij overlap op de radar kiest een tik het dichtstbijzijnde vliegtuig.
- Lijndikte ringen: 1,5 px. Hoekradius kaarten 18 px, panelen 20 tot 22 px, knoppen als pil (radius = halve hoogte).
- Knophoogte op het ronde scherm: 48 px (statusknoppen 52 px).

## Radarscherm

- Afstandsringen op straal 50, 100, 150 en 200 px (10, 20, 30 en 40 km bij bereik 40 km, dus 5 px per km). Ringen 1 tot 3 in Lijn, ring 4 in Lijn sterk.
- Ringlabels "10", "20", "30", "40 km" op de westas, net boven de horizontale lijn (y = 232), in Gedempt, Inter 500 13 px.
- Noord: "N" boven in het midden (y = 8), daaronder een driehoekje van 12 x 9 px in Accent op y = 33.
- Thuispunt: witte stip 8 px met een ring van 20 px, 2 px dik, in Accent.
- Sweep: lijn van het midden naar straal 200, 1,5 px in Accent, plus twee waaiers achter de lijn: 30° met 7% dekking en 12° met 12% dekking. 6 s per omwenteling.
- Vliegtuigicoon: 26 px (22 px zonder label), gedraaid naar de vliegrichting, gevuld met de hoogtekleur.
- Callsign: Inter 600 15 px in Tekst, 16 px rechts van het icoon. Rechts van x = 330 komt het label links van het icoon (rechts uitgelijnd).
- Bij veel verkeer krijgen alleen de 6 dichtstbijzijnde vliegtuigen een label.
- Hoekinfo: aantal met klein vliegtuigje op (62, 73), tijd op (368, 73), Space Grotesk 500 17 px in Tekst 2.

## Paginabolletjes

- Drie raakvlakken van 48 x 48 px naast elkaar, links boven op (168, 428).
- Actief: pil van 20 x 8 px, radius 4, wit. Inactief: stip van 8 px in Inactief.
- Volgorde: radar, klok, weer.
