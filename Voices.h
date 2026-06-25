#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "Pads.h"

namespace DrumOS {
namespace Voices {

struct Instrument {
  int16_t* buffer;
  int samples;
};

void begin();

int16_t* getBuffer(int pad);
int getSamples(int pad);

}
}