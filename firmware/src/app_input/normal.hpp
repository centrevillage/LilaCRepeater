#ifndef LR_APP_INPUT_NORMAL_H
#define LR_APP_INPUT_NORMAL_H

#include "app_input/base.hpp"
#include "looper.hpp"

struct AppInputNormal {
  static inline uint16_t _to_uint16_t(float v) {
    return (uint16_t)(v * 1023.9999f);
  }

  void init() {
    view.changeMode(AppViewNormal {});
  }

  void refresh() {
    if (!view.isMode<AppViewNormal>()) { return; }
    auto& view_mode = view.getMode<AppViewNormal>();

    view_mode.setDryVol(_to_uint16_t(analogs.value(AppSliderID::dry)));
    view_mode.setWetVol(_to_uint16_t(analogs.value(AppSliderID::wet)));
    view_mode.setPan(_to_uint16_t(analogs.value(AppSliderID::pan)));
    view_mode.setLength(_to_uint16_t(analogs.value(AppSliderID::length)));
    view_mode.setPos(_to_uint16_t(analogs.value(AppSliderID::pos)));
    view_mode.setFdbk(_to_uint16_t(analogs.value(AppSliderID::fdbk)));
    view_mode.setSpeed(_to_uint16_t(analogs.value(AppSliderID::speed)));

    view.update();
  }

  bool button(AppBtnID id, bool on) {
    switch (id) {
      case AppBtnID::rec:
        if (on) {
          looper.rec();
          view.dirty();
        }
        // TODO:
        break;
      default:
        break;
    }
    return false;
  }

  bool slider(AppSliderID id, float value) {
    if (!view.isMode<AppViewNormal>()) { return false; }
    auto& view_mode = view.getMode<AppViewNormal>();
    uint16_t v = _to_uint16_t(value);
    switch (id) {
      case AppSliderID::dry:
        view_mode.setDryVol(v);
        break;
      case AppSliderID::wet:
        view_mode.setWetVol(v);
        break;
      case AppSliderID::pan:
        view_mode.setPan(v);
        break;
      case AppSliderID::length:
        view_mode.setLength(v);
        break;
      case AppSliderID::pos:
        view_mode.setPos(v);
        break;
      case AppSliderID::fdbk:
        view_mode.setFdbk(v);
        break;
      case AppSliderID::speed:
        view_mode.setSpeed(v);
        break;
      default:
        break;
    }
    return true;
  }
};

#endif /* LR_APP_INPUT_NORMAL_H */
