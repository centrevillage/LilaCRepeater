#ifndef LR_APP_VIEW_H
#define LR_APP_VIEW_H

#include <variant>
#include <algorithm>

#include <igb_stm32/periph/gpio.hpp>
#include <igb_stm32/periph/spi.hpp>
#include <igb_sdk/device/oled_ssd1306.hpp>
#include <igb_daisy/periph/gpio.hpp>
#include <igb_sdk/util/text.h>

#include "app_oled.hpp"

#include "app_view/base.hpp"
#include "app_view/normal.hpp"
#include "app_view/system.hpp"
#include "app_view/program.hpp"
#include "app_view/start.hpp"
#include "app_view/firmware.hpp"
#include "app_view/test.hpp"

using namespace daisy;
using namespace igb::stm32;
using namespace igb::sdk;
using namespace igb::daisy;

struct AppViewModeState {
  using Mode = std::variant< AppViewNormal, AppViewProgram, AppViewSystem, AppViewStart, AppViewFirmware, AppViewTest>;

  Mode mode = AppViewNormal {};

  AppViewModeId currentMode() {
    return static_cast<AppViewModeId>(mode.index());
  }

  void change(AppViewModeId m) {
    switch (m) {
      case AppViewModeId::normal:
        mode = AppViewNormal {};
        break;
      case AppViewModeId::program:
        mode = AppViewProgram {};
        break;
      case AppViewModeId::system:
        mode = AppViewSystem {};
        break;
      case AppViewModeId::start:
        mode = AppViewStart {};
        break;
      case AppViewModeId::firmware:
        mode = AppViewFirmware {};
        break;
      case AppViewModeId::test:
        mode = AppViewTest {};
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

  void update() {
    std::visit([](auto& m) {
      m.update();
    }, mode);
  }

  void dirty() {
    std::visit([](auto& m) {
      m.is_dirty = true;
    }, mode);
  }

  bool isDirty() {
    return std::visit([](auto& m) {
      return m.is_dirty;
    }, mode);
  }

  void process() {
    std::visit([](auto& m) {
      m.process();
    }, mode);
  }
};

struct AppView {
  AppViewModeState state;

  void init() {
    app_oled.spi.initMasterOutOnly(SpiBaudratePrescaler::div2);
    app_oled.init();

    state.init();
  };

  void changeMode(AppViewModeState::Mode&& mode) {
    state.mode = mode;
    state.init();
  }

  void process() {
    state.process();
  }

  void dirty() {
    state.dirty();
  }

  void update() {
    state.update();
  }

  template<typename MODE_TYPE>
  MODE_TYPE& getMode() {
    return std::get<MODE_TYPE>(state.mode);
  }

  template<typename MODE_TYPE>
  bool isMode() {
    return std::holds_alternative<MODE_TYPE>(state.mode);
  }
};

extern AppView view;

#endif /* LR_APP_VIEW_H */
