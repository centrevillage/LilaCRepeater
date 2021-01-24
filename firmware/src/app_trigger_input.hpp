#ifndef LR_APP_TRIGGER_INPUT_H
#define LR_APP_TRIGGER_INPUT_H

#include <functional>

#include <igb_stm32/periph/gpio.hpp>
#include <igb_daisy/periph/gpio.hpp>

using namespace igb::stm32;
using namespace igb::daisy;

enum class AppTrigId : uint8_t {
  sync = 0,
  run,
  reset,
  rec,
  ext_sync_sw
};

constexpr uint8_t trigger_count = 5;

struct AppTriggerInput {
  std::array<GpioPin, trigger_count> pins = {
    // sync
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p10)),
    // run
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p1)),
    // reset
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p23)),
    // rec
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p24)),
    // ext_sync_sw
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p25)),
  };

  uint8_t trig_bits = 0;

  std::function<void(AppTrigId, bool)> on_change;

  inline void init() {
    for (auto& pin : pins) {
      pin.enable();
      pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);
    }
  }

  inline bool isOn(AppTrigId id) {
    return !pins[static_cast<uint8_t>(id)].read();
  }

  // TODO: use EXTI for trigger action
  inline void process() {
    uint8_t prev_trig_bits = trig_bits;
    trig_bits = 0;
    for (uint8_t i = 0; i < trigger_count; ++i) {
      AppTrigId id = static_cast<AppTrigId>(i);
      trig_bits |= ((uint8_t)(isOn(id) ? 1 : 0) << i);
      //trig_bits = (trig_bits & ~((uint8_t)i << i)) | ((uint8_t)(isOn(id) ? 1 : 0) << i);
    }
    if (on_change) {
      uint8_t diff_bits = trig_bits ^ prev_trig_bits;
      for (uint8_t i = 0; i < trigger_count; ++i) {
        if (diff_bits & ((uint8_t)1 << i)) {
          on_change(static_cast<AppTrigId>(i), !!(trig_bits & ((uint8_t)1 << i)));
        }
      }
    }
  }
};

extern AppTriggerInput triggers;

#endif /* LR_APP_TRIGGER_INPUT_H */
