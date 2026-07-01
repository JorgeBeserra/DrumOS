#include <Arduino.h>
#include <driver/adc.h>

#include "Engine/Trigger.h"
#include "Engine/Pads.h"
#include "Engine/Velocity.h"
#include "Engine/Audio.h"
#include "Engine/Voices.h"

namespace DrumOS {
namespace Trigger {

static int scopePad = -1;
static int lastScopeValue = -1;
static bool firstScopePrint = true;

static int lastTriggeredPad = -1;
static int lastTriggeredPeak = 0;
static unsigned long lastTriggeredAt = 0;

static const unsigned long CROSSTALK_WINDOW_MS = 25;
static const int CROSSTALK_RATIO_PERCENT = 45;

static bool isCrosstalk(int pad, int peak, unsigned long now) {
  if (lastTriggeredPad < 0) return false;
  if (pad == lastTriggeredPad) return false;
  if (now - lastTriggeredAt > CROSSTALK_WINDOW_MS) return false;
  if (lastTriggeredPeak <= 0) return false;

  int ratio = (peak * 100) / lastTriggeredPeak;

  if (ratio < CROSSTALK_RATIO_PERCENT) {
    Serial.print("CROSSTALK bloqueado: ");
    Serial.print(DrumOS::Pads::pads[pad].name);
    Serial.print(" peak=");
    Serial.print(peak);
    Serial.print(" origem=");
    Serial.print(DrumOS::Pads::pads[lastTriggeredPad].name);
    Serial.print(" origemPeak=");
    Serial.println(lastTriggeredPeak);
    return true;
  }

  return false;
}

void begin() {
  scopePad = -1;
  lastTriggeredPad = -1;
  lastTriggeredPeak = 0;
  lastTriggeredAt = 0;
}

void triggerPad(int pad, int peak) {
  if (pad < 0 || pad >= DrumOS::Pads::PAD_COUNT) return;

  auto& p = DrumOS::Pads::pads[pad];

  int velocity = DrumOS::Velocity::apply(peak, p.threshold, p.peakMax, p.curve);

  int dynamicVolume = (velocity * 2 + ((velocity * velocity) / 127)) / 3;
  int volume = (dynamicVolume * p.volume) / 127;
  volume = constrain(volume, 1, 127);

  DrumOS::Audio::playBuffer(
    DrumOS::Voices::getBuffer(pad),
    DrumOS::Voices::getSamples(pad),
    volume
  );

  p.lastTrigger = millis();

  Serial.print(p.name);
  Serial.print(" peak=");
  Serial.print(peak);
  Serial.print(" velocity=");
  Serial.print(velocity);
  Serial.print(" volume=");
  Serial.println(volume);
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
          if (!isCrosstalk(i, p.peak, now)) {
            triggerPad(i, p.peak);
            lastTriggeredPad = i;
            lastTriggeredPeak = p.peak;
            lastTriggeredAt = now;
          }

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
