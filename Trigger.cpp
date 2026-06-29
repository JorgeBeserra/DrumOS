#include <Arduino.h>
#include <driver/adc.h>

#include "Trigger.h"
#include "Pads.h"
#include "Velocity.h"
#include "Audio.h"
#include "Voices.h"

namespace DrumOS {
namespace Trigger {

static int scopePad = -1;
static int lastScopeValue = -1;
static bool firstScopePrint = true;

void begin() {
  scopePad = -1;
}

void triggerPad(int pad, int peak) {
  if (pad < 0 || pad >= DrumOS::Pads::PAD_COUNT) return;

  auto& p = DrumOS::Pads::pads[pad];

  int velocity = DrumOS::Velocity::apply(
    peak,
    p.threshold,
    p.curve
  );

  int volume = (velocity * p.volume) / 127;

  DrumOS::Audio::playBuffer(
    DrumOS::Voices::getBuffer(pad),
    DrumOS::Voices::getSamples(pad),
    volume
  );

  Serial.print(p.name);
  Serial.print(" peak=");
  Serial.print(peak);
  Serial.print(" velocity=");
  Serial.println(velocity);
}

void process() {
  unsigned long now = millis();

  for (int i = 0; i < DrumOS::Pads::PAD_COUNT; i++) {
    auto& p = DrumOS::Pads::pads[i];

    int value = adc1_get_raw(p.adc);

    switch (p.state) {
      case DrumOS::Pads::IDLE:
        if (value > p.threshold) {
          p.peak = value;
          p.timer = now;
          p.state = DrumOS::Pads::WAIT_PEAK;
        }
        break;

      case DrumOS::Pads::WAIT_PEAK:
        if (value > p.peak) {
          p.peak = value;
        }

        if (now - p.timer >= (unsigned long)p.scanMs) {
          triggerPad(i, p.peak);

          p.timer = now;
          p.state = DrumOS::Pads::MASK_TIME;
        }
        break;

      case DrumOS::Pads::MASK_TIME:
        if (now - p.timer >= (unsigned long)p.debounceMs) {
          p.peak = 0;
          p.state = DrumOS::Pads::IDLE;
        }
        break;
    }
  }

  static unsigned long lastScope = 0;
  static int lastScopeValue = -1;
  static bool firstScopePrint = true;

  const int SCOPE_ZERO_VALUE = 20;
  const int SCOPE_DELTA = 10;

  if (scopePad >= 0 && now - lastScope > 20) {
    lastScope = now;

    int value = adc1_get_raw(DrumOS::Pads::pads[scopePad].adc);

    if (value <= SCOPE_ZERO_VALUE) {
      value = 0;
    }

    bool shouldPrint = false;

    if (firstScopePrint) {
      shouldPrint = true;
      firstScopePrint = false;
    } else if (lastScopeValue == 0 && value > 0) {
      shouldPrint = true;
    } else if (lastScopeValue > 0 && value == 0) {
      shouldPrint = true;
    } else if (value > 0 && abs(value - lastScopeValue) >= SCOPE_DELTA) {
      shouldPrint = true;
    }

    if (shouldPrint) {
      Serial.print(DrumOS::Pads::pads[scopePad].name);
      Serial.print(": ");
      Serial.println(value);

      lastScopeValue = value;
    }
  }
}

void setScopePad(int pad) {
  if (pad >= 0 && pad < DrumOS::Pads::PAD_COUNT) {
    scopePad = pad;
  } else {
    scopePad = -1;
  }

  lastScopeValue = -1;
  firstScopePrint = true;
}

int getScopePad() {
  return scopePad;
}

}
}