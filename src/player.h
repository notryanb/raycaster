#ifndef PLAYER_H
#define PLAYER_H

#include "constants.h"
#include "graphics.h"
#include "map.h"

#include <math.h>

typedef struct {
  float x;
  float y;
  float width;
  float height;
  int turn_direction; // -1 for left, +1 for right
  int walk_direction; // -1 for backwards, +1 forwards
  float rotation_angle;
  float rotation_speed;
  float move_speed;
} player_t;

extern player_t player;

void move_player(float delta_time);
void render_player(void);

#endif
