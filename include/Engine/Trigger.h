#pragma once

#include <Arduino.h>

namespace DrumOS {
namespace Trigger {

struct PadStats {
  uint32_t hits;
  uint32_t crosstalkBlocked;
  uint32_t retriggerBlocked;
  uint32_t noiseRejected;
  uint32_t peakSum;
  uint32_t velocitySum;
  uint16_t minPeak;
  uint16_t maxPeak;
};

void begin();
void process();

void setScopePad(int pad);
int getScopePad();

void triggerPad(int pad, int peak);

void resetCrosstalkMatrix();
void setCrosstalkLevel(int sourcePad, int targetPad, int level);
int getCrosstalkLevel(int sourcePad, int targetPad);
void printCrosstalkMatrix();

const PadStats& getStats(int pad);
void resetStats();
void resetStats(int pad);
void printStats();
void printStats(int pad);

}
}
