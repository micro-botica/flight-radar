# Fonts

LVGL fonts generated from Inter and Space Grotesk (Google Fonts, SIL Open Font License, see `OFL-Inter.txt` and `OFL-SpaceGrotesk.txt`). Do not edit the `.c` files by hand.

Sizes and glyphs follow `docs/design/stijlgids.md`. Latin text uses `0x20-0x7E,0xB0,0xB7,0xC0-0xFF`. The 116 and 96 px fonts use `0x20,0x2D,0x30-0x3A,0xB0` (space, `-`, `0-9`, `:`, degree sign). All fonts are 4 bpp and uncompressed (`LV_USE_FONT_COMPRESSED` is 0).

## Regenerate

1. Download the variable fonts from https://github.com/google/fonts (`ofl/inter`, `ofl/spacegrotesk`).
2. Make static instances with fonttools, for example:
   `fonttools varLib.instancer Inter[opsz,wght].ttf wght=600 opsz=14 -o Inter-600.ttf`
   `fonttools varLib.instancer SpaceGrotesk[wght].ttf wght=600 -o SG-600.ttf`
3. Convert with `lv_font_conv` (npm):
   `lv_font_conv --font Inter-600.ttf -r 0x20-0x7E,0xB0,0xB7,0xC0-0xFF --size 15 --bpp 4 --no-compress --format lvgl --lv-include lvgl.h --lv-font-name font_inter_600_15 -o font_inter_600_15.c`
4. Declare a new font in `firmware/src/ui/theme.h`.
