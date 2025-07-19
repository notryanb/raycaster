#include <stdio.h>
#include "textures.h"

static const char* texture_file_names[NUM_TEXTURES] = {
  "./images/redbrick.png",
  "./images/purplestone.png",
  "./images/mossystone.png",
  "./images/graystone.png",
  "./images/colorstone.png",
  "./images/bluestone.png",
  "./images/wood.png",
  "./images/eagle.png",
  "./images/pikuma.png",
};


void load_wall_textures(texture_t* wall_textures) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    upng_t* upng;
    upng = upng_new_from_file(texture_file_names[i]);

    if (upng != NULL) {
      upng_decode(upng);

      if (upng_get_error(upng) == UPNG_EOK) {
        // TODO - Maybe copy the buffer so we don't have to store
        // the upng pointer and can free immediately
        wall_textures[i].upng_ptr = upng;
        wall_textures[i].width = upng_get_width(upng);
        wall_textures[i].height = upng_get_height(upng);
        wall_textures[i].texture_buffer = (uint32_t*)upng_get_buffer(upng);
      }
    }
  }
}

void free_wall_textures(texture_t* wall_textures) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    upng_t* upng = wall_textures[i].upng_ptr;
    upng_free(upng);
  }
}
