#include <Arduino.h>

#include "Core/Version.h"
#include "Core/Preferences.h"
#include "Engine/Audio.h"
#include "Modules/Click.h"
#include "Engine/Pads.h"
#include "Engine/Trigger.h"
#include "Engine/Voices.h"
#include "UI/Shell.h"

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

  Serial.println("Inicializando ConfigStore...");
  ConfigStore::begin();
  ConfigStore::loadPads();

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
