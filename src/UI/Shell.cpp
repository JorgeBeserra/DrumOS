#include <Arduino.h>

#include "UI/Shell.h"
#include "Core/Preferences.h"
#include "Engine/Audio.h"
#include "Modules/Click.h"
#include "Engine/Pads.h"
#include "Engine/Trigger.h"
#include "Engine/Velocity.h"

namespace DrumOS {
namespace Shell {

void begin() {
  Serial.println("Shell pronto. Digite: help");
  Serial.print("> ");
}

static void printHelp() {
  Serial.println("Comandos:");
  Serial.println("1..6                = testar sons");
  Serial.println("vol 80              = volume master 0-100");
  Serial.println("kick 120            = volume do bumbo 0-127");
  Serial.println("snare 100           = volume da caixa 0-127");
  Serial.println("thr kick 550        = threshold do pad");
  Serial.println("max kick 2100       = peak maximo para velocity 127");
  Serial.println("deb kick 90         = debounce/mask do pad em ms");
  Serial.println("scan kick 5         = janela de captura do peak em ms");
  Serial.println("lock kick 35        = retrigger lock do pad em ms");
  Serial.println("curve kick soft     = curva: soft, linear, hard");
  Serial.println("save                = salvar configuracoes dos pads");
  Serial.println("load                = carregar configuracoes salvas");
  Serial.println("factory             = restaurar padroes de fabrica");
  Serial.println("scope kick          = monitorar um pad");
  Serial.println("scope off           = desligar scope");
  Serial.println("click on/off        = ligar/desligar click");
  Serial.println("bpm 120             = definir BPM");
  Serial.println("status              = listar configuracoes");
}

static const char* curveToText(DrumOS::Velocity::Curve curve) {
  switch (curve) {
    case DrumOS::Velocity::SOFT: return "SOFT";
    case DrumOS::Velocity::HARD: return "HARD";
    case DrumOS::Velocity::LINEAR:
    default: return "LINEAR";
  }
}

static bool parseCurve(const String& text, DrumOS::Velocity::Curve& curve) {
  if (text == "soft") { curve = DrumOS::Velocity::SOFT; return true; }
  if (text == "linear") { curve = DrumOS::Velocity::LINEAR; return true; }
  if (text == "hard") { curve = DrumOS::Velocity::HARD; return true; }
  return false;
}

static bool parsePadValue(const String& rest, String& padText, int& value) {
  int space2 = rest.indexOf(' ');
  if (space2 < 0) return false;
  padText = rest.substring(0, space2);
  value = rest.substring(space2 + 1).toInt();
  return true;
}

static void handleCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();

  if (cmd.length() == 0) return;

  if (cmd == "help") { printHelp(); return; }

  if (cmd == "save") {
    Serial.println(DrumOS::ConfigStore::savePads() ? "Configuracoes salvas" : "Erro ao salvar configuracoes");
    return;
  }

  if (cmd == "load") {
    Serial.println(DrumOS::ConfigStore::loadPads() ? "Configuracoes carregadas" : "Nenhuma configuracao salva encontrada");
    return;
  }

  if (cmd == "factory") {
    Serial.println(DrumOS::ConfigStore::factoryReset() ? "Padroes de fabrica restaurados e salvos" : "Erro ao restaurar padroes");
    return;
  }

  if (cmd == "status") {
    Serial.print("Master Volume: ");
    Serial.println(DrumOS::Audio::getMasterVolume());
    Serial.print("Click: ");
    Serial.println(DrumOS::Click::isEnabled() ? "ON" : "OFF");
    Serial.print("BPM: ");
    Serial.println(DrumOS::Click::getBpm());
    Serial.print("ConfigStore: ");
    Serial.println(DrumOS::ConfigStore::isReady() ? "READY" : "OFF");

    for (int i = 0; i < DrumOS::Pads::PAD_COUNT; i++) {
      auto& p = DrumOS::Pads::pads[i];
      Serial.print(p.name);
      Serial.print(" volume=");
      Serial.print(p.volume);
      Serial.print(" threshold=");
      Serial.print(p.threshold);
      Serial.print(" peakMax=");
      Serial.print(p.peakMax);
      Serial.print(" debounce=");
      Serial.print(p.debounceMs);
      Serial.print(" scan=");
      Serial.print(p.scanMs);
      Serial.print(" lock=");
      Serial.print(p.retriggerLockMs);
      Serial.print(" curve=");
      Serial.println(curveToText(p.curve));
    }
    return;
  }

  if (cmd == "click on") { DrumOS::Click::setEnabled(true); Serial.println("Click ligado"); return; }
  if (cmd == "click off") { DrumOS::Click::setEnabled(false); Serial.println("Click desligado"); return; }

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
    if (pad < 0) { Serial.println("Pad invalido"); return; }

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
    String padText;
    int value;
    if (!parsePadValue(rest, padText, value)) { Serial.println("Uso: thr kick 550"); return; }
    int pad = DrumOS::Pads::findByName(padText);
    if (pad < 0) { Serial.println("Pad invalido"); return; }
    DrumOS::Pads::pads[pad].threshold = constrain(value, 0, 4095);
    if (DrumOS::Pads::pads[pad].peakMax <= DrumOS::Pads::pads[pad].threshold) {
      DrumOS::Pads::pads[pad].peakMax = constrain(DrumOS::Pads::pads[pad].threshold + 1, 1, 4095);
    }
    DrumOS::Pads::resetRuntimeState(pad);
    Serial.print("Threshold ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" = ");
    Serial.println(DrumOS::Pads::pads[pad].threshold);
    return;
  }

  if (a == "max") {
    String padText;
    int value;
    if (!parsePadValue(rest, padText, value)) { Serial.println("Uso: max kick 2100"); return; }
    int pad = DrumOS::Pads::findByName(padText);
    if (pad < 0) { Serial.println("Pad invalido"); return; }
    int minValue = DrumOS::Pads::pads[pad].threshold + 1;
    DrumOS::Pads::pads[pad].peakMax = constrain(value, minValue, 4095);
    DrumOS::Pads::resetRuntimeState(pad);
    Serial.print("PeakMax ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" = ");
    Serial.println(DrumOS::Pads::pads[pad].peakMax);
    return;
  }

  if (a == "deb") {
    String padText;
    int value;
    if (!parsePadValue(rest, padText, value)) { Serial.println("Uso: deb kick 90"); return; }
    int pad = DrumOS::Pads::findByName(padText);
    if (pad < 0) { Serial.println("Pad invalido"); return; }
    DrumOS::Pads::pads[pad].debounceMs = constrain(value, 10, 500);
    DrumOS::Pads::resetRuntimeState(pad);
    Serial.print("Debounce ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" = ");
    Serial.print(DrumOS::Pads::pads[pad].debounceMs);
    Serial.println(" ms");
    return;
  }

  if (a == "scan") {
    String padText;
    int value;
    if (!parsePadValue(rest, padText, value)) { Serial.println("Uso: scan kick 5"); return; }
    int pad = DrumOS::Pads::findByName(padText);
    if (pad < 0) { Serial.println("Pad invalido"); return; }
    DrumOS::Pads::pads[pad].scanMs = constrain(value, 1, 30);
    DrumOS::Pads::resetRuntimeState(pad);
    Serial.print("Scan ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" = ");
    Serial.print(DrumOS::Pads::pads[pad].scanMs);
    Serial.println(" ms");
    return;
  }

  if (a == "lock") {
    String padText;
    int value;
    if (!parsePadValue(rest, padText, value)) { Serial.println("Uso: lock kick 35"); return; }
    int pad = DrumOS::Pads::findByName(padText);
    if (pad < 0) { Serial.println("Pad invalido"); return; }
    DrumOS::Pads::pads[pad].retriggerLockMs = constrain(value, 0, 200);
    DrumOS::Pads::resetRuntimeState(pad);
    Serial.print("Retrigger lock ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" = ");
    Serial.print(DrumOS::Pads::pads[pad].retriggerLockMs);
    Serial.println(" ms");
    return;
  }

  if (a == "curve") {
    int space2 = rest.indexOf(' ');
    if (space2 < 0) { Serial.println("Uso: curve kick soft"); Serial.println("Curvas: soft, linear, hard"); return; }
    String padText = rest.substring(0, space2);
    String curveText = rest.substring(space2 + 1);
    curveText.trim();
    int pad = DrumOS::Pads::findByName(padText);
    if (pad < 0) { Serial.println("Pad invalido"); return; }
    DrumOS::Velocity::Curve curve;
    if (!parseCurve(curveText, curve)) { Serial.println("Curva invalida. Use: soft, linear ou hard"); return; }
    DrumOS::Pads::pads[pad].curve = curve;
    DrumOS::Pads::resetRuntimeState(pad);
    Serial.print("Curve ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" = ");
    Serial.println(curveToText(DrumOS::Pads::pads[pad].curve));
    return;
  }

  int pad = DrumOS::Pads::findByName(a);
  if (pad >= 0) {
    int value = rest.toInt();
    DrumOS::Pads::pads[pad].volume = constrain(value, 0, 127);
    DrumOS::Pads::resetRuntimeState(pad);
    Serial.print("Volume ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" = ");
    Serial.println(DrumOS::Pads::pads[pad].volume);
    return;
  }

  Serial.println("Comando desconhecido. Digite: help");
}

static void executeLine(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "1") DrumOS::Trigger::triggerPad(DrumOS::Pads::KICK, 4095);
  else if (cmd == "2") DrumOS::Trigger::triggerPad(DrumOS::Pads::SNARE, 4095);
  else if (cmd == "3") DrumOS::Trigger::triggerPad(DrumOS::Pads::HIHAT, 4095);
  else if (cmd == "4") DrumOS::Trigger::triggerPad(DrumOS::Pads::CRASH, 4095);
  else if (cmd == "5") DrumOS::Trigger::triggerPad(DrumOS::Pads::HITOM, 4095);
  else if (cmd == "6") DrumOS::Trigger::triggerPad(DrumOS::Pads::LOWTOM, 4095);
  else handleCommand(cmd);
}

void process() {
  static char line[96];
  static uint8_t pos = 0;

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (pos == 0) continue;
      Serial.println();
      line[pos] = '\0';
      String cmd = String(line);
      pos = 0;
      line[0] = '\0';
      executeLine(cmd);
      Serial.print("> ");
      continue;
    }

    if (c == 8 || c == 127) {
      if (pos > 0) {
        pos--;
        line[pos] = '\0';
        Serial.write(8);
        Serial.write(' ');
        Serial.write(8);
      }
      continue;
    }

    if (c < 32 || c > 126) continue;

    if (pos < sizeof(line) - 1) {
      line[pos++] = c;
      line[pos] = '\0';
      Serial.write(c);
    }
  }
}

}
}
