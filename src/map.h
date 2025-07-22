#ifndef MAP_H
#define MAP_H

#include "constants.h"
#include <stdbool.h>

int map_get_wall_content_at(float x, float y);
bool map_has_wall_at(float x, float y);
void render_map(void);

#endif
