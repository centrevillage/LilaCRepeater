#ifndef LR_APP_INPUT_PROGRAM_H
#define LR_APP_INPUT_PROGRAM_H

#include "app_input/base.hpp"

struct AppInputProgram {
  void init() {
    view.changeMode(AppViewProgram {});
  }

  void refresh() {
    if (!view.isMode<AppViewProgram>()) { return; }
    auto& view_mode = view.getMode<AppViewProgram>();

    view.update();
  }

  bool button(AppBtnId id, bool on) {
    return false;
  }

  bool trigger(AppTrigId id, bool on) {
    return false;
  }

  bool slider(AppSliderID id, float value) {
    return false;
  }
};

#endif /* LR_APP_INPUT_PROGRAM_H */
