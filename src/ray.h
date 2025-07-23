#ifndef RAY_H
#define RAY_H

#include "constants.h"
#include "map.h"
#include "player.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>

typedef struct {
  float ray_angle;
  float wall_hit_x;
  float wall_hit_y;
  float distance;
  bool was_hit_vertical;
  int wall_hit_content;
} ray_t;

extern ray_t rays[NUM_RAYS];

float degrees_to_radians(float degrees);
void normalize_angle(float* angle);
float distance_between_points(float x1, float y1, float x2, float y2);
void cast_ray(float ray_angle, int strip_id);
void cast_all_rays(void);
void render_rays(void);

#endif
