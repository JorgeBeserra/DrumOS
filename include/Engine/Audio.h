#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "Core/Config.h"

#define SAMPLE_RATE  DRUMOS_SAMPLE_RATE
#define AUDIO_BLOCK  DRUMOS_AUDIO_BLOCK
#define MAX_VOICES   DRUMOS_MAX_VOICES

namespace DrumOS {
namespace Audio {

void begin();
void process();

void playBuffer(const int16_t* buffer, int samples, int volume);

void setMasterVolume(int volume);
int getMasterVolume();

}
}