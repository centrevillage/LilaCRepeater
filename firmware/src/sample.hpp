#ifndef LR_SAMPLE_H
#define LR_SAMPLE_H

#include <cstddef>
#include <cassert>
#include <utility>
#include <array>

constexpr uint16_t sample_rate = 48000;

//constexpr size_t sample_max_buffer_size = (48000 * 60 * 5) // 5 minutes of floats at 48 khz
constexpr size_t sample_max_buffer_size = (1024 * 1024 * 64 / sizeof(float)); // 64MB

struct SampleSlice {
  enum class OverrapType : uint8_t {
    none = 0,
    by_size,
    by_end,
    by_loop_length
  };

  size_t size;
  float* buf;
  float  end_pos = 0.0f;
  float  loop_length = 0.0f;
  float  offset = 0.0f;
  float _pos = 0.0f;
  bool is_rec = false;
  bool loop_range_enabled = false;
  float step_value = 1.0f;
  float feedback = 1.0f;
  size_t _erase_pos = 0; // 上書き録音時に初期化すべき次のサンプルの位置を保持
  OverrapType overrap = OverrapType::none;


  inline void initStepValue(float step) {
    step_value = step;
  }

  inline float getRawPos() const {
    return _pos;
  };

  inline bool isEmpty() const {
    return end_pos == 0.0f;
  }

  inline void setFeedback(float fb) {
    feedback = fb;
  }

  inline void setOffset(float ofst) {
    if (ofst < end_pos) {
      offset = ofst;
    }
  }

  inline float getFeedback() {
    return feedback;
  }

  inline void startRec() {
    is_rec = true;
  }

  inline void endRec() {
    is_rec = false;
    end_pos = _pos;
    reset();
  }

  inline void reset() {
    _pos = 0.0f;
    overrap = OverrapType::none;
  }

  inline void jump(float new_pos) {
    if (new_pos < end_pos) {
      _pos = new_pos;
      overrap = OverrapType::none;
    } else {
      reset();
    }
  }

  inline void step(float v) {
    step_value = v;
    volatile float tmp = _pos;
    tmp += v;
    if (tmp > (float)size) {
      tmp -= size;
      overrap = OverrapType::by_size;
    } else if (end_pos > 0.0f && tmp > end_pos) {
      overrap = OverrapType::by_end;
    } else if (loop_range_enabled && tmp > loop_length) {
      tmp -= loop_length;
      overrap = OverrapType::by_loop_length;
    } else {
      overrap = OverrapType::none;
    }
    _pos = tmp;
  }

  inline float get(float pos) const {
    // linear interporation
    size_t spos = (size_t)pos;
    size_t epos = (spos+1) % size;
    float a = pos - (float)spos;
    return (1.0f - a) * buf[spos] + a * buf[epos];
  }

  inline float getPos() const {
    if (!loop_range_enabled || offset == 0.0f) {
      return _pos;
    }
    float p = _pos + offset;
    if (p > end_pos) {
      p -= end_pos;
    }
    return p;
  }

  inline float getCurrent() const {
    return get(getPos());
  }

  void recValueAndStep(float step_, float value) {
    step_value = step_;
    float p = getPos();
    size_t new_erase_pos = (size_t)p;
    if (new_erase_pos != _erase_pos) {
      // 消去後に書き込みが一回しか行われずにサンプルの録音が半端になるケースが考えられるけど、
      // 別途録音開始/終了付近のスムージングをやれば良い様な気はする。
      for (size_t i = _erase_pos; i <= new_erase_pos; ++i) {
        buf[i] = buf[i] * feedback;
      }
      _erase_pos = new_erase_pos;
    }
    set(p, value);
    step(step_);
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

  inline void setCurrent(float value) {
    set(getPos(), value);
  }

  inline void clear(size_t pos) {
    buf[pos] = 0.0f;
  }

  inline void setRaw(size_t pos, float value) {
    buf[pos] = value;
  }

  inline float getRaw(size_t pos) const {
    return buf[pos];
  }

  inline SampleSlice slice(size_t start, size_t size) const {
    return SampleSlice {
      .size = size,
      .buf = buf + start
    };
  }
};

struct Sample {
  void init();
  SampleSlice slice(size_t start, size_t size) const;
};

extern Sample sample;

#endif /* LR_SAMPLE_H */
