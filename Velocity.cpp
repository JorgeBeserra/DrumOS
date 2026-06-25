#include <Arduino.h>
#include <math.h>
#include "Velocity.h"

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

  int velocity = 35 + (int)(y * 92.0f);
  return constrain(velocity, 35, 127);
}

}
}