#include <Arduino.h>
#include "Audio.h"
#include "Pads.h"
#include "Synth.h"
#include "Voices.h"

#define DUR_KICK    220
#define DUR_SNARE   120
#define DUR_HIHAT   60
#define DUR_CRASH   220
#define DUR_HITOM   120
#define DUR_LOWTOM  150

#define SAMPLES_KICK    (SAMPLE_RATE * DUR_KICK / 1000)
#define SAMPLES_SNARE   (SAMPLE_RATE * DUR_SNARE / 1000)
#define SAMPLES_HIHAT   (SAMPLE_RATE * DUR_HIHAT / 1000)
#define SAMPLES_CRASH   (SAMPLE_RATE * DUR_CRASH / 1000)
#define SAMPLES_HITOM   (SAMPLE_RATE * DUR_HITOM / 1000)
#define SAMPLES_LOWTOM  (SAMPLE_RATE * DUR_LOWTOM / 1000)

namespace DrumOS {
namespace Voices {

static int16_t bufKick[SAMPLES_KICK];
static int16_t bufSnare[SAMPLES_SNARE];
static int16_t bufHiHat[SAMPLES_HIHAT];
static int16_t bufCrash[SAMPLES_CRASH];
static int16_t bufHiTom[SAMPLES_HITOM];
static int16_t bufLowTom[SAMPLES_LOWTOM];

static Instrument instruments[DrumOS::Pads::PAD_COUNT] = {
  {bufKick,   SAMPLES_KICK},
  {bufSnare,  SAMPLES_SNARE},
  {bufHiHat,  SAMPLES_HIHAT},
  {bufCrash,  SAMPLES_CRASH},
  {bufHiTom,  SAMPLES_HITOM},
  {bufLowTom, SAMPLES_LOWTOM}
};

void begin() {
  DrumOS::Synth::createKick(bufKick, SAMPLES_KICK, SAMPLE_RATE);
  DrumOS::Synth::createSnare(bufSnare, SAMPLES_SNARE, SAMPLE_RATE);
  DrumOS::Synth::createHiHat(bufHiHat, SAMPLES_HIHAT, SAMPLE_RATE);
  DrumOS::Synth::createCrash(bufCrash, SAMPLES_CRASH, SAMPLE_RATE);
  DrumOS::Synth::createHiTom(bufHiTom, SAMPLES_HITOM, SAMPLE_RATE);
  DrumOS::Synth::createLowTom(bufLowTom, SAMPLES_LOWTOM, SAMPLE_RATE);
}

int16_t* getBuffer(int pad) {
  if (pad < 0 || pad >= DrumOS::Pads::PAD_COUNT) return nullptr;
  return instruments[pad].buffer;
}

int getSamples(int pad) {
  if (pad < 0 || pad >= DrumOS::Pads::PAD_COUNT) return 0;
  return instruments[pad].samples;
}

}
}