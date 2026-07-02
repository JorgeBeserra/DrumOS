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

static int previousValue[DrumOS::Pads::PAD_COUNT];
static int fallingCount[DrumOS::Pads::PAD_COUNT];
static int retriggerPeak[DrumOS::Pads::PAD_COUNT];
static uint8_t crosstalkMatrix[DrumOS::Pads::PAD_COUNT][DrumOS::Pads::PAD_COUNT];

static const unsigned long CROSSTALK_WINDOW_MS = 25;
static const int DEFAULT_CROSSTALK_RATIO_PERCENT = 45;

static const unsigned long MIN_PEAK_SCAN_MS = 2;
static const int MIN_PEAK_FALL_DELTA = 24;
static const int PEAK_FALL_PERCENT = 4;
static const int REQUIRED_FALLING_SAMPLES = 1;

static const int RETRIGGER_WEAK_PERCENT = 65;
static const int RETRIGGER_RESET_VALUE = 40;

void resetCrosstalkMatrix() {
  for (int source = 0; source < DrumOS::Pads::PAD_COUNT; source++) {
    for (int target = 0; target < DrumOS::Pads::PAD_COUNT; target++) {
      crosstalkMatrix[source][target] = 0;
    }
  }

  crosstalkMatrix[DrumOS::Pads::KICK][DrumOS::Pads::HIHAT] = 15;
  crosstalkMatrix[DrumOS::Pads::SNARE][DrumOS::Pads::HITOM] = 25;
  crosstalkMatrix[DrumOS::Pads::HITOM][DrumOS::Pads::LOWTOM] = 35;
  crosstalkMatrix[DrumOS::Pads::LOWTOM][DrumOS::Pads::HITOM] = 35;
  crosstalkMatrix[DrumOS::Pads::CRASH][DrumOS::Pads::HITOM] = 30;
  crosstalkMatrix[DrumOS::Pads::CRASH][DrumOS::Pads::LOWTOM] = 30;
}

void setCrosstalkLevel(int sourcePad, int targetPad, int level) {
  if (sourcePad < 0 || sourcePad >= DrumOS::Pads::PAD_COUNT) return;
  if (targetPad < 0 || targetPad >= DrumOS::Pads::PAD_COUNT) return;
  if (sourcePad == targetPad) return;
  crosstalkMatrix[sourcePad][targetPad] = constrain(level, 0, 100);
}

int getCrosstalkLevel(int sourcePad, int targetPad) {
  if (sourcePad < 0 || sourcePad >= DrumOS::Pads::PAD_COUNT) return 0;
  if (targetPad < 0 || targetPad >= DrumOS::Pads::PAD_COUNT) return 0;
  return crosstalkMatrix[sourcePad][targetPad];
}

void printCrosstalkMatrix() {
  Serial.println("Crosstalk Matrix:");
  for (int source = 0; source < DrumOS::Pads::PAD_COUNT; source++) {
    for (int target = 0; target < DrumOS::Pads::PAD_COUNT; target++) {
      if (source == target) continue;
      int level = crosstalkMatrix[source][target];
      if (level <= 0) continue;
      Serial.print(DrumOS::Pads::pads[source].name);
      Serial.print(" -> ");
      Serial.print(DrumOS::Pads::pads[target].name);
      Serial.print(" = ");
      Serial.println(level);
    }
  }
}

static bool isCrosstalk(int pad, int peak, unsigned long now) {
  if (lastTriggeredPad < 0) return false;
  if (pad == lastTriggeredPad) return false;
  if (now - lastTriggeredAt > CROSSTALK_WINDOW_MS) return false;
  if (lastTriggeredPeak <= 0) return false;

  int level = crosstalkMatrix[lastTriggeredPad][pad];
  if (level <= 0) return false;

  int ratio = (peak * 100) / lastTriggeredPeak;
  int allowedRatio = max(5, DEFAULT_CROSSTALK_RATIO_PERCENT - level);
  return ratio < allowedRatio;
}

static bool hasPeakStartedFalling(int pad, int value, unsigned long elapsedMs) {
  if (elapsedMs < MIN_PEAK_SCAN_MS) return false;
  auto& p = DrumOS::Pads::pads[pad];
  int fallDelta = max(MIN_PEAK_FALL_DELTA, (p.peak * PEAK_FALL_PERCENT) / 100);
  if (previousValue[pad] - value >= fallDelta || p.peak - value >= fallDelta) {
    fallingCount[pad]++;
  } else {
    fallingCount[pad] = 0;
  }
  return fallingCount[pad] >= REQUIRED_FALLING_SAMPLES;
}

static void startMaskTime(int pad, unsigned long now) {
  auto& p = DrumOS::Pads::pads[pad];
  p.timer = now;
  p.state = DrumOS::Pads::MASK_TIME;
  fallingCount[pad] = 0;
  previousValue[pad] = 0;
}

static void finishPeakScan(int pad, unsigned long now) {
  auto& p = DrumOS::Pads::pads[pad];
  if (!isCrosstalk(pad, p.peak, now)) {
    triggerPad(pad, p.peak);
    lastTriggeredPad = pad;
    lastTriggeredPeak = p.peak;
    lastTriggeredAt = now;
  }
  if (p.retriggerLockMs > 0) {
    p.timer = now;
    p.state = DrumOS::Pads::RETRIGGER_LOCK;
    retriggerPeak[pad] = p.peak;
  } else {
    startMaskTime(pad, now);
  }
  fallingCount[pad] = 0;
  previousValue[pad] = 0;
}

static bool isStrongNewHitDuringLock(int pad, int value) {
  auto& p = DrumOS::Pads::pads[pad];
  if (value <= p.threshold) return false;
  if (value <= RETRIGGER_RESET_VALUE) return false;
  int strongLimit = max(p.threshold, (retriggerPeak[pad] * RETRIGGER_WEAK_PERCENT) / 100);
  return value >= strongLimit;
}

void begin() {
  scopePad = -1;
  lastTriggeredPad = -1;
  lastTriggeredPeak = 0;
  lastTriggeredAt = 0;
  resetCrosstalkMatrix();
  for (int i = 0; i < DrumOS::Pads::PAD_COUNT; i++) {
    previousValue[i] = 0;
    fallingCount[i] = 0;
    retriggerPeak[i] = 0;
  }
}

void triggerPad(int pad, int peak) {
  if (pad < 0 || pad >= DrumOS::Pads::PAD_COUNT) return;
  auto& p = DrumOS::Pads::pads[pad];
  int velocity = DrumOS::Velocity::apply(peak, p.threshold, p.peakMax, p.curve);
  int dynamicVolume = (velocity * 2 + ((velocity * velocity) / 127)) / 3;
  int volume = (dynamicVolume * p.volume) / 127;
  volume = constrain(volume, 1, 127);
  DrumOS::Audio::playBuffer(DrumOS::Voices::getBuffer(pad), DrumOS::Voices::getSamples(pad), volume);
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
          previousValue[i] = value;
          fallingCount[i] = 0;
          p.state = DrumOS::Pads::WAIT_PEAK;
        }
        break;
      case DrumOS::Pads::WAIT_PEAK: {
        unsigned long elapsed = now - p.timer;
        if (value > p.peak) {
          p.peak = value;
          fallingCount[i] = 0;
        }
        bool peakStartedFalling = hasPeakStartedFalling(i, value, elapsed);
        bool scanTimedOut = elapsed >= (unsigned long)p.scanMs;
        previousValue[i] = value;
        if (peakStartedFalling || scanTimedOut) finishPeakScan(i, now);
        break;
      }
      case DrumOS::Pads::RETRIGGER_LOCK: {
        unsigned long elapsed = now - p.timer;
        if (value > retriggerPeak[i]) retriggerPeak[i] = value;
        bool lockExpired = elapsed >= (unsigned long)p.retriggerLockMs;
        bool signalReturnedToRest = value <= RETRIGGER_RESET_VALUE;
        if (lockExpired || signalReturnedToRest) {
          startMaskTime(i, now);
        } else if (isStrongNewHitDuringLock(i, value)) {
          p.peak = value;
          p.timer = now;
          previousValue[i] = value;
          fallingCount[i] = 0;
          p.state = DrumOS::Pads::WAIT_PEAK;
        }
        break;
      }
      case DrumOS::Pads::MASK_TIME:
        if (now - p.timer >= (unsigned long)p.debounceMs) {
          p.peak = 0;
          previousValue[i] = 0;
          fallingCount[i] = 0;
          retriggerPeak[i] = 0;
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
    if (value <= SCOPE_ZERO_VALUE) value = 0;
    bool shouldPrint = false;
    if (firstScopePrint) { shouldPrint = true; firstScopePrint = false; }
    else if (lastScopeValue == 0 && value > 0) shouldPrint = true;
    else if (lastScopeValue > 0 && value == 0) shouldPrint = true;
    else if (value > 0 && abs(value - lastScopeValue) >= SCOPE_DELTA) shouldPrint = true;
    if (shouldPrint) {
      Serial.print(DrumOS::Pads::pads[scopePad].name);
      Serial.print(": ");
      Serial.println(value);
      lastScopeValue = value;
    }
  }
}

void setScopePad(int pad) {
  if (pad >= 0 && pad < DrumOS::Pads::PAD_COUNT) scopePad = pad;
  else scopePad = -1;
  lastScopeValue = -1;
  firstScopePrint = true;
}

int getScopePad() {
  return scopePad;
}

}
}
