#ifndef LR_LOOPER_H
#define LR_LOOPER_H

#include <array>
#include <cassert>

#include "timer_conf.h"
#include "sample.hpp"
#include <igb_stm32/periph/tim.hpp>

using namespace igb::stm32;

struct LooperTrack {
  // 連続したRAM領域の一部を参照するバッファ
  std::array<SampleSlice, 2> slices;

  // TODO: ループサイズ可変
  void init(uint8_t track_idx) {
    slices[0] = sample.slice(
        sample_max_buffer_size / 4 * track_idx, sample_max_buffer_size / 8);
    slices[1] = sample.slice(
        sample_max_buffer_size / 4 * track_idx + sample_max_buffer_size / 8, sample_max_buffer_size / 8);
  }

  // 1 sample tickごとのサンプルテープ進行量
  void step(float v) {
    slices[0].step(v);
    slices[1].step(v);
  }

  void startRec() {
    slices[0].startRec();
    slices[1].startRec();
  }

  void endRec() {
    slices[0].endRec();
    slices[1].endRec();
  }

  bool isOverrap() {
    return slices[0].is_overrap;
  }

  void setCurrent(float left, float right) {
    slices[0].setCurrent(left);
    slices[1].setCurrent(right);
  }

  std::pair<float, float> getCurrent() {
    return std::make_pair(slices[0].getCurrent(), slices[1].getCurrent());
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

/*
 * ルーパーへの操作を行う。
 */
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
  bool is_ext_sync = false;
  float bpm = default_bpm;
  uint32_t tim_period = calc_tim_period(default_bpm);
  float recorded_speed = 1.0f; // 録音時のベーススピード
  bool is_quantized_rec = false;
  Tim tim; // 32bit timer

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
    tim.setCount(0);
    tim.enable();
    is_run = true;
  }

  void stop() {
    tim.disable();
    is_run = false;
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
      recorded_speed = speed;
      if (is_ext_sync) {
        is_quantized_rec = true;
      }
      reset();
      start();
      tracks[current_track_idx].startRec();
    }
  }

  void stopRec() {
    // TODO:
    is_rec = false;
    tracks[current_track_idx].endRec();
  }

  void reset() {
    if (is_rec) {
      stopRec();
    }
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

    auto current_values = tracks[current_track_idx].getCurrent();
    if (is_rec) {
      for(size_t i = 0; i < size; i += 2) {
        tracks[current_track_idx].step(1.0f);
        if (tracks[current_track_idx].isOverrap()) {
          // RAMの空きがなくなった
          is_rec = false;
          tracks[current_track_idx].endRec();
        } else {
          tracks[current_track_idx].setCurrent(
              dry_vol * in[i] + fdbk * current_values.first,
              dry_vol * in[i+1] + fdbk * current_values.second
              );
        }
        out[i] = dry_vol * in[i] + wet_vol * current_values.first;
        out[i+1] = dry_vol * in[i + 1] + wet_vol * current_values.second;
      }
    } else {
      for(size_t i = 0; i < size; i += 2) {
        tracks[current_track_idx].step(1.0f);
        out[i] = dry_vol * in[i] + wet_vol * current_values.first;
        out[i+1] = dry_vol * in[i + 1] + wet_vol * current_values.second;
      }
    }
  }
};

extern Looper looper;

extern "C" {

void TIM_LOOPER_CLOCK_HANDLER();

}

#endif /* LR_LOOPER_H */
