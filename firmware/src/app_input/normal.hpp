#ifndef LR_APP_INPUT_NORMAL_H
#define LR_APP_INPUT_NORMAL_H

#include <algorithm>

#include "app_input/base.hpp"
#include "looper.hpp"

struct AppInputNormal {
  bool is_rescale_length = false;
  bool is_rescale_pos = false;

  static inline uint16_t _to_uint16_t(float v) {
    return (uint16_t)(v * 1023.9999f);
  }

  static inline uint8_t _to_uint8_t(float v) {
    return (uint8_t)(v * 127.9999f);
  }

  static inline float _to_speed_value(float v) {
    static const float speed_table[25] = {
// ruby:
// scales = (0..12).map {|i| 2.0**(i.to_f/12.0)}
// scales.each do |scale|
//   print <<-EOS
//       #{0.5*scale},
//   EOS
// end
// scales[1..-1].each do |scale|
//   print <<-EOS
//       #{1.0*scale},
//   EOS
// end
      0.5,
      0.5297315471796477,
      0.5612310241546865,
      0.5946035575013605,
      0.6299605249474366,
      0.6674199270850172,
      0.7071067811865476,
      0.7491535384383408,
      0.7937005259840997,
      0.8408964152537145,
      0.8908987181403393,
      0.9438743126816934,
      1.0,
      1.0594630943592953,
      1.122462048309373,
      1.189207115002721,
      1.2599210498948732,
      1.3348398541700344,
      1.4142135623730951,
      1.4983070768766815,
      1.5874010519681994,
      1.681792830507429,
      1.7817974362806785,
      1.8877486253633868,
      2.0,
    };
    uint8_t idx = (uint8_t)(v * 24.9999f);
    return speed_table[idx];
  }

  void init() {
    view.changeMode(AppViewNormal {});
  }

  void refresh() {
    if (!view.isMode<AppViewNormal>()) { return; }
    auto& view_mode = view.getMode<AppViewNormal>();

    for (uint8_t i = 0; i < slider_count; ++i) {
      AppSliderID id = static_cast<AppSliderID>(i);
      slider(id, analogs.value(id));
    }

    view.update();
  }

  bool button(AppBtnId id, bool on) {
    switch (id) {
      case AppBtnId::rec:
        if (on) {
          looper.toggleRec();
          view.dirty();
        }
        // TODO:
        break;
      case AppBtnId::run:
        if (on) {
          if (looper.is_rec) {
            // TODO: どういう挙動にすべきか検討
          } else {
            if (looper.is_run) {
              looper.start();
              view.dirty();
            } else {
              looper.stop();
              view.dirty();
            }
          }
        }
        break;
      default:
        break;
    }
    return false;
  }

  bool slider(AppSliderID id, float value) {
    if (!view.isMode<AppViewNormal>()) { return false; }
    auto& view_mode = view.getMode<AppViewNormal>();
    switch (id) {
      case AppSliderID::dry:
        view_mode.setDryVol(value);
        looper.dry_vol = value;
        break;
      case AppSliderID::wet:
        view_mode.setWetVol(value);
        looper.wet_vol = value;
        break;
      case AppSliderID::pan:
        view_mode.setPan(value);
        looper.pan = value;
        break;
      case AppSliderID::length:
        // TODO: 
        // length と pos は即座に値を反映するのでなくて、ステップの境界で反映する必要がある
        // でないと、ループの位相がずれた状態になってしまう
        if (!looper.is_rec) {
          uint8_t v = _to_uint8_t(value);
          if (!looper.is_empty && v == 127) {
            is_rescale_length = true;
          }
          if (looper.is_empty) {
            is_rescale_length = false;
          }
          if (is_rescale_length) {
            v = (uint8_t)(value * (looper.recorded_length + 0.9999f));
          }
          uint8_t len = std::min(looper.recorded_length, v);
          view_mode.setLength(len);
          looper.length = len;
        }
        break;
      case AppSliderID::pos:
        if (!looper.is_rec) {
          uint8_t v = _to_uint8_t(value);
          if (!looper.is_empty && v == 127) {
            is_rescale_pos = true;
          }
          if (looper.is_empty) {
            is_rescale_pos = false;
          }
          if (is_rescale_length) {
            v = (uint8_t)(value * (looper.recorded_length + 0.9999f));
          }
          uint8_t pos = std::min(looper.recorded_length, v);
          view_mode.setPos(pos);
          looper.pos = pos;
        }
        break;
      case AppSliderID::fdbk:
        view_mode.setFdbk(value);
        looper.fdbk = value;
        break;
      case AppSliderID::speed:
        view_mode.setSpeed(_to_speed_value(value));
        looper.speed = _to_speed_value(value);
        break;
      default:
        break;
    }
    return true;
  }
};

#endif /* LR_APP_INPUT_NORMAL_H */
