#include <stdio.h>
#include <string.h>
#include "daisy_seed.h"
//#include "hid_oled_display.h"

using namespace daisy;

DaisySeed hardware;
OledDisplay display;

bool is_dirty = true;
char text_buf[128];

enum {
  SLIDER_DRY_VOL = 0,
  SLIDER_TRACK_VOL,
  SLIDER_PAN,
  SLIDER_LENGTH,
  SLIDER_POS,
  SLIDER_FDBK,
  SLIDER_SPEED,

  SLIDER_COUNT
};

float slider_values[SLIDER_COUNT];

#define SLIDER_DRY_VOL_PIN 22
#define SLIDER_TRACK_VOL_PIN 21
#define SLIDER_PAN_PIN 20
#define SLIDER_LENGTH_PIN 19
#define SLIDER_POS_PIN 18
#define SLIDER_FDBK_PIN 17
#define SLIDER_SPEED_PIN 16

static dsy_gpio pin_cs;

static inline void setup_oled() {
  pin_cs.mode = DSY_GPIO_MODE_OUTPUT_PP;
  pin_cs.pin  = hardware.GetPin(12);
  dsy_gpio_init(&pin_cs);
  dsy_gpio_write(&pin_cs, 0);

  dsy_gpio_pin oled_pins[OledDisplay::NUM_PINS];
  oled_pins[OledDisplay::DATA_COMMAND] = hardware.GetPin(8);
  oled_pins[OledDisplay::RESET] = hardware.GetPin(13);
  display.Init(oled_pins);
}

static inline void setup_adc() {
  //This is our ADC configuration
  AdcChannelConfig adcConfigs[SLIDER_COUNT];
  adcConfigs[SLIDER_DRY_VOL].InitSingle(hardware.GetPin(SLIDER_DRY_VOL_PIN));
  adcConfigs[SLIDER_TRACK_VOL].InitSingle(hardware.GetPin(SLIDER_TRACK_VOL_PIN));
  adcConfigs[SLIDER_PAN].InitSingle(hardware.GetPin(SLIDER_PAN_PIN));
  adcConfigs[SLIDER_LENGTH].InitSingle(hardware.GetPin(SLIDER_LENGTH_PIN));
  adcConfigs[SLIDER_POS].InitSingle(hardware.GetPin(SLIDER_POS_PIN));
  adcConfigs[SLIDER_FDBK].InitSingle(hardware.GetPin(SLIDER_FDBK_PIN));
  adcConfigs[SLIDER_SPEED].InitSingle(hardware.GetPin(SLIDER_SPEED_PIN));

  //Initialize the adc with the config we just made
  hardware.adc.Init(adcConfigs, SLIDER_COUNT);
  //Start reading values
  hardware.adc.Start();
}

static inline void setup() {
  hardware.Configure();
  hardware.Init();

  setup_adc();
  setup_oled();
}

static inline void loop() {
  for (uint8_t i = 0; i < SLIDER_COUNT; ++i) {
    // TODO: LPFかけて読み出すべき？
    float value = hardware.adc.GetFloat(i);
    if (slider_values[i] != value) {
      slider_values[i] = value;
      is_dirty = true;
    }
  }
  if (is_dirty) {
    display.Fill(true);
    display.SetCursor(0, 0);
    sprintf(text_buf, "%f", slider_values[SLIDER_DRY_VOL]);
    display.WriteString(text_buf, Font_11x18, false);
    display.Update();
  }
}

int main(void) {
  setup();
  for(;;) {
    loop();
    //wait 1 ms
    dsy_system_delay(1);
  }
}
