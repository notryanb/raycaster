#ifndef SPRITE_H
#define SPRITE_H

#include "constants.h"
#include "graphics.h"
#include "player.h"
#include "ray.h"
#include "textures.h"
#include "utils.h"

#include <stdbool.h>

#define NUM_SPRITES 3

typedef struct {
  float x;
  float y;
  float distance;
  float angle;
  int texture;
  bool is_visible;
} sprite_t;

static sprite_t sprites[NUM_SPRITES] = {
  { .x = 640, .y = 630, .texture = 13 },
  { .x = 290, .y = 640, .texture =  9 },
  { .x = 300, .y = 400, .texture = 11 }
};

void render_sprite_projection(texture_t *textures);
void render_map_sprites(void);

#endif
