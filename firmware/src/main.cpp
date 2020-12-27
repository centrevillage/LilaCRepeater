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

//#include "stm32h7xx_ll_spi.h"
//#include "stm32h7xx_ll_gpio.h"

#include "app_input.hpp"
#include "app_view.hpp"

// TODO: remove daisy lib
using namespace daisy;
using namespace igb::stm32;
using namespace igb::sdk;
using namespace igb::daisy;

SdmmcHandler sdcard;
WavPlayer    sampler;

static void AudioCallback(float *in, float *out, size_t size) {
  // thru
  for(size_t i = 0; i < size; i += 2) {
    // left out
    out[i] = in[i];
    // right out
    out[i + 1] = in[i + 1];
  }

  //// wav player
  //for(size_t i = 0; i < size; i += 2)
  //{
  //    out[i] = out[i + 1] = s162f(sampler.Stream()) * 0.5f;
  //}
}

static inline void setup() {
  hardware.Configure();
  hardware.Init();

  input.init();
  view.init();
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
    dsy_system_delay(1);
  }
}

// wav player
//int main(void) {
//    // Init hardware
//    //size_t blocksize = 48;
//    size_t blocksize = 128;
//    hardware.Init();
//    //    hardware.ClearLeds();
//    sdcard.Init();
//    sampler.Init();
//    sampler.SetLooping(true);
//
//    // SET LED to indicate Looping status.
//    //hardware.SetLed(DaisyPatch::LED_2_B, sampler.GetLooping());
//
//    //sampler.Open(0);
//    // Init Audio
//    hardware.SetAudioBlockSize(blocksize);
//    hardware.StartAudio(AudioCallback);
//    // Loop forever...
//
//    for(;;)
//    {
//        // Prepare buffers for sampler as needed
//        sampler.Prepare();
//    }
//}
