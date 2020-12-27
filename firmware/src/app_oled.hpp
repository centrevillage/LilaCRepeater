#ifndef LR_APP_OLED_H
#define LR_APP_OLED_H

#include <igb_stm32/periph/gpio.hpp>
#include <igb_stm32/periph/spi.hpp>
#include <igb_sdk/device/oled_ssd1306.hpp>

extern igb::sdk::OledSsd1306<igb::stm32::GpioPin, igb::stm32::Spi, 128, 64> app_oled;

#endif /* LR_APP_OLED_H */
