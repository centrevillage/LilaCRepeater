#ifndef LR_APP_VIEW_DIALOG_H
#define LR_APP_VIEW_DIALOG_H

#include "app_view/base.hpp"
#include <igb_sdk/util/soft_timer.hpp>
#include <igb_stm32/periph/systick.hpp>
#include "app_oled.hpp"

using namespace igb::sdk;

struct Dialog {
  SoftTimer<1> timer;
  bool show_dialog = false;
  constexpr static size_t msg_size = 24;
  char msg[msg_size];
  bool is_dirty = false;

  void alert(const char* str) {
    for (size_t i = 0; i < msg_size; ++i) {
      if (str[i] == 0) {
        break;
      }
      msg[i] = str[i];
    }
    show();
    timer.oneshotCallback(2000, current_msec(), [this]{
      hide();
    });
  }

  void show() {
    show_dialog = true;
    is_dirty = true;
  }

  void hide() {
    show_dialog = false;
    is_dirty = true;
  }

  bool process() {
    timer.process(current_msec());
    if (is_dirty) {
      is_dirty = false;
      return true;
    }
    return false;
  }

  void update() {
    if (show_dialog) {
      app_oled.drawFillRect(7, 15, 114, 34, true, false);
      app_oled.drawTextMedium(msg, sizeof(msg), 2, 8);
    }
  }
};

#endif /* LR_APP_VIEW_DIALOG_H */
