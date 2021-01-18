#ifndef LR_APP_VIEW_NORMAL_H
#define LR_APP_VIEW_NORMAL_H

#include <cstdio>
#include <igb_sdk/util/text.h>
#include "app_analogs.hpp"
#include "app_oled.hpp"
#include "looper.hpp"
#include "image/recorder.h"
#include "app_view/dialog.hpp"

struct AppViewNormal {
  bool is_dirty = true;
  float dry_vol = 0;    /* 0.0 ~ 1.0 */
  float wet_vol = 0;    /* 0.0 ~ 1.0 */
  float pan = 0;        /* 0.0 ~ 1.0 */
  uint8_t length = 0;  /* 0 ~ 127 */
  uint8_t pos = 0;     /* 0 ~ 127 */
  float fdbk = 0;       /* 0.0 ~ 1.0 */
  float speed = 0;      /* [0.5, ..., 1, ..., 2] */

  uint8_t current_pos = 0; // 現在位置の表示用
  uint8_t unmuted_track = 0b1111;
  uint8_t current_track = 0;

  float bpm = 120.0f;

  Dialog alert_dialog;

  void init() {
    app_oled.drawFillBG();
    app_oled.drawTextMedium("LilaC Repeater", 14, 3, 0);
    is_dirty = true;
    app_oled.process();
    looper.on_error = [this](LooperErrorType error_type, uint8_t track_idx){
      switch (error_type) {
        case LooperErrorType::ram_full:
          alert_dialog.alert("Memory Full!");
          break;
        default:
          break;
      }
    };
  }

  void update() {
    char text_buf[24];
    app_oled.drawFillBG();

    // header
    snprintf(
      text_buf, sizeof(text_buf),
      "D%3d P%3d W%3d",
      (uint8_t)(dry_vol * 100), (uint8_t)(pan * 100), (uint8_t)(wet_vol * 100)
    );
    app_oled.drawTextSmall(text_buf, sizeof(text_buf), 0, 0);
    snprintf(text_buf, sizeof(text_buf), "%3d.%1dBPM", (uint16_t)bpm, (uint8_t)((bpm - (uint16_t)bpm) * 10));
    app_oled.drawTextSmall(text_buf, sizeof(text_buf), 0, 128-(5*8));

    // track
    for (uint8_t i = 0; i < 4; ++i) {
      if (unmuted_track & ((uint8_t)1<<i)) {
        snprintf(text_buf, sizeof(text_buf), "%1d", i+1);
        app_oled.drawTextSmall(text_buf, sizeof(text_buf), 1, 14+i*32);
      }
    }
    app_oled.drawInvert(1, current_track*32, 32); 

    snprintf(text_buf, sizeof(text_buf), "%3d L%3d", pos+1, length+1);
    app_oled.drawTextMedium(text_buf, sizeof(text_buf), 4, 0);
    snprintf(text_buf, sizeof(text_buf), "x%1d.%02d FB%3d%%", (uint16_t)speed, (uint16_t)((speed - (uint16_t)speed) * 100), (uint8_t)(fdbk * 100));
    app_oled.drawTextMedium(text_buf, sizeof(text_buf), 6, 0);

    if (looper.is_rec) {
      // locked parameter
      app_oled.drawInvert(4, 0, 10*8); // pos length
      app_oled.drawInvert(5, 0, 10*8);
      app_oled.drawInvert(6, 0, 6*8); // speed
    }

    // content
    app_oled.drawPageBit(2, 0, 0b10000000);
    app_oled.drawPageBit(3, 0, 0xFF);
    app_oled.drawPageBit(4, 0, 0b00000001);
    app_oled.drawPageBit(2, 127, 0b10000000);
    app_oled.drawPageBit(3, 127, 0xFF);
    app_oled.drawPageBit(4, 127, 0b00000001);
    for (uint8_t i = 0; i < 128; ++i) {
      app_oled.drawPageBit(2, i, 0b10000000);
      app_oled.drawPageBit(4, i, 0b00000001);
      if (
        (i >= pos && i < pos + length) ||
        (pos + length > 127 && i >= 0 && i < (pos + length - 127))
      ) {
        app_oled.drawPageBit(3, i, 0xFF);
      }
    }
    app_oled.drawPageBit(2, current_pos, 0b11110000);
    if (0 < current_pos && current_pos < 127) {
      app_oled.drawInvert(3, current_pos, 1);
    }
    app_oled.drawPageBit(4, current_pos, 0b00001111);

    // status
    if (looper.is_run) {
      //app_oled.drawTextMedium("RUN", 3, 6, 84);
      _drawImage(REC_IMG_TYPE_PLAY, 4, 112);
    }
    if (looper.is_rec) {
      //app_oled.drawTextMedium("REC", 3, 6, 0);
      _drawImage(REC_IMG_TYPE_REC, 6, 112);
    } else if (looper.is_empty) {
      app_oled.drawTextMedium("New", 3, 6, 104);
    }

    alert_dialog.update();

    app_oled.process();
  }

  void _drawImage(RecorderImageType type, uint8_t page, uint8_t offset) {
    for (uint8_t x = 0; x < 16; ++x) {
      app_oled.drawPageBit(page, offset + x, (uint8_t)recorder_image_16_16[type][x]);
      app_oled.drawPageBit(page+1, offset + x, (uint8_t)(recorder_image_16_16[type][x] >> 8));
    }
  }

  void process() {
    is_dirty = is_dirty || alert_dialog.process();
    if (is_dirty) {
      update();
      is_dirty = false;
    }
  }

  void setDryVol(float v) {
    dry_vol = v;
    is_dirty = true;
  }

  void setWetVol(float v) {
    wet_vol = v;
    is_dirty = true;
  }

  void setPan(float v) {
    pan = v;
    is_dirty = true;
  }

  void setLength(uint8_t v) {
    length = v;
    is_dirty = true;
  }

  void setPos(uint8_t v) {
    pos = v;
    is_dirty = true;
  }

  void setFdbk(float v) {
    fdbk = v;
    is_dirty = true;
  }

  void setSpeed(float v) {
    speed = v;
    is_dirty = true;
  }

  void setCurrentPos(uint8_t v) {
    current_pos = v;
    is_dirty = true;
  }
};

#endif /* LR_APP_VIEW_NORMAL_H */
