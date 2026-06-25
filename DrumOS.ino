#include <Arduino.h>
#include "DrumOS.h"

void setup() {
  Serial.begin(115200);
  DrumOS::begin();
}

void loop() {
  DrumOS::loop();
}