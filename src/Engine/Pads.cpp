#include "Engine/Pads.h"

namespace DrumOS {
namespace Pads {

static const PadConfig defaultPads[PAD_COUNT] = {
  // name      adc             thr   max   vol  deb scan lock gate last state timer peak curve
  {"KICK",    ADC1_CHANNEL_4,  550,  2100, 127, 90,  10,  35,  80, 0, IDLE, 0, 0, DrumOS::Velocity::LINEAR},
  {"SNARE",   ADC1_CHANNEL_5,  500,  2400, 120, 80,  5,   30,  70, 0, IDLE, 0, 0, DrumOS::Velocity::LINEAR},
  {"HI-HAT",  ADC1_CHANNEL_6,  350,  1800, 100, 45,  3,   18,  45, 0, IDLE, 0, 0, DrumOS::Velocity::SOFT},
  {"CRASH",   ADC1_CHANNEL_7,  350,  2600, 115, 140, 8,   55,  60, 0, IDLE, 0, 0, DrumOS::Velocity::SOFT},
  {"HI-TOM",  ADC1_CHANNEL_0,  500,  2400, 110, 80,  5,   30,  70, 0, IDLE, 0, 0, DrumOS::Velocity::LINEAR},
  {"LOW-TOM", ADC1_CHANNEL_3,  500,  2400, 110, 80,  5,   35,  70, 0, IDLE, 0, 0, DrumOS::Velocity::LINEAR}
};

PadConfig pads[PAD_COUNT];

void resetRuntimeState(int pad) {
  if (pad < 0 || pad >= PAD_COUNT) return;

  pads[pad].state = IDLE;
  pads[pad].timer = 0;
  pads[pad].peak = 0;
  pads[pad].lastTrigger = 0;
}

void resetToDefaults() {
  for (int i = 0; i < PAD_COUNT; i++) {
    pads[i] = defaultPads[i];
    resetRuntimeState(i);
  }
}

void begin() {
  adc1_config_width(ADC_WIDTH_BIT_12);
  resetToDefaults();

  for (int i = 0; i < PAD_COUNT; i++) {
    adc1_config_channel_atten(pads[i].adc, ADC_ATTEN_DB_11);
    resetRuntimeState(i);
  }
}

int findByName(String name) {
  name.toLowerCase();

  if (name == "kick" || name == "bumbo") return KICK;
  if (name == "snare" || name == "caixa") return SNARE;
  if (name == "hihat" || name == "hi-hat") return HIHAT;
  if (name == "crash") return CRASH;
  if (name == "hitom" || name == "tom1") return HITOM;
  if (name == "lowtom" || name == "tom2") return LOWTOM;

  return -1;
}

}
}
