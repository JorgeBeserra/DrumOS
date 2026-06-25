#include <Arduino.h>
#include <math.h>
#include "Click.h"
#include "Audio.h"

namespace DrumOS {
namespace Click {

static int sampleRate = 44100;
static int bpm = 120;
static bool enabled = false;
static int beat = 0;
static unsigned long lastMs = 0;

static const int DUR_CLICK_MS = 40;

static int samples = 0;
static int16_t* bufferStrong = nullptr;
static int16_t* bufferWeak = nullptr;

static void createClick(int16_t* buffer, int samples, float freq, float volume) {
  float duration = (float)samples / sampleRate;

  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;
    float progress = t / duration;

    float amp = exp(-18.0f * progress);
    float tone = sin(2.0f * 3.14159265f * freq * t);

    buffer[i] = (int16_t)(tone * amp * volume);
  }
}

void begin(int sr) {
  sampleRate = sr;
  samples = (sampleRate * DUR_CLICK_MS) / 1000;

  bufferStrong = new int16_t[samples];
  bufferWeak = new int16_t[samples];

  createClick(bufferStrong, samples, 1800.0f, 20000.0f);
  createClick(bufferWeak, samples, 1200.0f, 14000.0f);

  bpm = 120;
  enabled = false;
  beat = 0;
  lastMs = millis();
}

void process() {
  if (!enabled) return;

  unsigned long now = millis();
  unsigned long interval = 60000UL / bpm;

  while (now - lastMs >= interval) {
    lastMs += interval;

    bool strong = beat == 0;

    DrumOS::Audio::playBuffer(
      strong ? bufferStrong : bufferWeak,
      samples,
      strong ? 120 : 85
    );

    beat = (beat + 1) % 4;
  }
}

void setEnabled(bool value) {
  enabled = value;
  beat = 0;
  lastMs = millis() - (60000UL / bpm);
}

bool isEnabled() {
  return enabled;
}

void setBpm(int value) {
  bpm = constrain(value, 40, 240);
}

int getBpm() {
  return bpm;
}

}
}