#include <Arduino.h>
#include <math.h>
#include "Engine/Synth.h"

namespace DrumOS {
namespace Synth {

static constexpr float TWO_PI_F = 6.28318530f;

void createCrash(int16_t* buffer, int samples, int sampleRate) {
  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;

    // Cauda longa do prato
    float tailEnv = expf(-t * 2.8f);

    // Ataque forte e curto a cada batida
    float attackEnv = expf(-t * 95.0f);

    // Ruído principal
    float noise = ((float)random(0, 20000) / 10000.0f) - 1.0f;

    // Ruído mais forte no ataque
    float attackNoise = ((float)random(0, 20000) / 10000.0f) - 1.0f;

    // Harmônicos metálicos
    float metal =
      sinf(6.28318530f * 3200.0f * t)  * 0.18f +
      sinf(6.28318530f * 5100.0f * t)  * 0.15f +
      sinf(6.28318530f * 7400.0f * t)  * 0.11f +
      sinf(6.28318530f * 9600.0f * t)  * 0.07f +
      sinf(6.28318530f * 12200.0f * t) * 0.04f;

    // Pequena variação na cauda, para parecer mais vivo
    float shimmer =
      1.0f +
      sinf(6.28318530f * 7.0f * t) * 0.08f +
      sinf(6.28318530f * 11.0f * t) * 0.05f;

    float body = (noise * 0.82f + metal) * tailEnv * shimmer;
    float attack = attackNoise * attackEnv * 1.10f;

    float sample = body + attack;

    sample *= 20000.0f;
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
  float phase = 0.0f;
  float phaseSub = 0.0f;

  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;

    // Pitch descendo: começa mais agudo e cai para o grave
    float freq = 48.0f + 87.0f * expf(-t * 32.0f);

    // Envelope mais seco
    float bodyEnv = expf(-t * 22.0f);

    // Acumula fase corretamente para frequência variável
    phase += 6.28318530f * freq / sampleRate;
    phaseSub += 6.28318530f * 48.0f / sampleRate;

    // Corpo principal do bumbo
    float body = sinf(phase) * bodyEnv;

    // Subgrave mais curto e controlado
    float sub = sinf(phaseSub) * expf(-t * 16.0f) * 0.22f;

    // Click inicial mais presente para calibração
    float click = 0.0f;
    if (t < 0.004f) {
      float n = ((float)random(0, 20000) / 10000.0f) - 1.0f;
      click = n * (1.0f - (t / 0.004f)) * 0.60f;
    }

    float sample = (body * 0.90f) + sub + click;

    sample *= 26000.0f;
    sample = constrain(sample, -30000.0f, 30000.0f);

    buffer[i] = (int16_t)sample;
  }
}

}
}