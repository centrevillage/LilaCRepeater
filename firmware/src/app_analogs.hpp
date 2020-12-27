#ifndef LR_APP_ANALOGS_H
#define LR_APP_ANALOGS_H

#include "hardware.hpp"

// Daisyのピン番号が0 originなのでハードウェアのGPIO番号-1で定義する
#define SLIDER_DRY_VOL_PIN 22-1
#define SLIDER_WET_VOL_PIN 21-1
#define SLIDER_PAN_PIN 20-1
#define SLIDER_LENGTH_PIN 19-1
#define SLIDER_POS_PIN 18-1
#define SLIDER_FDBK_PIN 17-1
#define SLIDER_SPEED_PIN 16-1

enum class AppSliderID : uint8_t {
  dry = 0,
  wet,
  pan,
  length,
  pos,
  fdbk,
  speed
};
constexpr size_t slider_count = 7;

struct AppAnalogs {

  float slider_values[slider_count];
  std::function<void(AppSliderID, float)> on_change;

  void init() {
    daisy::AdcChannelConfig adcConfigs[slider_count];
    adcConfigs[static_cast<uint8_t>(AppSliderID::dry)].InitSingle(hardware.GetPin(SLIDER_DRY_VOL_PIN));
    adcConfigs[static_cast<uint8_t>(AppSliderID::wet)].InitSingle(hardware.GetPin(SLIDER_WET_VOL_PIN));
    adcConfigs[static_cast<uint8_t>(AppSliderID::pan)].InitSingle(hardware.GetPin(SLIDER_PAN_PIN));
    adcConfigs[static_cast<uint8_t>(AppSliderID::length)].InitSingle(hardware.GetPin(SLIDER_LENGTH_PIN));
    adcConfigs[static_cast<uint8_t>(AppSliderID::pos)].InitSingle(hardware.GetPin(SLIDER_POS_PIN));
    adcConfigs[static_cast<uint8_t>(AppSliderID::fdbk)].InitSingle(hardware.GetPin(SLIDER_FDBK_PIN));
    adcConfigs[static_cast<uint8_t>(AppSliderID::speed)].InitSingle(hardware.GetPin(SLIDER_SPEED_PIN));

    //Initialize the adc with the config we just made
    hardware.adc.Init(adcConfigs, slider_count);
    //Start reading values
    hardware.adc.Start();
  }

  bool process() {
    bool is_dirty = false;
    for (uint8_t i = 0; i < slider_count; ++i) {
      // TODO: LPFかけて読み出すべき？
      float value = hardware.adc.GetFloat(i);
      if (slider_values[i] != value) {
        slider_values[i] = value;
        if (on_change) {
          on_change(static_cast<AppSliderID>(i), value);
        }
        is_dirty = true;
      }
    }
    return is_dirty;
  }

  float value(AppSliderID id) {
    return slider_values[static_cast<uint8_t>(id)];
  }
};

extern AppAnalogs analogs;

#endif /* LR_APP_ANALOGS_H */
