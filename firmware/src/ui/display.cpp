#include "display.h"

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include <esp_heap_caps.h>
#include <lvgl.h>

namespace {

// I2C bus, shared by the I/O expander, touch controller, RTC and IMU.
constexpr int kPinI2cSda = 15;
constexpr int kPinI2cScl = 7;

// TCA9554 I/O expander. Bit numbers are 0 based (EXIO1 is bit 0).
constexpr uint8_t kExpanderAddress = 0x20;
constexpr int8_t kExpanderLcdReset = 0;  // EXIO1
constexpr int8_t kExpanderLcdCs = 2;     // EXIO3

// The ST7701S is configured over a 3-wire SPI bus, bit-banged through the
// expander. The pixel data itself goes over the parallel RGB bus below.
constexpr int8_t kPinLcdSck = 2;
constexpr int8_t kPinLcdMosi = 1;

constexpr int kPinBacklight = 6;
constexpr int kBacklightChannel = 1;
constexpr int kBacklightFrequencyHz = 20000;
constexpr int kBacklightResolutionBits = 10;

// LVGL draw buffers: 60 lines of 480 pixels, two of them, kept in PSRAM.
constexpr uint32_t kDrawBufferLines = 60;
constexpr uint32_t kDrawBufferPixels = kDisplayWidth * kDrawBufferLines;

// Register setup for the ST7701S of this panel (from the Waveshare reference
// design). Command 0xFF selects the register bank: 0x10 is bank 0 (command
// 2), 0x11 is bank 1, 0x13 is bank 3 and 0x00 is the normal command set.
const uint8_t kSt7701InitOperations[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x10,

    WRITE_C8_D16, 0xC0, 0x3B, 0x00,
    WRITE_C8_D16, 0xC1, 0x0B, 0x02,
    WRITE_C8_D16, 0xC2, 0x07, 0x02,
    WRITE_C8_D8, 0xCC, 0x10,
    WRITE_C8_D8, 0xCD, 0x08,

    WRITE_COMMAND_8, 0xB0,  // Positive gamma
    WRITE_BYTES, 16,
    0x00, 0x11, 0x16, 0x0E, 0x11, 0x06, 0x05, 0x09,
    0x08, 0x21, 0x06, 0x13, 0x10, 0x29, 0x31, 0x18,

    WRITE_COMMAND_8, 0xB1,  // Negative gamma
    WRITE_BYTES, 16,
    0x00, 0x11, 0x16, 0x0E, 0x11, 0x07, 0x05, 0x09,
    0x09, 0x21, 0x05, 0x13, 0x11, 0x2A, 0x31, 0x18,

    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x11,

    WRITE_C8_D8, 0xB0, 0x6D,  // VOP
    WRITE_C8_D8, 0xB1, 0x37,  // VCOM
    WRITE_C8_D8, 0xB2, 0x81,  // VGH
    WRITE_C8_D8, 0xB3, 0x80,
    WRITE_C8_D8, 0xB5, 0x43,  // VGL
    WRITE_C8_D8, 0xB7, 0x85,
    WRITE_C8_D8, 0xB8, 0x20,
    WRITE_C8_D8, 0xC1, 0x78,
    WRITE_C8_D8, 0xC2, 0x78,
    WRITE_C8_D8, 0xD0, 0x88,

    WRITE_COMMAND_8, 0xE0,
    WRITE_BYTES, 3, 0x00, 0x00, 0x02,

    WRITE_COMMAND_8, 0xE1,
    WRITE_BYTES, 11,
    0x03, 0xA0, 0x00, 0x00, 0x04, 0xA0, 0x00, 0x00, 0x00, 0x20, 0x20,

    WRITE_COMMAND_8, 0xE2,
    WRITE_BYTES, 13,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00,

    WRITE_COMMAND_8, 0xE3,
    WRITE_BYTES, 4, 0x00, 0x00, 0x11, 0x00,

    WRITE_C8_D16, 0xE4, 0x22, 0x00,

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 16,
    0x05, 0xEC, 0xA0, 0xA0, 0x07, 0xEE, 0xA0, 0xA0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    WRITE_COMMAND_8, 0xE6,
    WRITE_BYTES, 4, 0x00, 0x00, 0x11, 0x00,

    WRITE_C8_D16, 0xE7, 0x22, 0x00,

    WRITE_COMMAND_8, 0xE8,
    WRITE_BYTES, 16,
    0x06, 0xED, 0xA0, 0xA0, 0x08, 0xEF, 0xA0, 0xA0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    WRITE_COMMAND_8, 0xEB,
    WRITE_BYTES, 7, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00,

    WRITE_COMMAND_8, 0xED,
    WRITE_BYTES, 16,
    0xFF, 0xFF, 0xFF, 0xBA, 0x0A, 0xBF, 0x45, 0xFF,
    0xFF, 0x54, 0xFB, 0xA0, 0xAB, 0xFF, 0xFF, 0xFF,

    WRITE_COMMAND_8, 0xEF,
    WRITE_BYTES, 6, 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F,

    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x13,

    WRITE_C8_D8, 0xEF, 0x08,

    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 5, 0x77, 0x01, 0x00, 0x00, 0x00,

    WRITE_C8_D8, 0x36, 0x00,  // Memory access control
    WRITE_C8_D8, 0x3A, 0x66,  // 18 bit pixels on the RGB bus

    WRITE_COMMAND_8, 0x11,  // Sleep out
    END_WRITE,
    DELAY, 120,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x20,  // Display inversion off
    END_WRITE,
    DELAY, 120,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x29,  // Display on
    END_WRITE,
};

Arduino_DataBus* bus = nullptr;
Arduino_ESP32RGBPanel* rgbPanel = nullptr;
Arduino_RGB_Display* gfx = nullptr;

lv_disp_draw_buf_t drawBuf;
lv_disp_drv_t dispDrv;

void flushToPanel(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* pixels) {
  const uint32_t w = area->x2 - area->x1 + 1;
  const uint32_t h = area->y2 - area->y1 + 1;
  gfx->draw16bitRGBBitmap(area->x1, area->y1,
                          reinterpret_cast<uint16_t*>(pixels), w, h);
  lv_disp_flush_ready(drv);
}

}  // namespace

bool displayInit() {
  Wire.begin(kPinI2cSda, kPinI2cScl);

  bus = new Arduino_XCA9554SWSPI(kExpanderLcdReset, kExpanderLcdCs, kPinLcdSck,
                                 kPinLcdMosi, &Wire, kExpanderAddress);

  // Pin order: DE, VSYNC, HSYNC, PCLK, R0-R4, G0-G5, B0-B4.
  rgbPanel = new Arduino_ESP32RGBPanel(
      40 /* DE */, 39 /* VSYNC */, 38 /* HSYNC */, 41 /* PCLK */,
      46 /* R0 */, 3 /* R1 */, 8 /* R2 */, 18 /* R3 */, 17 /* R4 */,
      14 /* G0 */, 13 /* G1 */, 12 /* G2 */, 11 /* G3 */, 10 /* G4 */,
      9 /* G5 */,
      5 /* B0 */, 45 /* B1 */, 48 /* B2 */, 47 /* B3 */, 21 /* B4 */,
      1 /* hsync_polarity */, 50 /* hsync_front_porch */,
      8 /* hsync_pulse_width */, 10 /* hsync_back_porch */,
      1 /* vsync_polarity */, 8 /* vsync_front_porch */,
      3 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
      0 /* pclk_active_neg */, 16000000 /* prefer_speed */,
      false /* useBigEndian */, 0 /* de_idle_high */, 0 /* pclk_idle_high */,
      kDisplayWidth * 10 /* bounce_buffer_size_px */);

  gfx = new Arduino_RGB_Display(
      kDisplayWidth, kDisplayHeight, rgbPanel, 0 /* rotation */,
      true /* auto_flush */, bus, GFX_NOT_DEFINED /* RST, done by the bus */,
      kSt7701InitOperations, sizeof(kSt7701InitOperations));

  if (!gfx->begin()) {
    Serial.println("Display: panel init failed");
    return false;
  }
  gfx->fillScreen(0x0000);

  // Backlight PWM. Brightness 0 until the first frame is drawn.
  ledcSetup(kBacklightChannel, kBacklightFrequencyHz, kBacklightResolutionBits);
  ledcAttachPin(kPinBacklight, kBacklightChannel);
  displaySetBacklight(0);

  lv_init();

  const size_t bufBytes = kDrawBufferPixels * sizeof(lv_color_t);
  auto* buf1 = static_cast<lv_color_t*>(
      heap_caps_malloc(bufBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  auto* buf2 = static_cast<lv_color_t*>(
      heap_caps_malloc(bufBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  if (buf1 == nullptr || buf2 == nullptr) {
    Serial.println("Display: could not allocate LVGL buffers in PSRAM");
    return false;
  }
  lv_disp_draw_buf_init(&drawBuf, buf1, buf2, kDrawBufferPixels);

  lv_disp_drv_init(&dispDrv);
  dispDrv.hor_res = kDisplayWidth;
  dispDrv.ver_res = kDisplayHeight;
  dispDrv.flush_cb = flushToPanel;
  dispDrv.draw_buf = &drawBuf;
  lv_disp_drv_register(&dispDrv);

  return true;
}

void displaySetBacklight(uint8_t percent) {
  if (percent > 100) {
    percent = 100;
  }
  constexpr uint32_t kMaxDuty = (1u << kBacklightResolutionBits) - 1;
  ledcWrite(kBacklightChannel, kMaxDuty * percent / 100);
}

void displayUpdate() {
  lv_timer_handler();
}
