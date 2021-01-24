#ifndef LR_APP_INPUT_TEST_H
#define LR_APP_INPUT_TEST_H

#include "app_input/base.hpp"
#include "app_view/test.hpp"

struct AppInputTest {
  void init() {
    view.changeMode(AppViewTest {});
  }

  void refresh() {
    for (uint8_t i = 0; i < button_count; ++i) {
      AppBtnId id = static_cast<AppBtnId>(i);
      button(id, buttons.isOn(id));
    }

    for (uint8_t i = 0; i < trigger_count; ++i) {
      AppTrigId id = static_cast<AppTrigId>(i);
      trigger(id, triggers.isOn(id));
    }

    for (uint8_t i = 0; i < slider_count; ++i) {
      AppSliderID id = static_cast<AppSliderID>(i);
      slider(id, analogs.value(id));
    }

    view.update();
  }

  bool button(AppBtnId id, bool on) {
    if (!view.isMode<AppViewTest>()) { return false; }
    auto& view_mode = view.getMode<AppViewTest>();
    view_mode.button_states[static_cast<uint8_t>(id)] = on;
    view.dirty();
    return true;
  }

  bool trigger(AppTrigId id, bool on) {
    if (!view.isMode<AppViewTest>()) { return false; }
    auto& view_mode = view.getMode<AppViewTest>();
    view_mode.trig_states[static_cast<uint8_t>(id)] = on;
    view.dirty();
    return false;
  }

  bool slider(AppSliderID id, float value) {
    if (!view.isMode<AppViewTest>()) { return false; }
    auto& view_mode = view.getMode<AppViewTest>();
    switch(id) {
      case AppSliderID::dry:
        view_mode.dry_vol = value;
        break;
      case AppSliderID::wet:
        view_mode.wet_vol = value;
        break;
      case AppSliderID::pan:
        view_mode.pan = value;
        break;
      case AppSliderID::length:
        view_mode.length = value;
        break;
      case AppSliderID::pos:
        view_mode.pos = value;
        break;
      case AppSliderID::fdbk:
        view_mode.fdbk = value;
        break;
      case AppSliderID::speed:
        view_mode.speed = value;
        break;
      default:
        break;
    }
    view.dirty();
    return true;
  }
};

#endif /* LR_APP_INPUT_TEST_H */
