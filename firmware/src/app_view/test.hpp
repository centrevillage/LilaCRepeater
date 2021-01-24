#ifndef LR_APP_VIEW_TEST_H
#define LR_APP_VIEW_TEST_H

struct AppViewTest {
  bool is_dirty = true;

  float dry_vol = 0.0f;
  float wet_vol = 0.0f;
  float pan = 0.0f;
  float length = 0.0f;
  float pos = 0.0f;
  float fdbk = 0.0f;
  float speed = 0.0f;

  bool button_states[9];
  bool trig_states[9];

  void init() {
    is_dirty = true;
    app_oled.drawFillBG();
    app_oled.drawTextMedium("LilaC Repeater", 14, 3, 0);
    app_oled.process();
  }

  void update() {
    char text_buf[24];
    app_oled.drawFillBG();

    if (trig_states[0]) {
      app_oled.drawTextSmall("SYN", 3, 0, 64);
    }
    if (trig_states[4]) {
      app_oled.drawTextSmall("EXT", 3, 0, 96);
    }
    if (trig_states[1]) {
      app_oled.drawTextSmall("RUN", 3, 1, 32);
    }
    if (trig_states[2]) {
      app_oled.drawTextSmall("RST", 3, 1, 64);
    }
    if (trig_states[3]) {
      app_oled.drawTextSmall("REC", 3, 1, 96);
    }

    snprintf(text_buf, sizeof(text_buf), "%3d", (uint8_t)(pos * 128.0f));
    app_oled.drawTextSmall(text_buf, sizeof(text_buf), 3, 42);

    snprintf(text_buf, sizeof(text_buf), "%3d", (uint8_t)(length * 128.0f));
    app_oled.drawTextSmall(text_buf, sizeof(text_buf), 4, 42);

    snprintf(text_buf, sizeof(text_buf), "%3d", (uint8_t)(pan * 128.0f));
    app_oled.drawTextSmall(text_buf, sizeof(text_buf), 5, 42);

    snprintf(text_buf, sizeof(text_buf), "%3d", (uint8_t)(fdbk * 128.0f));
    app_oled.drawTextSmall(text_buf, sizeof(text_buf), 4, 0);

    snprintf(text_buf, sizeof(text_buf), "%3d", (uint8_t)(speed * 128.0f));
    app_oled.drawTextSmall(text_buf, sizeof(text_buf), 4, 84);

    snprintf(text_buf, sizeof(text_buf), "%3d", (uint8_t)(dry_vol * 128.0f));
    app_oled.drawTextSmall(text_buf, sizeof(text_buf), 5, 0);

    snprintf(text_buf, sizeof(text_buf), "%3d", (uint8_t)(wet_vol * 128.0f));
    app_oled.drawTextSmall(text_buf, sizeof(text_buf), 5, 84);

    if (button_states[0]) {
      app_oled.drawTextSmall("T1", 2, 6, 0);
    }
    if (button_states[1]) {
      app_oled.drawTextSmall("T2", 2, 6, 32);
    }
    if (button_states[2]) {
      app_oled.drawTextSmall("T3", 2, 6, 64);
    }
    if (button_states[3]) {
      app_oled.drawTextSmall("T4", 2, 6, 96);
    }
    if (button_states[4]) {
      app_oled.drawTextSmall("MUT", 3, 7, 0);
    }
    if (button_states[5]) {
      app_oled.drawTextSmall("REC", 3, 7, 25);
    }
    if (button_states[6]) {
      app_oled.drawTextSmall("CLR", 3, 7, 50);
    }
    if (button_states[7]) {
      app_oled.drawTextSmall("REV", 3, 7, 75);
    }
    if (button_states[8]) {
      app_oled.drawTextSmall("RUN", 3, 7, 100);
    }

    app_oled.process();
  }

  void process() {
    if (is_dirty) {
      update();
      is_dirty = false;
    }
  }
};

#endif /* LR_APP_VIEW_TEST_H */
