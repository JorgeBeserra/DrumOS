#include <Arduino.h>
#include <driver/i2s.h>

#include "Drivers/PCM5102.h"
#include "Core/Config.h"

#ifndef DRUMOS_I2S_BCK_PIN
#define DRUMOS_I2S_BCK_PIN   26
#endif

#ifndef DRUMOS_I2S_LRC_PIN
#define DRUMOS_I2S_LRC_PIN   25
#endif

#ifndef DRUMOS_I2S_DOUT_PIN
#define DRUMOS_I2S_DOUT_PIN  22
#endif

#ifndef DRUMOS_SAMPLE_RATE
#define DRUMOS_SAMPLE_RATE   22050
#endif

namespace DrumOS {
namespace Drivers {
namespace PCM5102 {

static bool ready = false;

bool begin() {
  if (ready) {
    return true;
  }

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = DRUMOS_SAMPLE_RATE,
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
    .bck_io_num = DRUMOS_I2S_BCK_PIN,
    .ws_io_num = DRUMOS_I2S_LRC_PIN,
    .data_out_num = DRUMOS_I2S_DOUT_PIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  esp_err_t err;

  err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, nullptr);
  if (err != ESP_OK) {
    Serial.print("PCM5102: erro i2s_driver_install: ");
    Serial.println((int)err);
    return false;
  }

  err = i2s_set_pin(I2S_NUM_0, &pin_config);
  if (err != ESP_OK) {
    Serial.print("PCM5102: erro i2s_set_pin: ");
    Serial.println((int)err);
    i2s_driver_uninstall(I2S_NUM_0);
    return false;
  }

  i2s_zero_dma_buffer(I2S_NUM_0);

  ready = true;

  Serial.println("PCM5102: I2S iniciado");
  return true;
}

bool isReady() {
  return ready;
}

void writeStereoBlock(const int16_t* data, size_t samplesStereo) {
  if (!ready || data == nullptr || samplesStereo == 0) {
    return;
  }

  size_t bytesToWrite = samplesStereo * sizeof(int16_t);
  size_t bytesWritten = 0;

  i2s_write(
    I2S_NUM_0,
    data,
    bytesToWrite,
    &bytesWritten,
    portMAX_DELAY
  );
}

void clear() {
  if (!ready) {
    return;
  }

  i2s_zero_dma_buffer(I2S_NUM_0);
}

}
}
}