#include "Engine/Pads.h"

namespace DrumOS {
namespace Pads {

PadConfig pads[PAD_COUNT] = {
  // name      adc             thr   max   vol  deb scan last state timer peak curve
  {"KICK",    ADC1_CHANNEL_4,  550,  2100, 127, 90,  10, 0, IDLE, 0, 0, DrumOS::Velocity::LINEAR},
  {"SNARE",   ADC1_CHANNEL_5,  500,  2400, 120, 80,  5,  0, IDLE, 0, 0, DrumOS::Velocity::LINEAR},
  {"HI-HAT",  ADC1_CHANNEL_6,  350,  1800, 100, 45,  3,  0, IDLE, 0, 0, DrumOS::Velocity::SOFT},
  {"CRASH",   ADC1_CHANNEL_7,  350,  2600, 115, 140, 8,  0, IDLE, 0, 0, DrumOS::Velocity::SOFT},
  {"HI-TOM",  ADC1_CHANNEL_0,  500,  2400, 110, 80,  5,  0, IDLE, 0, 0, DrumOS::Velocity::LINEAR},
  {"LOW-TOM", ADC1_CHANNEL_3,  500,  2400, 110, 80,  5,  0, IDLE, 0, 0, DrumOS::Velocity::LINEAR}
};

void begin() {
  adc1_config_width(ADC_WIDTH_BIT_12);

  for (int i = 0; i < PAD_COUNT; i++) {
    adc1_config_channel_atten(pads[i].adc, ADC_ATTEN_DB_11);
    pads[i].state = IDLE;
    pads[i].timer = 0;
    pads[i].peak = 0;
    pads[i].lastTrigger = 0;
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
