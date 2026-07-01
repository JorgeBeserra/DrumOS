#pragma once

#include <Arduino.h>

namespace DrumOS {
namespace Trigger {

void begin();
void process();

void setScopePad(int pad);
int getScopePad();

void triggerPad(int pad, int peak);

}
}