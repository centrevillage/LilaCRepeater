#ifndef LR_APP_OLED_H
#define LR_APP_OLED_H

#include <igb_stm32/periph/gpio.hpp>
#include <igb_stm32/periph/spi.hpp>
#include <igb_sdk/device/oled_ssd1306.hpp>

constexpr size_t app_oled_width = 128;
constexpr size_t app_oled_height = 64;
extern igb::sdk::OledSsd1306<igb::stm32::GpioPin, igb::stm32::Spi, app_oled_width, app_oled_height> app_oled;

#endif /* LR_APP_OLED_H */
