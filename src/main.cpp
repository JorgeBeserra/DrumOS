#include <Arduino.h>
#include "Core/DrumOS.h"

void setup() {
  Serial.begin(115200);
  DrumOS::begin();
}

void loop() {
  DrumOS::loop();
}