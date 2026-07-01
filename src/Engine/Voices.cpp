#include <Arduino.h>
#include "Engine/Audio.h"
#include "Engine/Pads.h"
#include "Engine/Voices.h"

// Samples TR-909
#include "samples/_909_bassdrum_8bb2c6e8_9a6d_4d32_b7ec_1263594ef367.h"
#include "samples/_909_snare_0017fa18_a5eb_4d9d_b6f2_e2ddd30a3010.h"
#include "samples/_909_closed_hat_e0ac4b39_23fb_4a56_841d_c9e0ff440cab.h"
#include "samples/_909_open_hat_51c5eea4_391c_4743_896a_859692ec1105.h"
#include "samples/_909_crash_42a56ff6_89b6_4f2e_8a66_5a41d316f4cb.h"
#include "samples/_909_high_tom_ad503883_8a72_46ab_a05b_a84149953e17.h"
#include "samples/_909_low_tom_28d14cb9_1dc6_4193_9dd7_4e881f25f520.h"

namespace DrumOS {
namespace Voices {

static Instrument instruments[DrumOS::Pads::PAD_COUNT] = {
  // KICK
  {
    _909_bassdrum_8bb2c6e8_9a6d_4d32_b7ec_1263594ef367,
    _909_bassdrum_8bb2c6e8_9a6d_4d32_b7ec_1263594ef367_len
  },

  // SNARE
  {
    _909_snare_0017fa18_a5eb_4d9d_b6f2_e2ddd30a3010,
    _909_snare_0017fa18_a5eb_4d9d_b6f2_e2ddd30a3010_len
  },

  // HI-HAT
  {
    _909_closed_hat_e0ac4b39_23fb_4a56_841d_c9e0ff440cab,
    _909_closed_hat_e0ac4b39_23fb_4a56_841d_c9e0ff440cab_len
  },

  // CRASH
  {
    _909_crash_42a56ff6_89b6_4f2e_8a66_5a41d316f4cb,
    _909_crash_42a56ff6_89b6_4f2e_8a66_5a41d316f4cb_len
  },

  // HI TOM
  {
    _909_high_tom_ad503883_8a72_46ab_a05b_a84149953e17,
    _909_high_tom_ad503883_8a72_46ab_a05b_a84149953e17_len
  },

  // LOW TOM
  {
    _909_low_tom_28d14cb9_1dc6_4193_9dd7_4e881f25f520,
    _909_low_tom_28d14cb9_1dc6_4193_9dd7_4e881f25f520_len
  }
};

void begin() {
  Serial.println("Samples TR-909 carregados da Flash/PROGMEM");
}

const int16_t* getBuffer(int pad) {
  if (pad < 0 || pad >= DrumOS::Pads::PAD_COUNT) return nullptr;
  return instruments[pad].buffer;
}

int getSamples(int pad) {
  if (pad < 0 || pad >= DrumOS::Pads::PAD_COUNT) return 0;
  return instruments[pad].samples;
}

}
}