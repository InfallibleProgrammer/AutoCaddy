#include "Interpolation.h"

float linear_interp(float x1, const interpolation_point_xy table[], uint8_t length) {
  float y1;

  if (x1 <= table[0U].x) {
    y1 = table[0U].y;
  } else if (x1 >= table[length - 1U].x) {
    y1 = table[length - 1U].y;
  } else {
    uint8_t idx0 = 0U;

    if (length > 2U) {
      while ((x1 > table[idx0 + 1U].x) && (idx0 < (length - 2U))) {
        idx0++;
      }
    }

    float stepY = table[idx0 + 1U].y - table[idx0].y;
    float stepX = table[idx0 + 1U].x - table[idx0].x;
    float deltaX = x1 - table[idx0].x;

    y1 = table[idx0].y + (stepY * (deltaX / stepX));
  }

  return y1;
}