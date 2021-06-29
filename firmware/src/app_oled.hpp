#ifndef LR_APP_OLED_H
#define LR_APP_OLED_H

#include <igb_stm32/periph/gpio.hpp>
#include <igb_stm32/periph/spi.hpp>
#include <igb_sdk/device/oled_ssd1306.hpp>
#include <igb_daisy/periph/gpio.hpp>

constexpr size_t app_oled_width = 128;
constexpr size_t app_oled_height = 64;
typedef igb::stm32::Spi<
  igb::stm32::SpiType::spi1,
  igb::daisy::daisy_pin_to_stm32_pin(igb::daisy::DaisyGpioPinType::p11),
  igb::daisy::daisy_pin_to_stm32_pin(igb::daisy::DaisyGpioPinType::p10),
  igb::daisy::daisy_pin_to_stm32_pin(igb::daisy::DaisyGpioPinType::p9)> AppSpiType;
extern igb::sdk::OledSsd1306<AppSpiType, igb::stm32::GpioPin, app_oled_width, app_oled_height> app_oled;

#endif /* LR_APP_OLED_H */
