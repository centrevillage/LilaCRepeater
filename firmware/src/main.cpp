#include <stdio.h>
#include <string.h>
#include "daisy_seed.h"

#include <igb_stm32/base.hpp>
#include <igb_stm32/periph/systick.hpp>
#include <igb_stm32/periph/gpio.hpp>
#include <igb_stm32/periph/spi.hpp>
#include <igb_sdk/device/oled_ssd1306.hpp>
#include <igb_daisy/periph/gpio.hpp>

#include <igb_stm32/periph/flash.hpp>
#include <igb_stm32/periph/nvic.hpp>
#include <igb_stm32/periph/rcc.hpp>
#include <igb_stm32/periph/tim.hpp>
#include <igb_sdk/util/text.h>

#include "stm32h7xx_ll_spi.h"
#include "stm32h7xx_ll_gpio.h"

using namespace daisy;
using namespace igb::stm32;
using namespace igb::sdk;
using namespace igb::daisy;

DaisySeed hardware;

OledSsd1306<GpioPin, Spi, 128, 64> ssd1306 = {
  .cs_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p12)),
  .dc_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p8)),
  .reset_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p13)),
  .spi = Spi::newSpi(SpiType::spi1)
};

GpioPin test_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p1));
GpioPin state_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p3));
//static dsy_gpio test_pin;
GpioPin btn_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p2));

bool is_dirty = true;
char text_buf[128];

enum {
  SLIDER_DRY_VOL = 0,
  SLIDER_TRACK_VOL,
  SLIDER_PAN,
  SLIDER_LENGTH,
  SLIDER_POS,
  SLIDER_FDBK,
  SLIDER_SPEED,

  SLIDER_COUNT
};

float slider_values[SLIDER_COUNT];

// Daisyのピン番号が0 originなのでハードウェアのGPIO番号-1で定義する
#define SLIDER_DRY_VOL_PIN 22-1
#define SLIDER_TRACK_VOL_PIN 21-1
#define SLIDER_PAN_PIN 20-1
#define SLIDER_LENGTH_PIN 19-1
#define SLIDER_POS_PIN 18-1
#define SLIDER_FDBK_PIN 17-1
#define SLIDER_SPEED_PIN 16-1

#define OLED_CS_PIN 12-1
#define OLED_DC_PIN 8-1
#define OLED_RST_PIN 13-1

static inline void setup_oled() {
  ssd1306.spi.prepareSpiMasterOutOnly(
      daisy_pin_to_stm32_pin(DaisyGpioPinType::p11) /* MOSI pin */,
      daisy_pin_to_stm32_pin(DaisyGpioPinType::p9) /* SCK pin */,
      SpiBaudratePrescaler::DIV2);

  ssd1306.init();
  ssd1306.drawFillBG();
  ssd1306.drawTextMedium("LilaC Repeater", 14, 3, 0);
  ssd1306.process();
}

static inline void setup_adc() {
  //This is our ADC configuration
  AdcChannelConfig adcConfigs[SLIDER_COUNT];
  adcConfigs[SLIDER_DRY_VOL].InitSingle(hardware.GetPin(SLIDER_DRY_VOL_PIN));
  adcConfigs[SLIDER_TRACK_VOL].InitSingle(hardware.GetPin(SLIDER_TRACK_VOL_PIN));
  adcConfigs[SLIDER_PAN].InitSingle(hardware.GetPin(SLIDER_PAN_PIN));
  adcConfigs[SLIDER_LENGTH].InitSingle(hardware.GetPin(SLIDER_LENGTH_PIN));
  adcConfigs[SLIDER_POS].InitSingle(hardware.GetPin(SLIDER_POS_PIN));
  adcConfigs[SLIDER_FDBK].InitSingle(hardware.GetPin(SLIDER_FDBK_PIN));
  adcConfigs[SLIDER_SPEED].InitSingle(hardware.GetPin(SLIDER_SPEED_PIN));

  //Initialize the adc with the config we just made
  hardware.adc.Init(adcConfigs, SLIDER_COUNT);
  //Start reading values
  hardware.adc.Start();
}

static inline void setup() {
  hardware.Configure();
  hardware.Init();

  test_pin.enable();
  test_pin.initOutputDefault();
  test_pin.on();

  state_pin.enable();
  state_pin.initOutputDefault();
  state_pin.off();

  btn_pin.enable();
  btn_pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);

  setup_adc();
  setup_oled();
}

static inline void loop() {
  for (uint8_t i = 0; i < SLIDER_COUNT; ++i) {
    // TODO: LPFかけて読み出すべき？
    float value = hardware.adc.GetFloat(i);
    if (slider_values[i] != value) {
      slider_values[i] = value;
      is_dirty = true;
    }
  }
  if (is_dirty) {
    ssd1306.drawFillBG();
    text_from_uint16(text_buf, (uint16_t)(slider_values[SLIDER_DRY_VOL] * 1024.0f));
    ssd1306.drawTextMedium(text_buf, 6, 2, 0);
    ssd1306.process();
    is_dirty = false;
  }

  test_pin.write(!btn_pin.read());
}

int main(void) {
  setup();
  for(;;) {
    loop();
    //wait 1 ms
    dsy_system_delay(1);
  }
}
