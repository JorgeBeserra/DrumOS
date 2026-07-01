#pragma once

#include <Arduino.h>

namespace DrumOS {
namespace Velocity {

enum Curve {
  LINEAR,
  SOFT,
  HARD
};

int apply(int raw, int threshold, int peakMax, Curve curve);

}
}
