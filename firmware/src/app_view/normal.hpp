#ifndef LR_APP_VIEW_NORMAL_H
#define LR_APP_VIEW_NORMAL_H

#include <igb_sdk/util/text.h>
#include "app_analogs.hpp"
#include "app_oled.hpp"

struct AppViewNormal {
  bool is_dirty = true;
  uint16_t dry_vol = 0;
  uint16_t wet_vol = 0;
  uint16_t pan = 0;
  uint16_t length = 0;
  uint16_t pos = 0;
  uint16_t fdbk = 0;
  uint16_t speed= 0;

  void init() {
    app_oled.drawFillBG();
    app_oled.drawTextMedium("LilaC Repeater", 14, 3, 0);
    is_dirty = true;
    app_oled.process();
  }

  void update() {
    char text_buf[12];
    app_oled.drawFillBG();

    text_from_uint16(text_buf, fdbk);
    app_oled.drawTextSmall(text_buf, 6, 2, 0);

    text_from_uint16(text_buf, dry_vol);
    app_oled.drawTextSmall(text_buf, 6, 3, 0);

    text_from_uint16(text_buf, pos);
    app_oled.drawTextSmall(text_buf, 6, 1, 42);

    text_from_uint16(text_buf, length);
    app_oled.drawTextSmall(text_buf, 6, 2, 42);

    text_from_uint16(text_buf, pan);
    app_oled.drawTextSmall(text_buf, 6, 3, 42);

    text_from_uint16(text_buf, speed);
    app_oled.drawTextSmall(text_buf, 6, 2, 84);

    text_from_uint16(text_buf, wet_vol);
    app_oled.drawTextSmall(text_buf, 6, 3, 84);

    app_oled.process();
  }

  void process() {
    if (is_dirty) {
      update();
      is_dirty = false;
    }
  }

  void setDryVol(uint16_t v) {
    dry_vol = v;
    is_dirty = true;
  }

  void setWetVol(uint16_t v) {
    wet_vol = v;
    is_dirty = true;
  }

  void setPan(uint16_t v) {
    pan = v;
    is_dirty = true;
  }

  void setLength(uint16_t v) {
    length = v;
    is_dirty = true;
  }

  void setPos(uint16_t v) {
    pos = v;
    is_dirty = true;
  }

  void setFdbk(uint16_t v) {
    fdbk = v;
    is_dirty = true;
  }

  void setSpeed(uint16_t v) {
    speed = v;
    is_dirty = true;
  }
};

#endif /* LR_APP_VIEW_NORMAL_H */
