#ifndef RAY_H
#define RAY_H

#include "constants.h"
#include "map.h"
#include "player.h"
#include "utils.h"

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

void cast_ray(float ray_angle, int strip_id);
void cast_all_rays(void);
void render_rays(void);

#endif
