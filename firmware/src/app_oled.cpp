#include "app_oled.hpp"
#include <igb_daisy/periph/gpio.hpp>

using namespace igb::stm32;
using namespace igb::sdk;
using namespace igb::daisy;

OledSsd1306<GpioPin, Spi, 128, 64> app_oled = {
  .cs_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p12)),
  .dc_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p8)),
  .reset_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p13)),
  .spi = Spi::newSpi(SpiType::spi1)
};
