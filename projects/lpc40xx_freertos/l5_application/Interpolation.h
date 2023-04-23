#include <stdint.h>
#include <stdio.h>
// Define a struct for x-y data pairs
typedef struct {
  float x;
  float y;
} interpolation_point_xy;

// Perform linear interpolation of y-values for given x-values
float linear_interp(float x1, const interpolation_point_xy table[], uint8_t length);