#include <Arduino.h>
#include <math.h>
#include "Engine/Velocity.h"

namespace DrumOS {
namespace Velocity {

int apply(int raw, int threshold, int peakMax, Curve curve) {
  if (peakMax <= threshold) {
    peakMax = threshold + 1;
  }

  raw = constrain(raw, threshold, peakMax);

  float x = (float)(raw - threshold) / (float)(peakMax - threshold);
  x = constrain(x, 0.0f, 1.0f);

  float y;

  switch (curve) {
    case SOFT:
      y = sqrtf(x);
      break;

    case HARD:
      y = x * x;
      break;

    case LINEAR:
    default:
      y = x;
      break;
  }

  int velocity = 8 + (int)(y * 119.0f);
  return constrain(velocity, 8, 127);
}

}
}
