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
  float pos = 0.0f;
  bool overdub = true;

  // TODO: ループサイズ可変
  void init() {
    slice = sample.slice(0, sample_max_buffer_size / 4);
  }

  // 1 sample tickごとのサンプルテープ進行量
  void step(float v) {
    volatile float tmp = pos;
    tmp += v;
    if (tmp > slice.sizeStereo()) {
      tmp -= slice.sizeStereo();
    }
    pos = tmp;
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
  bool run = false;
  bool is_rec = false;
  float bpm = default_bpm;
  uint32_t tim_period = calc_tim_period(default_bpm);
  Tim tim; // 32bit timer

  void init() {
    tim = Tim::newIntervalTimer(TIM_LOOPER_CLOCK_TYPE, 0, tim_period, 0);
    tim.setCount(0);
  }

  void start() {
    tim.setCount(0);
    tim.enable();
    run = true;
  }

  void stop() {
    tim.disable();
    run = false;
  }

  // モードによって挙動が変わる。
  // default:
  //  何もレコーディングされていない場合、Track1にレコーディング開始と内部クロックのSTART
  //  レコーディング済みの場合、Overdub開始/停止
  void rec() {
    // TODO: quantize
    if (is_rec) {
      is_rec = false;
    } else {
      is_rec = true;
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
    if (!run) {
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
    }
  }
};

extern Looper looper;

extern "C" {

void TIM_LOOPER_CLOCK_HANDLER();

}

#endif /* LR_LOOPER_H */
