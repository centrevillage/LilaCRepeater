#ifndef LR_APP_INPUT_H
#define LR_APP_INPUT_H

#include "app_buttons.hpp"
#include "app_analogs.hpp"
#include "app_view.hpp"

#include "app_input/base.hpp"
#include "app_input/normal.hpp"
#include "app_input/system.hpp"

struct AppInputModeState {
  std::variant<
    AppInputNormal,
    AppInputSystem
  > mode = AppInputNormal {};

  void change(AppInputModeId m) {
    switch (m) {
      case AppInputModeId::normal:
        mode = AppInputNormal {};
        break;
      case AppInputModeId::system:
        mode = AppInputSystem {};
        break;
      default:
        break;
    }
    init();
  }

  void init() {
    std::visit([](auto& m) {
      m.init();
    }, mode);
  }

  void refresh() {
    std::visit([](auto& m) {
      m.refresh();
    }, mode);
  }

  bool button(AppBtnID id, bool on) {
    return std::visit([=](auto& m) {
      return m.button(id, on);
    }, mode);
  }

  bool slider(AppSliderID id, float value) {
    return std::visit([=](auto& m) {
      return m.slider(id, value);
    }, mode);
  }
};

struct AppInput {
  AppInputModeId current_mode = AppInputModeId::normal;
  AppInputModeState state;

  bool is_dirty = true;
  
  void init() {
    state.init();
    analogs.init();
    buttons.init();

    buttons.on_change = [this](AppBtnID id, bool on) {
      state.button(id, on);
    };

    analogs.on_change = [this](AppSliderID id, float value) {
      state.slider(id, value);
    };
  }

  void process() {
    buttons.process();
    analogs.process();

    //bool is_dirty = buttons.process();
    //is_dirty ||= analogs.process();
    //if (is_dirty) {
    //  view.refresh();
    //  is_dirty = false;
    //}
  }
};

extern AppInput input;

#endif /* LR_APP_INPUT_H */
