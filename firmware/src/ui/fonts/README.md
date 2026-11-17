# Fonts

LVGL fonts generated from Inter and Space Grotesk (Google Fonts, SIL Open Font License, see `OFL-Inter.txt` and `OFL-SpaceGrotesk.txt`). Do not edit the `.c` files by hand.

Sizes follow the font table in `docs/design/stijlgids.md` (version 2). Only these sizes are included:

| Font | Size (px) | Glyphs |
|---|---|---|
| Space Grotesk 600 | 116, 96 | `0x2D,0x30-0x3A,0xB0` (`-`, `0-9`, `:`, degree sign) |
| Space Grotesk 600 | 46, 26, 22, 20, 18 | Latin |
| Space Grotesk 500 | 20 | Latin |
| Inter 600 | 20, 18, 16, 15 | Latin |
| Inter 500 | 21 | Latin |

Latin is `0x20-0x7E,0xB0,0xB7,0xC0-0xFF`. All fonts are 4 bpp and uncompressed (`LV_USE_FONT_COMPRESSED` is 0).

## Regenerate

1. Download the variable fonts from https://github.com/google/fonts (`ofl/inter`, `ofl/spacegrotesk`).
2. Make static instances with fonttools:
   `fonttools varLib.instancer Inter[opsz,wght].ttf wght=600 opsz=14 -o Inter-600.ttf`
   `fonttools varLib.instancer Inter[opsz,wght].ttf wght=500 opsz=14 -o Inter-500.ttf`
   `fonttools varLib.instancer SpaceGrotesk[wght].ttf wght=600 -o SG-600.ttf`
   `fonttools varLib.instancer SpaceGrotesk[wght].ttf wght=500 -o SG-500.ttf`
3. Convert with `lv_font_conv` 1.5.3 (npm), for example:
   `lv_font_conv --font Inter-600.ttf -r 0x20-0x7E,0xB0,0xB7,0xC0-0xFF --size 15 --bpp 4 --no-compress --format lvgl --lv-include lvgl.h --lv-font-name font_inter_600_15 -o font_inter_600_15.c`
   `lv_font_conv --font SG-600.ttf -r 0x2D,0x30-0x3A,0xB0 --size 116 --bpp 4 --no-compress --format lvgl --lv-include lvgl.h --lv-font-name font_space_grotesk_600_116 -o font_space_grotesk_600_116.c`
4. Declare a new font in `firmware/src/ui/theme.h`.
