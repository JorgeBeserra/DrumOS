#include <Arduino.h>
#include <Preferences.h>

#include "Core/Preferences.h"
#include "Engine/Pads.h"

namespace DrumOS {
namespace ConfigStore {

static ::Preferences nvs;
static bool ready = false;

static const char* NAMESPACE = "drumos";
static const uint32_t SCHEMA_VERSION = 1;

static String keyFor(const char* prefix, int pad) {
  return String(prefix) + String(pad);
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
  nvs.putInt(keyFor("curve", pad).c_str(), (int)p.curve);

  return true;
}

bool savePads() {
  if (!ready && !begin()) return false;

  nvs.putUInt("schema", SCHEMA_VERSION);

  for (int i = 0; i < DrumOS::Pads::PAD_COUNT; i++) {
    savePad(i);
  }

  Serial.println("ConfigStore: configuracoes salvas");
  return true;
}

bool loadPads() {
  if (!ready && !begin()) return false;

  uint32_t schema = nvs.getUInt("schema", 0);

  if (schema != SCHEMA_VERSION) {
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

    int curve = nvs.getInt(keyFor("curve", i).c_str(), (int)p.curve);
    curve = constrain(curve, (int)DrumOS::Velocity::LINEAR, (int)DrumOS::Velocity::HARD);
    p.curve = (DrumOS::Velocity::Curve)curve;

    DrumOS::Pads::resetRuntimeState(i);
  }

  Serial.println("ConfigStore: configuracoes carregadas");
  return true;
}

bool factoryReset() {
  if (!ready && !begin()) return false;

  nvs.clear();
  DrumOS::Pads::resetToDefaults();
  savePads();

  Serial.println("ConfigStore: padroes de fabrica restaurados");
  return true;
}

}
}
