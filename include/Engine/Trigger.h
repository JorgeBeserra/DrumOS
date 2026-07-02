#pragma once

#include <Arduino.h>

namespace DrumOS {
namespace Trigger {

void begin();
void process();

void setScopePad(int pad);
int getScopePad();

void triggerPad(int pad, int peak);

void resetCrosstalkMatrix();
void setCrosstalkLevel(int sourcePad, int targetPad, int level);
int getCrosstalkLevel(int sourcePad, int targetPad);
void printCrosstalkMatrix();

}
}
