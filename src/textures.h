#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdint.h>
#include "constants.h"
#include "upng.h"

typedef struct {
  int width;
  int height;
  uint32_t* texture_buffer;
  upng_t* upng_ptr;
} texture_t;

texture_t wall_textures[NUM_TEXTURES];
void load_wall_textures();
void free_wall_textures();

#endif
