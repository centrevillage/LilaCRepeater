#ifndef LR_SAMPLE_H
#define LR_SAMPLE_H

#include <cstddef>
#include <cassert>

//constexpr size_t sample_max_buffer_size = (48000 * 60 * 5) // 5 minutes of floats at 48 khz
constexpr size_t sample_max_buffer_size = (1024 * 1024 * 64 / sizeof(float)); // 64MB

struct SampleSlice {
  size_t size;
  float* buf;

  float get(float pos) {
    // linear interporation
    size_t spos = (size_t)pos;
    size_t epos = (spos+1) % size;
    float a = pos - (float)spos;
    return (1.0f - a) * buf[spos] + a * buf[epos];
  }
  
  void set(float pos, float value) {
    buf[(size_t)pos] = value;
  }

  // get left value on stereo mode
  float getL(float pos) {
    assert(pos * 2 < size);
    // linear interporation
    size_t spos = (size_t)(2.0f * pos);
    size_t epos = (spos+2) % size;
    float a = pos - (float)((size_t)pos);
    return (1.0f - a) * buf[spos] + a * buf[epos];
  }

  void setL(float pos, float value) {
    buf[(size_t)pos*2] = value;
  }
  void setR(float pos, float value) {
    buf[(size_t)pos*2+1] = value;
  }

  // get right value on stereo mode
  float getR(float pos) {
    // linear interporation
    size_t spos = (size_t)(2.0f * pos) + 1;
    size_t epos = (spos+2) % size;
    float a = pos - (float)((size_t)pos);
    return (1.0f - a) * buf[spos] + a * buf[epos];
  }

  float getRow(size_t pos) {
    return buf[pos];
  }

  inline float sizeStereo() {
    return size / 2;
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
