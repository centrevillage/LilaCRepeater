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
  SampleSlice slice;

  // TODO: ループサイズ可変
  void init() {
    slice = sample.slice(0, sample_max_buffer_size / 4);
  }

  // 1 sample tickごとのサンプルテープ進行量
  void step(float v) {
    slice.step(v);
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

  std::array<LooperTrack, track_size> tracks;
  uint8_t current_track_idx = 0;
  bool is_run = false;
  bool is_rec = false;
  float bpm = default_bpm;
  uint32_t tim_period = calc_tim_period(default_bpm);
  Tim tim; // 32bit timer

  void init() {
    sample.init();
    tim = Tim::newIntervalTimer(TIM_LOOPER_CLOCK_TYPE, 0, tim_period, 0);
    tim.setCount(0);
    for (auto& track : tracks) {
      track.init();
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

  // モードによって挙動が変わる。
  // default:
  //  何もレコーディングされていない場合、Track1にレコーディング開始と内部クロックのSTART
  //  レコーディング済みの場合、Overdub開始/停止
  void rec() {
    // TODO: quantize

    // とりあえずテストで録音始めたら自動でRunするようにしとく
    if (is_rec) {
      is_rec = false;
      tracks[current_track_idx].slice.endRec();
      //if (is_run) {
      //  stop();
      //}
    } else {
      is_rec = true;
      if (!is_run) {
        start();
        tracks[current_track_idx].slice.startRec();
      }
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

    if (is_rec) {
      for(size_t i = 0; i < size; i += 2) {
        tracks[current_track_idx].slice.step(1.0f);
        if (tracks[current_track_idx].slice.is_overrap) {
          // 1ループ録音完了
          is_rec = false;
          tracks[current_track_idx].slice.endRec();
        } else {
          tracks[current_track_idx].slice.setCurrent(in[i]);
        }
        out[i] = 0.5 * in[i] + 0.5 * tracks[current_track_idx].slice.getCurrent();
        out[i+1] = 0.5 * in[i + 1] + 0.5 * tracks[current_track_idx].slice.getCurrent();
      }
    } else {
      for(size_t i = 0; i < size; i += 2) {
        tracks[current_track_idx].slice.step(1.0f);
        out[i] = 0.5 * in[i] + 0.5 * tracks[current_track_idx].slice.getCurrent();
        out[i+1] = 0.5 * in[i + 1] + 0.5 * tracks[current_track_idx].slice.getCurrent();
      }
    }
  }
};

extern Looper looper;

extern "C" {

void TIM_LOOPER_CLOCK_HANDLER();

}

#endif /* LR_LOOPER_H */
