#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace DrumOS {
namespace Drivers {
namespace PCM5102 {

bool begin();
bool isReady();

void writeStereoBlock(const int16_t* data, size_t samplesStereo);
void clear();

}
}
}