#include <Arduino.h>
#include <math.h>
#include "Engine/Velocity.h"

namespace DrumOS {
namespace Velocity {

int apply(int raw, int threshold, Curve curve) {
  raw = constrain(raw, threshold, 4095);

  float x = (float)(raw - threshold) / (4095 - threshold);
  x = constrain(x, 0.0f, 1.0f);

  float y;

  switch (curve) {
    case SOFT:
      y = sqrt(x);
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