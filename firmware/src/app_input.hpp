#ifndef LR_APP_INPUT_H
#define LR_APP_INPUT_H

#include "app_buttons.hpp"
#include "app_analogs.hpp"
#include "app_view.hpp"
#include "app_trigger_input.hpp"

#include "app_input/base.hpp"
#include "app_input/normal.hpp"
#include "app_input/system.hpp"
#include "app_input/program.hpp"
#include "app_input/test.hpp"

struct AppInputModeState {
  using Mode = std::variant< AppInputNormal, AppInputProgram, AppInputSystem, AppInputTest>;

  Mode mode = AppInputNormal {};

  void change(AppInputModeId m) {
    switch (m) {
      case AppInputModeId::normal:
        mode = AppInputNormal {};
        break;
      case AppInputModeId::program:
        mode = AppInputProgram {};
        break;
      case AppInputModeId::system:
        mode = AppInputSystem {};
        break;
      case AppInputModeId::test:
        mode = AppInputTest {};
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

  bool button(AppBtnId id, bool on) {
    return std::visit([=](auto& m) {
      return m.button(id, on);
    }, mode);
  }

  bool trigger(AppTrigId id, bool on) {
    return std::visit([=](auto& m) {
      return m.trigger(id, on);
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
#ifdef TESTMODE
    current_mode = AppInputModeId::test;
#endif
    state.change(current_mode);
    analogs.init();
    buttons.init();
    triggers.init();

    buttons.on_change = [this](AppBtnId id, bool on) {
      state.button(id, on);
    };

    analogs.on_change = [this](AppSliderID id, float value) {
      state.slider(id, value);
    };

    triggers.on_change = [this](AppTrigId id, bool on) {
      if (!state.trigger(id, on)) {
        triggerOnChange(id, on);
      }
    };
  }

  void triggerOnChange(AppTrigId id, bool on) {
    // TODO:
    switch(id) {
      case AppTrigId::sync:
        break;
      case AppTrigId::reset:
        if (on) {
          looper.reset();
          view.dirty();
        }
        break;
      case AppTrigId::run:
        if (on) {
          looper.toggleRun();
          view.dirty();
        }
        break;
      case AppTrigId::rec:
        if (on) {
          looper.toggleRec();
          view.dirty();
        }
        break;
      case AppTrigId::ext_sync_sw:
        looper.setExtSync(on);
        view.dirty();
        break;
      default:
        break;
    }
  }

  void process() {
    buttons.process();
    triggers.process(); // TODO: use EXTI
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
