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

static char line[96];
static uint8_t pos = 0;

void begin() {
  Serial.println("Shell pronto. Digite: help");
  Serial.print("> ");
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

static bool parsePadValue(const String& rest, String& padText, int& value, int& pad) {
  int space = rest.indexOf(' ');
  if (space < 0) return false;
  padText = rest.substring(0, space);
  value = rest.substring(space + 1).toInt();
  pad = DrumOS::Pads::findByName(padText);
  return pad >= 0;
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
  Serial.println("margin kick 80      = margem minima acima do threshold");
  Serial.println("curve kick soft     = curva: soft, linear, hard");
  Serial.println("xtalk show/reset    = matriz de crosstalk");
  Serial.println("xtalk kick snare 15 = definir crosstalk 0-100");
  Serial.println("stats               = estatisticas de todos os pads");
  Serial.println("stats kick          = estatisticas de um pad");
  Serial.println("stats reset         = zerar estatisticas");
  Serial.println("scope kick/off      = monitorar um pad");
  Serial.println("scope graph kick    = osciloscopio serial rapido");
  Serial.println("click on/off        = ligar/desligar click");
  Serial.println("bpm 120             = definir BPM");
  Serial.println("status              = listar configuracoes");
}

static void printStatus() {
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
    Serial.print(" margin=");
    Serial.print(p.gateMargin);
    Serial.print(" curve=");
    Serial.println(curveToText(p.curve));
  }
}

static void handlePadNumberCommand(const String& command, const String& rest) {
  String padText;
  int value;
  int pad;

  if (!parsePadValue(rest, padText, value, pad)) {
    Serial.println("Uso invalido ou pad invalido");
    return;
  }

  auto& p = DrumOS::Pads::pads[pad];

  if (command == "thr") {
    p.threshold = constrain(value, 0, 4095);
    if (p.peakMax <= p.threshold) p.peakMax = constrain(p.threshold + 1, 1, 4095);
    Serial.print("Threshold ");
  } else if (command == "max") {
    p.peakMax = constrain(value, p.threshold + 1, 4095);
    Serial.print("PeakMax ");
  } else if (command == "deb") {
    p.debounceMs = constrain(value, 10, 500);
    Serial.print("Debounce ");
  } else if (command == "scan") {
    p.scanMs = constrain(value, 1, 30);
    Serial.print("Scan ");
  } else if (command == "lock") {
    p.retriggerLockMs = constrain(value, 0, 200);
    Serial.print("RetriggerLock ");
  } else if (command == "margin") {
    p.gateMargin = constrain(value, 0, 500);
    Serial.print("Margin ");
  } else {
    return;
  }

  DrumOS::Pads::resetRuntimeState(pad);
  Serial.print(p.name);
  Serial.print(" = ");
  Serial.println(value);
}

static void handleXtalkCommand(String rest) {
  rest.trim();
  if (rest == "show") { DrumOS::Trigger::printCrosstalkMatrix(); return; }
  if (rest == "reset") { DrumOS::Trigger::resetCrosstalkMatrix(); Serial.println("Matriz de crosstalk restaurada"); return; }
  int s1 = rest.indexOf(' ');
  if (s1 < 0) { Serial.println("Uso: xtalk kick snare 15"); return; }
  int s2 = rest.indexOf(' ', s1 + 1);
  if (s2 < 0) { Serial.println("Uso: xtalk kick snare 15"); return; }
  String sourceText = rest.substring(0, s1);
  String targetText = rest.substring(s1 + 1, s2);
  int level = rest.substring(s2 + 1).toInt();
  int source = DrumOS::Pads::findByName(sourceText);
  int target = DrumOS::Pads::findByName(targetText);
  if (source < 0 || target < 0) { Serial.println("Pad invalido"); return; }
  if (source == target) { Serial.println("Origem e destino nao podem ser iguais"); return; }
  DrumOS::Trigger::setCrosstalkLevel(source, target, level);
  Serial.print("XTalk ");
  Serial.print(DrumOS::Pads::pads[source].name);
  Serial.print(" -> ");
  Serial.print(DrumOS::Pads::pads[target].name);
  Serial.print(" = ");
  Serial.println(constrain(level, 0, 100));
}

static void handleStatsCommand(String rest) {
  rest.trim();
  if (rest.length() == 0) { DrumOS::Trigger::printStats(); return; }
  if (rest == "reset") { DrumOS::Trigger::resetStats(); Serial.println("Estatisticas zeradas"); return; }
  int pad = DrumOS::Pads::findByName(rest);
  if (pad < 0) { Serial.println("Uso: stats, stats kick ou stats reset"); return; }
  DrumOS::Trigger::printStats(pad);
}

static bool handleScopeCommand(String rest) {
  rest.trim();
  if (rest.startsWith("graph ")) {
    String padText = rest.substring(6);
    padText.trim();
    int pad = DrumOS::Pads::findByName(padText);
    if (pad < 0) { Serial.println("Pad invalido"); return true; }
    DrumOS::Trigger::printScopeGraph(pad);
    return true;
  }
  if (rest == "off") {
    DrumOS::Trigger::setScopePad(-1);
    Serial.println("Scope desligado");
    return true;
  }
  int pad = DrumOS::Pads::findByName(rest);
  if (pad < 0) { Serial.println("Pad invalido"); return true; }
  DrumOS::Trigger::setScopePad(pad);
  Serial.print("Scope ligado em: ");
  Serial.println(DrumOS::Pads::pads[pad].name);
  return true;
}

static void handleCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();
  if (cmd.length() == 0) return;
  if (cmd == "help") { printHelp(); return; }
  if (cmd == "status") { printStatus(); return; }
  if (cmd == "save") { Serial.println(DrumOS::ConfigStore::savePads() ? "Configuracoes salvas" : "Erro ao salvar configuracoes"); return; }
  if (cmd == "load") { Serial.println(DrumOS::ConfigStore::loadPads() ? "Configuracoes carregadas" : "Nenhuma configuracao salva encontrada"); return; }
  if (cmd == "factory") { Serial.println(DrumOS::ConfigStore::factoryReset() ? "Padroes restaurados" : "Erro ao restaurar padroes"); return; }
  if (cmd == "click on") { DrumOS::Click::setEnabled(true); Serial.println("Click ligado"); return; }
  if (cmd == "click off") { DrumOS::Click::setEnabled(false); Serial.println("Click desligado"); return; }
  if (cmd.startsWith("bpm ")) {
    DrumOS::Click::setBpm(cmd.substring(4).toInt());
    Serial.print("BPM = ");
    Serial.println(DrumOS::Click::getBpm());
    return;
  }
  int space = cmd.indexOf(' ');
  String command = space >= 0 ? cmd.substring(0, space) : cmd;
  String rest = space >= 0 ? cmd.substring(space + 1) : "";
  if (command == "scope") { handleScopeCommand(rest); return; }
  if (command == "vol") {
    DrumOS::Audio::setMasterVolume(rest.toInt());
    Serial.print("Volume master = ");
    Serial.println(DrumOS::Audio::getMasterVolume());
    return;
  }
  if (command == "xtalk") { handleXtalkCommand(rest); return; }
  if (command == "stats") { handleStatsCommand(rest); return; }
  if (command == "thr" || command == "max" || command == "deb" || command == "scan" || command == "lock" || command == "margin") {
    handlePadNumberCommand(command, rest);
    return;
  }
  if (command == "curve") {
    int space2 = rest.indexOf(' ');
    if (space2 < 0) { Serial.println("Uso: curve kick soft"); return; }
    String padText = rest.substring(0, space2);
    String curveText = rest.substring(space2 + 1);
    curveText.trim();
    int pad = DrumOS::Pads::findByName(padText);
    if (pad < 0) { Serial.println("Pad invalido"); return; }
    DrumOS::Velocity::Curve curve;
    if (!parseCurve(curveText, curve)) { Serial.println("Curva invalida"); return; }
    DrumOS::Pads::pads[pad].curve = curve;
    DrumOS::Pads::resetRuntimeState(pad);
    Serial.print("Curve ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" = ");
    Serial.println(curveToText(curve));
    return;
  }
  int pad = DrumOS::Pads::findByName(command);
  if (pad >= 0) {
    DrumOS::Pads::pads[pad].volume = constrain(rest.toInt(), 0, 127);
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
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (pos == 0) continue;
      Serial.println();
      line[pos] = 0;
      String cmd = String(line);
      pos = 0;
      line[0] = 0;
      executeLine(cmd);
      Serial.print("> ");
      continue;
    }
    if (c == 8 || c == 127) {
      if (pos > 0) { pos--; line[pos] = 0; }
      continue;
    }
    if (c < 32 || c > 126) continue;
    if (pos < sizeof(line) - 1) {
      line[pos++] = c;
      line[pos] = 0;
      Serial.write(c);
    }
  }
}

}
}
