#pragma once

#include <Arduino.h>
#include <stdint.h>

#define SAMPLE_RATE 44100
#define AUDIO_BLOCK 64
#define MAX_VOICES 16

namespace DrumOS {
namespace Audio {

void begin();
void process();

void playBuffer(int16_t* buffer, int samples, int volume);

void setMasterVolume(int volume);
int getMasterVolume();

}
}