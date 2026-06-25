#include <Arduino.h>

#include "Shell.h"
#include "Audio.h"
#include "Click.h"
#include "Pads.h"
#include "Trigger.h"

namespace DrumOS {
namespace Shell {

void begin() {
  Serial.println("Shell pronto. Digite: help");
}

static void printHelp() {
  Serial.println("Comandos:");
  Serial.println("1..6              = testar sons");
  Serial.println("vol 80            = volume master 0-100");
  Serial.println("kick 120          = volume do bumbo 0-127");
  Serial.println("snare 100         = volume da caixa 0-127");
  Serial.println("thr kick 1200     = threshold do pad");
  Serial.println("scope kick        = monitorar um pad");
  Serial.println("scope off         = desligar scope");
  Serial.println("click on/off      = ligar/desligar click");
  Serial.println("bpm 120           = definir BPM");
  Serial.println("status            = listar configuracoes");
}

static void handleCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();

  if (cmd.length() == 0) return;

  if (cmd == "help") {
    printHelp();
    return;
  }

  if (cmd == "status") {
    Serial.print("Master Volume: ");
    Serial.println(DrumOS::Audio::getMasterVolume());

    Serial.print("Click: ");
    Serial.println(DrumOS::Click::isEnabled() ? "ON" : "OFF");

    Serial.print("BPM: ");
    Serial.println(DrumOS::Click::getBpm());

    for (int i = 0; i < DrumOS::Pads::PAD_COUNT; i++) {
      auto& p = DrumOS::Pads::pads[i];

      Serial.print(p.name);
      Serial.print(" volume=");
      Serial.print(p.volume);
      Serial.print(" threshold=");
      Serial.println(p.threshold);
    }

    return;
  }

  if (cmd == "click on") {
    DrumOS::Click::setEnabled(true);
    Serial.println("Click ligado");
    return;
  }

  if (cmd == "click off") {
    DrumOS::Click::setEnabled(false);
    Serial.println("Click desligado");
    return;
  }

  if (cmd.startsWith("bpm ")) {
    int value = cmd.substring(4).toInt();
    DrumOS::Click::setBpm(value);

    Serial.print("BPM = ");
    Serial.println(DrumOS::Click::getBpm());
    return;
  }

  if (cmd.startsWith("scope ")) {
    String padText = cmd.substring(6);
    padText.trim();

    if (padText == "off") {
      DrumOS::Trigger::setScopePad(-1);
      Serial.println("Scope desligado");
      return;
    }

    int pad = DrumOS::Pads::findByName(padText);

    if (pad < 0) {
      Serial.println("Pad invalido");
      return;
    }

    DrumOS::Trigger::setScopePad(pad);

    Serial.print("Scope ligado em: ");
    Serial.println(DrumOS::Pads::pads[pad].name);
    return;
  }

  int space1 = cmd.indexOf(' ');
  String a = space1 >= 0 ? cmd.substring(0, space1) : cmd;
  String rest = space1 >= 0 ? cmd.substring(space1 + 1) : "";

  if (a == "vol") {
    int value = rest.toInt();
    DrumOS::Audio::setMasterVolume(value);

    Serial.print("Volume master = ");
    Serial.println(DrumOS::Audio::getMasterVolume());
    return;
  }

  if (a == "thr") {
    int space2 = rest.indexOf(' ');

    if (space2 < 0) {
      Serial.println("Uso: thr kick 1200");
      return;
    }

    String padText = rest.substring(0, space2);
    int value = rest.substring(space2 + 1).toInt();

    int pad = DrumOS::Pads::findByName(padText);

    if (pad < 0) {
      Serial.println("Pad invalido");
      return;
    }

    DrumOS::Pads::pads[pad].threshold = constrain(value, 0, 4095);

    Serial.print("Threshold ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" = ");
    Serial.println(DrumOS::Pads::pads[pad].threshold);
    return;
  }

  int pad = DrumOS::Pads::findByName(a);

  if (pad >= 0) {
    int value = rest.toInt();
    DrumOS::Pads::pads[pad].volume = constrain(value, 0, 127);

    Serial.print("Volume ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" = ");
    Serial.println(DrumOS::Pads::pads[pad].volume);
    return;
  }

  Serial.println("Comando desconhecido. Digite: help");
}

void process() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();

  if (cmd == "1") DrumOS::Trigger::triggerPad(DrumOS::Pads::KICK, 4095);
  else if (cmd == "2") DrumOS::Trigger::triggerPad(DrumOS::Pads::SNARE, 4095);
  else if (cmd == "3") DrumOS::Trigger::triggerPad(DrumOS::Pads::HIHAT, 4095);
  else if (cmd == "4") DrumOS::Trigger::triggerPad(DrumOS::Pads::CRASH, 4095);
  else if (cmd == "5") DrumOS::Trigger::triggerPad(DrumOS::Pads::HITOM, 4095);
  else if (cmd == "6") DrumOS::Trigger::triggerPad(DrumOS::Pads::LOWTOM, 4095);
  else handleCommand(cmd);
}

}
}