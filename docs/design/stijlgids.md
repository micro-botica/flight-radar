# Stijlgids Modern (Flight Radar display)

Versie 2, 17 november 2026. Bron: canvas "Flight Radar display", pagina "Uitwerking Modern". Scherm: rond IPS, 480 x 480 px, middelpunt (240, 240), 0,11 mm per pixel.

## Wat is er veranderd sinds versie 1

- Radar verkleind: buitenste ring op straal 180 px in plaats van 200 px. Dat is 4,5 px per km bij een bereik van 40 km.
- Kleinste tekst is 15 px in plaats van 13 px. Callsigns op de radar zijn 18 px, tijd en aantal 20 px.
- Kleine tekst is altijd Inter 600. Inter 500 wordt alleen nog voor de datum op de klok gebruikt.
- Lijnen zijn hele pixels: binnenringen 1 px, buitenring 2 px, sweeplijn 2 px. Alle ringen gebruiken de kleur Lijn sterk.
- Ringlabels staan links van de noordas in plaats van op de westas. De ring van 10 km heeft geen label meer.
- Op de radar krijgen maximaal 4 vliegtuigen een callsign (was 6).
- Geen gestippelde cirkel meer bij "Locatie geschat": het is een doorgetrokken ring van 2 px.
- Instellingen: het locatieprofiel kies je met pijltjes, zodat het voor elk aantal profielen werkt.

## Kleuren

| Naam | Hex | Gebruik |
|---|---|---|
| Achtergrond | `#0A0E13` | Achtergrond van alle schermen |
| Vlak | `#121922` | Kaarten, panelen, ronde knoppen |
| Vlak 2 | `#1A2330` | Badges, scheidingslijnen in lijsten, spoor van voortgangsboog |
| Lijn | `#1F2A36` | Spoor van schuifbalk en helderheidsbalk |
| Lijn sterk | `#2E3B4A` | Alle afstandsringen, randen van knoppen |
| Tekst | `#E8EEF5` | Standaard tekst |
| Tekst wit | `#FFFFFF` | Grote cijfers, titels, actief paginabolletje |
| Tekst 2 | `#C9D3DE` | Ondertitels, datum, tijd en aantal op de radar |
| Gedempt | `#7C8B9C` | Labels, ringlabels, hulptekst |
| Inactief | `#3A4756` | Inactieve paginabolletjes |
| Accent | `#4CC9F0` | Sweep, noordpijl, thuispunt-ring, primaire knoppen, voortgang |
| Accent vlak | `#16303B` | Gekozen locatieprofiel |
| Let op | `#FFC94A` | Waarschuwing (locatie geschat) |
| Let op vlak | `#2A2412` | Achtergrond waarschuwingspil |
| Fout | `#FF6B6B` | Geen wifi, verwijderen |
| Succes | `#7BE07B` | Verbonden, postcode gevonden |

Tekst op een accentknop: `#0A0E13`.

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
| Ringen | `#171E26` |
| Tekst (callsign, tijd) | `#8E97A3` |
| Gedempt (ringlabels, N, maan) | `#6E7785` |
| Sweeplijn | `#1D3A4A` |
| Paginabolletje actief / inactief | `#4A5462` / `#1F262E` |

Nachtmodus: alleen de ringen van 20 en 40 km, geen sweep-waaier (alleen de lijn), 14 s per omwenteling, callsigns alleen bij de 3 dichtstbijzijnde vliegtuigen, iconen 22 px. Linksboven staat een maantje in plaats van het aantal vliegtuigen.

## Lettertypen en maten

Space Grotesk voor cijfers en titels, Inter voor overige tekst. Kleinste tekst: 15 px. Beide fonts komen van Google Fonts (SIL Open Font License).

| Lettertype | Maat (px) | Waar |
|---|---|---|
| Space Grotesk 600 | 116 | Klok (alleen cijfers en `:`) |
| Space Grotesk 600 | 96 | Temperatuur (cijfers, `°`, `-`) |
| Space Grotesk 600 | 46 | Callsign op detailkaart |
| Space Grotesk 600 | 26 | Statustitels, waarden op detailkaart |
| Space Grotesk 600 | 22 | Schermtitels (Instellingen, In de buurt) |
| Space Grotesk 600 | 20 | Afstand in de lijst |
| Space Grotesk 600 | 18 | N-indicator |
| Space Grotesk 500 | 20 | Tijd en aantal vliegtuigen op de radar |
| Inter 600 | 20 | Waarden op het weerscherm |
| Inter 600 | 18 | Callsigns (radar en lijst), maatschappij, profielnaam |
| Inter 600 | 16 | Knoppen |
| Inter 600 | 15 | Labels, ringlabels, ondertitels, hulptekst |
| Inter 500 | 21 | Datum op de klok |

In het ontwerp komen ook 28, 24, 21, 19 en 17 px voor. Die mogen in de firmware naar de dichtstbijzijnde maat uit deze tabel, om flash te sparen.

## Afmetingen en afspraken

- De radar valt binnen een straal van 180 px vanaf het midden.
- De rand tussen 180 en 240 px is alleen voor: N-indicator (boven), aantal vliegtuigen (linksboven), tijd (rechtsboven) en paginabolletjes (onder).
- Houd minimaal 10 px afstand tot de rand van het ronde scherm.
- Raakvlakken minimaal 48 x 48 px. Bij overlap op de radar kiest een tik het dichtstbijzijnde vliegtuig.
- Hoekradius kaarten 18 px, panelen 20 tot 22 px, knoppen als pil (radius = halve hoogte).
- Knophoogte op het ronde scherm: 48 px.

## Radarscherm

- Afstandsringen op straal 45, 90, 135 en 180 px (10, 20, 30 en 40 km bij bereik 40 km). Ringen 1 tot 3: 1 px. Ring 4: 2 px. Kleur: Lijn sterk.
- Ringlabels "20", "30" en "40 km": Inter 600 15 px in Gedempt, rechts uitgelijnd tegen x = 234 (net links van de noordas), bovenkant op y = 153, 108 en 64.
- Noord: "N" in Space Grotesk 600 18 px, wit, gecentreerd, bovenkant y = 24. Daaronder een driehoekje van 12 x 9 px in Accent, bovenkant y = 50.
- Thuispunt: witte stip 8 px met een ring van 20 px doorsnee, 2 px dik, in Accent.
- Sweep: lijn van het midden naar straal 180, 2 px in Accent, plus twee waaiers achter de lijn: 30° met 7% dekking en 12° met 12% dekking. 6 s per omwenteling.
- Vliegtuigicoon: 26 px met callsign, 22 px zonder. Gedraaid naar de vliegrichting, gevuld met de hoogtekleur.
- Callsign: Inter 600 18 px in Tekst, 16 px rechts van het midden van het icoon. Staat het icoon op x = 320 of verder naar rechts, dan komt het label links (rechts uitgelijnd).
- Maximaal 4 callsigns tegelijk: de 4 dichtstbijzijnde vliegtuigen. Verberg een label als het een ander label of icoon raakt.
- Aantal vliegtuigen: vak van 60 x 24 px op (65, 83), icoon 16 px plus getal. Tijd: vak van 60 x 24 px op (355, 83). Beide Space Grotesk 500 20 px in Tekst 2, gecentreerd in het vak.

## Paginabolletjes

- Drie raakvlakken van 48 x 48 px naast elkaar, linksboven op (168, 428).
- Actief: pil van 20 x 8 px, radius 4, wit. Inactief: stip van 8 px in Inactief.
- Volgorde: radar, klok, weer.

## Overige schermen

- Detailkaart: vier vakken van 153 x 92 px in een raster van 2 x 2, tussenruimte 10 px, linksboven op (82, 192). Sluitknop: ronde knop van 48 px in Vlak, bovenaan in het midden.
- Lijst: precies 5 rijen van 60 px hoog, 320 px breed, vanaf y = 114. Schuifbalk als boog langs de rechterrand.
- Instellingen: locatieprofiel met pijltjes links en rechts (48 px) en de naam ertussen. Bereik: keuze uit 20, 40 en 80 km. Helderheid: min en plus (48 px) met balk van 10 px hoog.
- Klok: secondenboog op straal 200 px, 4 px dik, zonder losse stip.

## Aandachtspunten voor LVGL

- Lijndiktes zijn hele pixels. Gebruik geen 1,5 px.
- Een boog kan niet gestippeld worden. Gebruik doorgetrokken lijnen.
- De sweep-waaier is een boog met de dikte van de straal en een lage dekking. Hij ververst een groot deel van het scherm. Reken op ongeveer 10 stappen per seconde, dit is nog niet gemeten.
- De grote fonts (116 en 96 px) hebben alleen cijfers en een paar tekens nodig.

## Afbeeldingen van de schermen

De ontwerpen staan als PNG en GIF in `docs/design/schermen/`. Het zijn ontwerpen en geen foto's van het apparaat.
