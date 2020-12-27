#ifndef LR_APP_BUTTONS_H
#define LR_APP_BUTTONS_H

#include <igb_stm32/periph/gpio.hpp>
#include <igb_daisy/periph/gpio.hpp>
#include <igb_sdk/ui/button.hpp>
#include <functional>
#include <array>

using namespace igb::stm32;
using namespace igb::sdk;
using namespace igb::daisy;

enum class AppBtnID : uint8_t {
  track1 = 0,
  track2,
  track3,
  track4,
  mute,
  rec,
  clear,
  rev,
  run,

  size
};

struct AppButtons {
  uint16_t state_bits = 0;
  std::function<void(AppBtnID, bool)> on_change;

  // TODO: button matrix (fix hardware)
  ButtonSingle<GpioPin> mute_btn {
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p25))
  };
  ButtonSingle<GpioPin> rec_btn {
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p27))
  };
  ButtonSingle<GpioPin> clear_btn {
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p28))
  };
  ButtonSingle<GpioPin> rev_btn {
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p31))
  };
  ButtonSingle<GpioPin, false> run_btn {
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p14))
  }; // pull down
  ButtonSingle<GpioPin> track1_btn {
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p1))
  };
  ButtonSingle<GpioPin> track2_btn {
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p26))
  };
  ButtonSingle<GpioPin> track3_btn {
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p29))
  };
  ButtonSingle<GpioPin> track4_btn {
    GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::p30))
  };

  void init() {
    mute_btn.pin.enable();
    mute_btn.pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);

    rec_btn.pin.enable();
    rec_btn.pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);

    clear_btn.pin.enable();
    clear_btn.pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);

    rev_btn.pin.enable();
    rev_btn.pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);

    run_btn.pin.enable();
    run_btn.pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);

    track1_btn.pin.enable();
    track1_btn.pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);

    track2_btn.pin.enable();
    track2_btn.pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);

    track3_btn.pin.enable();
    track3_btn.pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);

    track4_btn.pin.enable();
    track4_btn.pin.initInput(GpioPullMode::UP, GpioSpeedMode::HIGH);
  }

  bool isOn(AppBtnID btn_id) {
    bool on = false;

    switch (btn_id) {
      case AppBtnID::track1:
        on = track1_btn.isOn();
        break;
      case AppBtnID::track2:
        on = track2_btn.isOn();
        break;
      case AppBtnID::track3:
        on = track3_btn.isOn();
        break;
      case AppBtnID::track4:
        on = track4_btn.isOn();
        break;
      case AppBtnID::mute:
        on = mute_btn.isOn();
        break;
      case AppBtnID::rec:
        on = rec_btn.isOn();
        break;
      case AppBtnID::clear:
        on = clear_btn.isOn();
        break;
      case AppBtnID::rev:
        on = rev_btn.isOn();
        break;
      case AppBtnID::run:
        on = run_btn.isOn();
        break;
      default:
        break;
    }

    return on;
  }

  bool process() {
    track1_btn.process();
    track2_btn.process();
    track3_btn.process();
    track4_btn.process();
    mute_btn.process();
    rec_btn.process();
    clear_btn.process();
    rev_btn.process();
    run_btn.process();

    uint16_t new_bits = 0;
    for (uint8_t i = 0; i < static_cast<uint8_t>(AppBtnID::size); ++i) {
      if (isOn(static_cast<AppBtnID>(i))) {
        new_bits |= (uint16_t)1 << i;
      }
    }
    if (state_bits != new_bits) {
      if (on_change) {
        uint16_t diff_bits = state_bits ^ new_bits;
        state_bits = new_bits;
        for (uint8_t i = 0; i < static_cast<uint8_t>(AppBtnID::size); ++i) {
          if (diff_bits & ((uint16_t)1 << i)) {
            on_change(static_cast<AppBtnID>(i), !!(new_bits & ((uint16_t)1 << i)));
          }
        }
      }
      return true;
    }
    return false;
  }
};

extern AppButtons buttons;

#endif /* LR_APP_BUTTONS_H */
