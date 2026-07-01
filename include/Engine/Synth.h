#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace DrumOS {
namespace Synth {

struct Harmonic {
  float freq;
  float amp;
  float phase;
};

struct VoiceDef {
  const char* name;
  const Harmonic* harmonics;
  int harmonicCount;

  float noise;
  float attack;
  float decay;
  float volume;
};

void generateAdditive(int16_t* buffer, int samples, int sampleRate, const VoiceDef& def);

void createCrash(int16_t* buffer, int samples, int sampleRate);
void createHiHat(int16_t* buffer, int samples, int sampleRate);
void createHiTom(int16_t* buffer, int samples, int sampleRate);
void createLowTom(int16_t* buffer, int samples, int sampleRate);
void createKick(int16_t* buffer, int samples, int sampleRate);
void createSnare(int16_t* buffer, int samples, int sampleRate);
void createTom(int16_t* buffer, int samples, int sampleRate, float startFreq, float endFreq, float decay);
void createNoise(int16_t* buffer, int samples, int sampleRate, float decay, float volume);

}
}