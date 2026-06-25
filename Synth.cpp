#include <Arduino.h>
#include <math.h>
#include "Synth.h"

namespace DrumOS {
namespace Synth {

static constexpr float TWO_PI_F = 6.28318530f;

void createTom(int16_t* buffer, int samples, int sampleRate, float startFreq, float endFreq, float decay) {
  float duration = (float)samples / sampleRate;

  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;
    float progress = t / duration;

    float freq = startFreq + (endFreq - startFreq) * progress;
    float amp = exp(-decay * progress);

    buffer[i] = (int16_t)(amp * 28000.0f * sin(TWO_PI_F * freq * t));
  }
}

void createNoise(int16_t* buffer, int samples, int sampleRate, float decay, float volume) {
  float duration = (float)samples / sampleRate;

  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;
    float progress = t / duration;

    float noise = ((float)random(0, 20000) / 10000.0f) - 1.0f;
    float amp = exp(-decay * progress);

    buffer[i] = (int16_t)(noise * amp * volume);
  }
}

void createSnare(int16_t* buffer, int samples, int sampleRate) {
  float duration = (float)samples / sampleRate;

  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;
    float progress = t / duration;

    float noise = ((float)random(0, 20000) / 10000.0f) - 1.0f;
    float tone1 = sin(TWO_PI_F * 180.0f * t);
    float tone2 = sin(TWO_PI_F * 330.0f * t);

    float amp = exp(-8.0f * progress);

    float sample = (noise * 0.75f) + (tone1 * 0.15f) + (tone2 * 0.10f);
    buffer[i] = (int16_t)(sample * amp * 22000.0f);
  }
}

void createKick(int16_t* buffer, int samples, int sampleRate) {
  float duration = (float)samples / sampleRate;

  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;
    float progress = t / duration;

    float freq = 95.0f + (38.0f - 95.0f) * progress;
    float amp = exp(-4.2f * progress);

    float body = sin(TWO_PI_F * freq * t);

    float click = 0.0f;
    if (i < 220) {
      float n = ((float)random(0, 20000) / 10000.0f) - 1.0f;
      click = n * (1.0f - (i / 220.0f)) * 0.35f;
    }

    float sample = (body * 1.25f) + click;
    buffer[i] = (int16_t)(sample * amp * 32000.0f);
  }
}

}
}