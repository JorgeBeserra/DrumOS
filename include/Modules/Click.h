#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace DrumOS {
namespace Click {

void begin(int sampleRate);
void process();

void setEnabled(bool enabled);
bool isEnabled();

void setBpm(int bpm);
int getBpm();

}
}