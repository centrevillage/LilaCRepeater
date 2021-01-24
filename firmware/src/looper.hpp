#ifndef LR_LOOPER_H
#define LR_LOOPER_H

#include <array>
#include <cassert>
#include <functional>

#include "timer_conf.h"
#include "sample.hpp"
#include <igb_stm32/periph/tim.hpp>

using namespace igb::stm32;

struct LooperTrack {
  // 連続したRAM領域の一部を参照するバッファ
  std::array<SampleSlice, 2> slices;

  // TODO: ループサイズ可変
  inline void init(uint8_t track_idx) {
    slices[0] = sample.slice(
        sample_max_buffer_size / 4 * track_idx, sample_max_buffer_size / 8);
    slices[1] = sample.slice(
        sample_max_buffer_size / 4 * track_idx + sample_max_buffer_size / 8, sample_max_buffer_size / 8);
  }

  inline float getPos() const {
    return slices[0].getPos();
  }

  inline void initStepValue(float step) {
    slices[0].initStepValue(step);
    slices[1].initStepValue(step);
  }

  // 1 sample tickごとのサンプルテープ進行量
  inline void step(float v) {
    slices[0].step(v);
    slices[1].step(v);
  }

  inline void jump(float pos) {
    slices[0].jump(pos);
    slices[1].jump(pos);
  }

  inline void setFeedback(float fb) {
    slices[0].setFeedback(fb);
    slices[1].setFeedback(fb);
  }

  inline void reset() {
    slices[0].reset();
    slices[1].reset();
  }

  inline void startRec() {
    slices[0].startRec();
    slices[1].startRec();
  }

  inline void endRec() {
    slices[0].endRec();
    slices[1].endRec();
  }

  inline void enableLoop() {
    slices[0].loop_range_enabled = true;
    slices[1].loop_range_enabled = true;
  }

  inline bool isOverrapBySize() const {
    return slices[0].overrap == SampleSlice::OverrapType::by_size;
  }

  inline bool isOverrapByEnd() const {
    return slices[0].overrap == SampleSlice::OverrapType::by_end;
  }

  inline void setCurrent(float left, float right) {
    slices[0].setCurrent(left);
    slices[1].setCurrent(right);
  }

  inline std::pair<float, float> getCurrent() const {
    return std::make_pair(slices[0].getCurrent(), slices[1].getCurrent());
  }

  inline void recValueAndStep(float step_, float left, float right) {
    slices[0].recValueAndStep(step_, left);
    slices[1].recValueAndStep(step_, right);
  }
};

constexpr size_t track_size = 4;

constexpr float default_bpm = 120.0f;
constexpr float tim_clock = 200000000.0f;
constexpr float beat_per_tick = 4.0; // 16分音符が最小単位

// Max TIM speed is 200MHz (SystemClock is 400MHz)
constexpr uint32_t calc_tim_period(float _bpm) {
  return (tim_clock / (_bpm / 60.0f)) / beat_per_tick;
}

enum class LooperErrorType {
  none = 0,
  ram_full
};

/*
 * ルーパーへの操作を行う。
 */
// TODO: pos/lengthを外部設定された場合の反映タイミング
//   外部同期なし:
//    ・再生中のサンプルが分割位置に到達したタイミング
//      ・分割位置に到達した判定
//        ・lenght/sizeによる位置リセットがある場合
//          ・overrapフラグで判定
//        ・それ以外の場合
//          ・サンプルの絶対位置で判定
struct Looper {
  float dry_vol = 1.0f;
  float wet_vol = 1.0f;
  float pan = 0.5f;
  float fdbk = 1.0f;
  uint8_t pos = 0;
  uint8_t length = 127;
  float speed = 1.0f;

  std::array<LooperTrack, track_size> tracks;
  uint8_t current_track_idx = 0;
  uint8_t fdbk_sources = 0;
  bool is_run = false;
  bool is_rec = false;
  bool is_empty = true;
  bool is_ext_sync = false;
  float bpm = default_bpm;
  uint32_t tim_period = calc_tim_period(default_bpm);
  uint8_t recorded_length = 127;
  float recorded_speed = 1.0f; // 録音時のベーススピード
  bool is_quantized_rec = false;
  Tim tim; // 32bit timer
  std::function<void(LooperErrorType, uint8_t)> on_error;

  LooperTrack& currentTrack() {
    return tracks[current_track_idx];
  }

  void init() {
    sample.init();
    tim = Tim::newIntervalTimer(TIM_LOOPER_CLOCK_TYPE, 0, tim_period, 0);
    tim.setCount(0);
    for (uint8_t i = 0; i < track_size; ++i) {
      tracks[i].init(i);
    }
  }

  void start() {
    if (!is_empty && !is_run) {
      tim.setCount(0);
      tim.enable();
      is_run = true;
    }
  }

  void stop() {
    if (is_run) {
      tim.disable();
      is_run = false;
    }
  }

  void toggleRun() {
    if (is_run) {
      stop();
    } else {
      start();
    }
  }

  void toggleRec() {
    if (is_rec) {
      stopRec();
    } else {
      rec();
    }
  }

  void rec() {
    if (!is_run) {
      is_rec = true;
      if (is_ext_sync) {
        is_quantized_rec = true;
      }
      if (is_empty) {
        recorded_speed = speed;
        reset();
      }
      start();
      currentTrack().initStepValue(speed);
      currentTrack().startRec();
    }
  }

  void stopRec() {
    currentTrack().endRec();
    if (is_empty) {
      updateBpm(_calcBpmFromLength());
      recorded_length = length;
      currentTrack().enableLoop();
      is_empty = false;
    }
    is_rec = false;
  }

  void abortRec() {
    is_rec = false;
    currentTrack().reset();
  }

  void reset() {
    if (is_rec) {
      stopRec();
    }
    currentTrack().reset(); 
  }

  void selectTrack(uint8_t track_idx) {
    assert(track_idx < 4);
    current_track_idx = track_idx;
  }

  void updateBpm(float new_bpm) {
    bpm = new_bpm;
    tim_period = calc_tim_period(default_bpm);
    tim.setAutoreload(tim_period);
  }

  void receiveSyncClock() {
  }

  void receiveMidiClock() {
  }

  void processAudio(float *in, float *out, size_t size) {
    if (!is_run) {
      // TODO: dry volume
      // dry thru
      for(size_t i = 0; i < size; i += 2) {
        // left out
        out[i] = in[i];
        // right out
        out[i + 1] = in[i + 1];
      }
      return;
    }

    // simple panning (variable amp)
    float pan_vol_l = 1.0f;
    float pan_vol_r = 1.0f;
    if (pan < 0.5f) {
      pan_vol_r = 1.0f - (0.5f - pan) * 2.0f;
    } else if (pan > 0.5f) {
      pan_vol_l = 1.0f - (pan - 0.5f) * 2.0f;
    }

    currentTrack().setFeedback(fdbk);

    if (is_rec) {
      for (size_t i = 0; i < size; i += 2) {
        if (currentTrack().isOverrapBySize()) {
          // RAMの空きがなくなった
          abortRec();
          if (on_error) {
            on_error(LooperErrorType::ram_full, current_track_idx);
          }
          break;
        }
        float dry_l = dry_vol * pan_vol_l * in[i];
        float dry_r = dry_vol * pan_vol_r * in[i+1];
        auto current_values = currentTrack().getCurrent();
        out[i] = dry_l + wet_vol * current_values.first;
        out[i+1] = dry_r + wet_vol * current_values.second;
        currentTrack().recValueAndStep(speed, dry_l, dry_r);
      }
    } else {
      for(size_t i = 0; i < size; i += 2) {
        auto current_values = currentTrack().getCurrent();
        float dry_l = dry_vol * pan_vol_l * in[i];
        float dry_r = dry_vol * pan_vol_r * in[i+1];
        out[i] = dry_l + wet_vol * current_values.first;
        out[i+1] = dry_r + wet_vol * current_values.second;
        currentTrack().step(speed);
      }
    }
  }

  inline float samplePos() {
    return (float)pos * sampleByStep();
  }

  inline float sampleByStep() {
    float bps = bpm / 60.0f;
    float sps = bps * 4.0f; // step per second
    return (float)sample_rate * sps;
  }

  inline void setExtSync(bool flag) {
    is_ext_sync = flag;
  }

  float _calcBpmFromLength() {
    float recEndPos = currentTrack().getPos();
    float recordedSec = recEndPos / 48000.0f;
    float recordedSteps = (float)(length+1);
    float bps = recordedSec / recordedSteps * 4.0f;
    return bps * 60.0f * recorded_speed;
  }
};

extern Looper looper;

extern "C" {

void TIM_LOOPER_CLOCK_HANDLER();

}

#endif /* LR_LOOPER_H */
