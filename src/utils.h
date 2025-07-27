#ifndef UTILS_H
#define UTILS_H

#include "constants.h"
#include <math.h>

float degrees_to_radians(float degrees);
void normalize_angle(float* angle);
float distance_between_points(float x1, float y1, float x2, float y2);

#endif
