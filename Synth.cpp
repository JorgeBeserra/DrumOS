#include <Arduino.h>
#include <math.h>
#include "Synth.h"

namespace DrumOS {
namespace Synth {

static constexpr float TWO_PI_F = 6.28318530f;

void createCrash(int16_t* buffer, int samples, int sampleRate) {
  float duration = (float)samples / sampleRate;

  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;
    float progress = t / duration;

    // Envelope longo, mais natural para prato
    float env = exp(-3.0f * progress);

    // Ruído principal
    float noise = ((float)random(0, 20000) / 10000.0f) - 1.0f;

    // Harmônicos metálicos, mas bem misturados
    float metal =
      sin(6.28318530f * 4200.0f * t) * 0.18f +
      sin(6.28318530f * 5870.0f * t) * 0.14f +
      sin(6.28318530f * 7350.0f * t) * 0.10f +
      sin(6.28318530f * 9100.0f * t) * 0.08f +
      sin(6.28318530f * 11700.0f * t) * 0.05f;

    // Ataque inicial curto, mas sem "martelada"
    float attack = 0.0f;
    if (i < 300) {
      float n = ((float)random(0, 20000) / 10000.0f) - 1.0f;
      attack = n * (1.0f - (i / 300.0f)) * 0.20f;
    }

    float sample = (noise * 0.75f + metal + attack) * env;

    sample *= 18000.0f;
    sample = constrain(sample, -30000.0f, 30000.0f);

    buffer[i] = (int16_t)sample;
  }
}

void createHiHat(int16_t* buffer, int samples, int sampleRate) {
  static Harmonic hatHarmonics[] = {
    {6000.0f, 1.00f, 0.0f},
    {8200.0f, 0.75f, 0.0f},
    {10500.0f, 0.50f, 0.0f},
    {12500.0f, 0.35f, 0.0f}
  };

  VoiceDef hat = {
    "Additive HiHat",
    hatHarmonics,
    4,
    0.90f,
    0.001f,
    12.0f,
    18000.0f
  };

  generateAdditive(buffer, samples, sampleRate, hat);
}

void createHiTom(int16_t* buffer, int samples, int sampleRate) {
  static Harmonic tomHarmonics[] = {
    {220.0f, 1.00f, 0.0f},
    {440.0f, 0.35f, 0.0f},
    {660.0f, 0.16f, 0.0f}
  };

  VoiceDef tom = {
    "Additive HiTom",
    tomHarmonics,
    3,
    0.03f,
    0.002f,
    5.5f,
    24000.0f
  };

  generateAdditive(buffer, samples, sampleRate, tom);
}

void createLowTom(int16_t* buffer, int samples, int sampleRate) {
  static Harmonic tomHarmonics[] = {
    {115.0f, 1.00f, 0.0f},
    {230.0f, 0.32f, 0.0f},
    {345.0f, 0.14f, 0.0f}
  };

  VoiceDef tom = {
    "Additive LowTom",
    tomHarmonics,
    3,
    0.03f,
    0.002f,
    4.8f,
    26000.0f
  };

  generateAdditive(buffer, samples, sampleRate, tom);
}


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

void generateAdditive(int16_t* buffer, int samples, int sampleRate, const VoiceDef& def) {
  float duration = (float)samples / sampleRate;

  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;
    float progress = t / duration;

    float env;

    if (progress < def.attack) {
      env = progress / def.attack;
    } else {
      float decayProgress = (progress - def.attack) / max(0.001f, 1.0f - def.attack);
      env = exp(-def.decay * decayProgress);
    }

    float sample = 0.0f;

    for (int h = 0; h < def.harmonicCount; h++) {
      sample += sin(TWO_PI_F * def.harmonics[h].freq * t + def.harmonics[h].phase)
                * def.harmonics[h].amp;
    }

    if (def.noise > 0.0f) {
      float n = ((float)random(0, 20000) / 10000.0f) - 1.0f;
      sample += n * def.noise;
    }

    sample *= env * def.volume;

    sample = constrain(sample, -32767.0f, 32767.0f);
    buffer[i] = (int16_t)sample;
  }
}

void createSnare(int16_t* buffer, int samples, int sampleRate) {
  static Harmonic snareHarmonics[] = {
    {180.0f, 0.35f, 0.0f},
    {330.0f, 0.25f, 0.0f},
    {520.0f, 0.12f, 0.0f}
  };

  VoiceDef snare = {
    "Additive Snare",
    snareHarmonics,
    3,
    0.95f,   // bastante ruído
    0.005f,
    8.0f,
    22000.0f
  };

  generateAdditive(buffer, samples, sampleRate, snare);
}

void createKick(int16_t* buffer, int samples, int sampleRate) {
  float duration = (float)samples / sampleRate;

  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;
    float progress = t / duration;

    // Pitch descendo rápido: ataque forte de bumbo
    float freq = 75.0f * exp(-7.0f * progress) + 42.0f;

    // Envelope do corpo
    float bodyEnv = exp(-5.2f * progress);

    // Corpo principal
    float body = sin(6.28318530f * freq * t);

    // Sub grave fixo para dar peso
    float sub = sin(6.28318530f * 48.0f * t) * exp(-2.4f * progress) * 0.85f;

    // Click curto no início
    float click = 0.0f;
    if (i < 180) {
      float n = ((float)random(0, 20000) / 10000.0f) - 1.0f;
      click = n * (1.0f - (i / 180.0f)) * 0.10f;
    }

    float sample = (body * bodyEnv * 1.15f) + sub + click;

    sample *= 30000.0f;
    sample = constrain(sample, -32767.0f, 32767.0f);

    buffer[i] = (int16_t)sample;
  }
}

}
}