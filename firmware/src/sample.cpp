#include "dev/sdram.h"
#include "sample.hpp"

static float DSY_SDRAM_BSS _buffer[sample_max_buffer_size];

void Sample::init() {
  for (size_t i = 0; i < sample_max_buffer_size; ++i) {
    _buffer[i] = 0; // todo: memset?
  }
}

SampleSlice Sample::slice(size_t start, size_t size) const {
  return SampleSlice {
    .size = size,
    .buf = _buffer + start
  };
}

Sample sample;
