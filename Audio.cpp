#include <Arduino.h>
#include <driver/i2s.h>
#include "Audio.h"

#define I2S_BCK_PIN   26
#define I2S_LRC_PIN   25
#define I2S_DOUT_PIN  22

namespace DrumOS {
namespace Audio {

struct Voice {
  int16_t* buffer;
  int samples;
  int pos;
  int volume;
  bool active;
};

static int masterVolume = 100;
static Voice voices[MAX_VOICES];

void begin() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 128,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK_PIN,
    .ws_io_num = I2S_LRC_PIN,
    .data_out_num = I2S_DOUT_PIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);

  for (int i = 0; i < MAX_VOICES; i++) {
    voices[i].active = false;
  }
}

void playBuffer(int16_t* buffer, int samples, int volume) {
  int freeVoice = -1;

  for (int i = 0; i < MAX_VOICES; i++) {
    if (!voices[i].active) {
      freeVoice = i;
      break;
    }
  }

  if (freeVoice < 0) freeVoice = 0;

  voices[freeVoice].buffer = buffer;
  voices[freeVoice].samples = samples;
  voices[freeVoice].pos = 0;
  voices[freeVoice].volume = constrain(volume, 0, 127);
  voices[freeVoice].active = true;
}

void process() {
  int16_t out[AUDIO_BLOCK * 2];

  for (int i = 0; i < AUDIO_BLOCK; i++) {
    int32_t mix = 0;

    for (int v = 0; v < MAX_VOICES; v++) {
      if (!voices[v].active) continue;

      if (voices[v].pos >= voices[v].samples) {
        voices[v].active = false;
        continue;
      }

      int32_t s = voices[v].buffer[voices[v].pos++];
      s = (s * voices[v].volume) / 127;

      mix += s;
    }

    mix = (mix * masterVolume) / 100;
    mix = constrain(mix, -26000, 26000);

    out[i * 2]     = (int16_t)mix;
    out[i * 2 + 1] = (int16_t)mix;
  }

  size_t written;
  i2s_write(I2S_NUM_0, out, sizeof(out), &written, portMAX_DELAY);
}

void setMasterVolume(int volume) {
  masterVolume = constrain(volume, 0, 100);
}

int getMasterVolume() {
  return masterVolume;
}

}
}