#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace DrumOS {
namespace Synth {

void createKick(int16_t* buffer, int samples, int sampleRate);
void createSnare(int16_t* buffer, int samples, int sampleRate);
void createTom(int16_t* buffer, int samples, int sampleRate, float startFreq, float endFreq, float decay);
void createNoise(int16_t* buffer, int samples, int sampleRate, float decay, float volume);

}
}