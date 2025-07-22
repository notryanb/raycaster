#ifndef MAP_H
#define MAP_H

#include "constants.h"
#include "graphics.h"
#include <stdbool.h>
#include <math.h>

int map_get_wall_content_at(float x, float y);
bool map_has_wall_at(float x, float y);
bool map_contains_point(float x, float y);
void render_map(void);

#endif
