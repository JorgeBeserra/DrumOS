#include "Pads.h"

namespace DrumOS {
namespace Pads {

PadConfig pads[PAD_COUNT] = {
  {"KICK",    ADC1_CHANNEL_4, 900,  120, 80,  5, 0, IDLE, 0, 0, DrumOS::Velocity::HARD},
  {"SNARE",   ADC1_CHANNEL_5, 900,  100, 80,  5, 0, IDLE, 0, 0, DrumOS::Velocity::LINEAR},
  {"HI-HAT",  ADC1_CHANNEL_6, 1200, 80,  50,  4, 0, IDLE, 0, 0, DrumOS::Velocity::SOFT},
  {"CRASH",   ADC1_CHANNEL_7, 1200, 90,  120, 6, 0, IDLE, 0, 0, DrumOS::Velocity::LINEAR},
  {"HI-TOM",  ADC1_CHANNEL_0, 900,  100, 80,  5, 0, IDLE, 0, 0, DrumOS::Velocity::LINEAR},
  {"LOW-TOM", ADC1_CHANNEL_3, 900,  100, 80,  5, 0, IDLE, 0, 0, DrumOS::Velocity::LINEAR}
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