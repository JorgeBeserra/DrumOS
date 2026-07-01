#include <Arduino.h>
#include <pgmspace.h>
#include "Engine/Audio.h"
#include "Drivers/PCM5102.h"

namespace DrumOS {
namespace Audio {

struct Voice {
  const int16_t* buffer;
  int samples;
  int pos;
  int volume;
  bool active;
};

static int masterVolume = 100;
static Voice voices[MAX_VOICES];

void begin() {
  DrumOS::Drivers::PCM5102::begin();

  for (int i = 0; i < MAX_VOICES; i++) {
    voices[i].active = false;
    voices[i].buffer = nullptr;
    voices[i].samples = 0;
    voices[i].pos = 0;
    voices[i].volume = 0;
  }
}

void playBuffer(const int16_t* buffer, int samples, int volume) {
  if (buffer == nullptr || samples <= 0) return;

  int freeVoice = -1;

  for (int i = 0; i < MAX_VOICES; i++) {
    if (!voices[i].active) {
      freeVoice = i;
      break;
    }
  }

  // Se todas as vozes estiverem ocupadas, rouba a voz 0
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

      int16_t rawSample = (int16_t)pgm_read_word(&voices[v].buffer[voices[v].pos]);
      voices[v].pos++;

      int32_t s = rawSample;
      s = (s * voices[v].volume) / 127;

      mix += s;
    }

    mix = (mix * masterVolume) / 100;
    mix = constrain(mix, -26000, 26000);

    out[i * 2]     = (int16_t)mix;
    out[i * 2 + 1] = (int16_t)mix;
  }

  size_t written;
  DrumOS::Drivers::PCM5102::writeStereoBlock(out, AUDIO_BLOCK * 2);
}

void setMasterVolume(int volume) {
  masterVolume = constrain(volume, 0, 100);
}

int getMasterVolume() {
  return masterVolume;
}

}
}