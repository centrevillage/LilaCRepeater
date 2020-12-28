#include <stdio.h>
#include <string.h>

#include "daisy_seed.h"
#include "hardware.hpp"

#include <igb_stm32/base.hpp>
#include <igb_stm32/periph/systick.hpp>
#include <igb_stm32/periph/gpio.hpp>
#include <igb_stm32/periph/spi.hpp>
#include <igb_sdk/device/oled_ssd1306.hpp>
#include <igb_daisy/periph/gpio.hpp>

#include <igb_stm32/periph/flash.hpp>
#include <igb_stm32/periph/nvic.hpp>
#include <igb_stm32/periph/rcc.hpp>
#include <igb_stm32/periph/tim.hpp>
#include <igb_sdk/util/text.h>

#include "app_input.hpp"
#include "app_view.hpp"
#include "looper.hpp"

// TODO: remove daisy lib
using namespace daisy;
using namespace igb::stm32;
using namespace igb::sdk;
using namespace igb::daisy;

SdmmcHandler sdcard;
WavPlayer    sampler;

static void AudioCallback(float *in, float *out, size_t size) {
  looper.processAudio(in, out, size);
}

static inline void setup() {
  hardware.Configure();
  hardware.Init();

  looper.init();
  view.init();
  input.init();
}

static inline void loop() {
  input.process();
  view.process();
}

int main(void) {
  setup();

  hardware.StartAudio(AudioCallback);
  for(;;) {
    loop();
    //wait 1 ms
    //dsy_system_delay(1);
    System::Delay(1);
  }
}
