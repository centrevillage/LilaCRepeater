#ifndef LR_SAMPLE_H
#define LR_SAMPLE_H

#include <cstddef>
#include <cassert>
#include <utility>

//constexpr size_t sample_max_buffer_size = (48000 * 60 * 5) // 5 minutes of floats at 48 khz
constexpr size_t sample_max_buffer_size = (1024 * 1024 * 64 / sizeof(float)); // 64MB

// stereo をLRの連続領域として扱うとめんどいので、別Sliceにする？
struct SampleSlice {
  size_t size;
  float* buf;
  float _pos = 0.0f;
  bool is_rec = false;
  float step_value = 1.0f;
  bool is_overrap = false;

  void startRec() {
    is_rec = true;
  }

  void endRec() {
    is_rec = false;
    size = _pos;
    reset();
  }

  void reset() {
    _pos = 0.0f;
    is_overrap = false;
  }

  void step(float v) {
    step_value = v;
    volatile float tmp = _pos;
    tmp += v;
    if (tmp > (float)size) {
      tmp -= size;
      is_overrap = true;
    } else {
      is_overrap = false;
    }
    _pos = tmp;
  }

  float get(float pos) {
    // linear interporation
    size_t spos = (size_t)pos;
    size_t epos = (spos+1) % size;
    float a = pos - (float)spos;
    return (1.0f - a) * buf[spos] + a * buf[epos];
  }

  float getCurrent() {
    return get(_pos);
  }
  
  // このメソッドでは常にoverdub想定なので、上書きしたい場合は別途クリア
  void set(float pos, float value) {
    // TODO: これ正しいのか？
    if (step_value > 1.0f) {
      size_t spos = (size_t)pos;
      // wrap した時の考慮
      size_t epos = (size_t)(pos + step_value);
      float a = pos - spos;
      float b = (pos + step_value) - (float)epos;
      buf[spos] += (1.0f - a) * value;
      buf[epos % size] += b * value;
      if (epos > spos + 1) {
        size_t count = epos - (spos + 1);
        // 飛ばされるサンプル分を埋める
        for (size_t i = 0; i < count; ++i) {
          buf[spos+1+i] += value;
        }
      }
    } else if (step_value == 1.0f) {
      buf[(size_t)pos] += value;
    } else {
      size_t spos = (size_t)pos;
      size_t epos = (spos+1) % size;
      float a = pos - (float)spos;
      buf[spos] += (value * (1.0f - a)) / step_value;
      buf[epos] += (value * a) / step_value;
    }
  }

  void setCurrent(float value) {
    set(_pos, value);
  }

  void clear(size_t pos) {
    buf[pos] = 0.0f;
  }

  void setRow(size_t pos, float value) {
    buf[pos] = value;
  }

  float getRow(size_t pos) {
    return buf[pos];
  }

  SampleSlice slice(size_t start, size_t size) {
    return SampleSlice {
      .size = size,
      .buf = buf + start
    };
  }
};

struct Sample {
  void init();
  SampleSlice slice(size_t start, size_t size);
};

extern Sample sample;

#endif /* LR_SAMPLE_H */
