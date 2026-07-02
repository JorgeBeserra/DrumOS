#include <Arduino.h>
#include <Preferences.h>

#include "Core/Preferences.h"
#include "Engine/Pads.h"
#include "Engine/Trigger.h"

namespace DrumOS {
namespace ConfigStore {

static ::Preferences nvs;
static bool ready = false;

static const char* NAMESPACE = "drumos";
static const uint32_t SCHEMA_VERSION = 4;

static String keyFor(const char* prefix, int pad) {
  return String(prefix) + String(pad);
}

static String xtalkKey(int source, int target) {
  return String("xt") + String(source) + String("_") + String(target);
}

bool begin() {
  if (ready) return true;

  ready = nvs.begin(NAMESPACE, false);

  if (!ready) {
    Serial.println("ConfigStore: erro ao iniciar NVS");
    return false;
  }

  Serial.println("ConfigStore: NVS pronto");
  return true;
}

bool isReady() {
  return ready;
}

bool savePad(int pad) {
  if (!ready && !begin()) return false;
  if (pad < 0 || pad >= DrumOS::Pads::PAD_COUNT) return false;

  auto& p = DrumOS::Pads::pads[pad];

  nvs.putInt(keyFor("thr", pad).c_str(), p.threshold);
  nvs.putInt(keyFor("max", pad).c_str(), p.peakMax);
  nvs.putInt(keyFor("vol", pad).c_str(), p.volume);
  nvs.putInt(keyFor("deb", pad).c_str(), p.debounceMs);
  nvs.putInt(keyFor("scan", pad).c_str(), p.scanMs);
  nvs.putInt(keyFor("lock", pad).c_str(), p.retriggerLockMs);
  nvs.putInt(keyFor("gate", pad).c_str(), p.gateMargin);
  nvs.putInt(keyFor("curve", pad).c_str(), (int)p.curve);

  return true;
}

bool savePads() {
  if (!ready && !begin()) return false;

  nvs.putUInt("schema", SCHEMA_VERSION);

  for (int i = 0; i < DrumOS::Pads::PAD_COUNT; i++) {
    savePad(i);
  }

  for (int source = 0; source < DrumOS::Pads::PAD_COUNT; source++) {
    for (int target = 0; target < DrumOS::Pads::PAD_COUNT; target++) {
      if (source == target) continue;
      nvs.putInt(xtalkKey(source, target).c_str(), DrumOS::Trigger::getCrosstalkLevel(source, target));
    }
  }

  Serial.println("ConfigStore: configuracoes salvas");
  return true;
}

bool loadPads() {
  if (!ready && !begin()) return false;

  uint32_t schema = nvs.getUInt("schema", 0);

  if (schema == 0) {
    Serial.println("ConfigStore: nenhuma configuracao salva encontrada");
    return false;
  }

  for (int i = 0; i < DrumOS::Pads::PAD_COUNT; i++) {
    auto& p = DrumOS::Pads::pads[i];

    p.threshold = constrain(nvs.getInt(keyFor("thr", i).c_str(), p.threshold), 0, 4095);
    p.peakMax = constrain(nvs.getInt(keyFor("max", i).c_str(), p.peakMax), p.threshold + 1, 4095);
    p.volume = constrain(nvs.getInt(keyFor("vol", i).c_str(), p.volume), 0, 127);
    p.debounceMs = constrain(nvs.getInt(keyFor("deb", i).c_str(), p.debounceMs), 10, 500);
    p.scanMs = constrain(nvs.getInt(keyFor("scan", i).c_str(), p.scanMs), 1, 30);
    p.retriggerLockMs = constrain(nvs.getInt(keyFor("lock", i).c_str(), p.retriggerLockMs), 0, 200);
    p.gateMargin = constrain(nvs.getInt(keyFor("gate", i).c_str(), p.gateMargin), 0, 500);

    int curve = nvs.getInt(keyFor("curve", i).c_str(), (int)p.curve);
    curve = constrain(curve, (int)DrumOS::Velocity::LINEAR, (int)DrumOS::Velocity::HARD);
    p.curve = (DrumOS::Velocity::Curve)curve;

    DrumOS::Pads::resetRuntimeState(i);
  }

  DrumOS::Trigger::resetCrosstalkMatrix();

  if (schema >= 3) {
    for (int source = 0; source < DrumOS::Pads::PAD_COUNT; source++) {
      for (int target = 0; target < DrumOS::Pads::PAD_COUNT; target++) {
        if (source == target) continue;
        int current = DrumOS::Trigger::getCrosstalkLevel(source, target);
        int level = constrain(nvs.getInt(xtalkKey(source, target).c_str(), current), 0, 100);
        DrumOS::Trigger::setCrosstalkLevel(source, target, level);
      }
    }
  }

  if (schema != SCHEMA_VERSION) {
    nvs.putUInt("schema", SCHEMA_VERSION);
    savePads();
  }

  Serial.println("ConfigStore: configuracoes carregadas");
  return true;
}

bool factoryReset() {
  if (!ready && !begin()) return false;

  nvs.clear();
  DrumOS::Pads::resetToDefaults();
  DrumOS::Trigger::resetCrosstalkMatrix();
  savePads();

  Serial.println("ConfigStore: padroes de fabrica restaurados");
  return true;
}

}
}
