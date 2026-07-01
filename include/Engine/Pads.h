#pragma once

#include <Arduino.h>
#include <driver/adc.h>
#include "Engine/Velocity.h"

namespace DrumOS {
namespace Pads {

enum PadId {
  KICK,
  SNARE,
  HIHAT,
  CRASH,
  HITOM,
  LOWTOM,
  PAD_COUNT
};

enum PadState {
  IDLE,
  WAIT_PEAK,
  MASK_TIME
};

struct PadConfig {
  const char* name;
  adc1_channel_t adc;
  int threshold;
  int peakMax;
  int volume;
  int debounceMs;
  int scanMs;
  unsigned long lastTrigger;
  PadState state;
  unsigned long timer;
  int peak;
  DrumOS::Velocity::Curve curve;
};

extern PadConfig pads[PAD_COUNT];

void begin();
void resetToDefaults();
void resetRuntimeState(int pad);
int findByName(String name);

}
}
