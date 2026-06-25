#include <Arduino.h>

#include "Version.h"
#include "Audio.h"
#include "Click.h"
#include "Pads.h"
#include "Trigger.h"
#include "Voices.h"
#include "Shell.h"

namespace DrumOS {

void begin() {
  Serial.println();
  Serial.println("================================");
  Serial.println("          DrumOS");
  Serial.println("================================");
  Serial.print("Versao : ");
  Serial.println(DRUMOS_VERSION);
  Serial.print("Build  : ");
  Serial.println(DRUMOS_BUILD);
  Serial.println();

  Serial.println("Inicializando Audio...");
  Audio::begin();

  Serial.println("Gerando Voices...");
  Voices::begin();

  Serial.println("Inicializando Pads...");
  Pads::begin();

  Serial.println("Inicializando Trigger...");
  Trigger::begin();

  Serial.println("Inicializando Click...");
  Click::begin(SAMPLE_RATE);

  Serial.println("Inicializando Shell...");
  Shell::begin();

  Serial.println("Sistema pronto!");
}

void loop() {
  Shell::process();
  Trigger::process();
  Click::process();
  Audio::process();
}

}