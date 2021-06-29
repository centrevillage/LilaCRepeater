#include "app_oled.hpp"
#include <igb_daisy/periph/gpio.hpp>

using namespace igb::stm32;
using namespace igb::sdk;
using namespace igb::daisy;

OledSsd1306<AppSpiType, GpioPin, 128, 64> app_oled = {
  // hardware ver 0.2 ~ 
#if 1
  .cs_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p8)),
  .dc_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p12)),
  .reset_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p13)),
#endif
  // hardware ver 0.1
#if 0
  .cs_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p12)),
  .dc_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p8)),
  .reset_pin = GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p13)),
#endif
};
