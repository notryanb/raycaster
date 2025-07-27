#include <stdio.h>
#include "textures.h"

static const char* texture_file_names[NUM_TEXTURES] = {        
  // Wall textures
  // No texture = 0 for the map
  "./images/redbrick.png", // 1
  "./images/purplestone.png", // 2
  "./images/mossystone.png", // 3
  "./images/graystone.png", // 4
  "./images/colorstone.png", // 5
  "./images/bluestone.png", // 6
  "./images/wood.png", // 7 
  "./images/eagle.png", // 8  
  "./images/pikuma.png", // 9
  // Sprites - these are 0 based indexes.
  "./images/barrel.png", // 9
  "./images/light.png", // 10
  "./images/table.png", // 11
  "./images/guard.png", // 12
  "./images/armor.png", // 13
};


void load_textures(texture_t* textures) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    upng_t* upng;
    upng = upng_new_from_file(texture_file_names[i]);

    if (upng != NULL) {
      upng_decode(upng);

      if (upng_get_error(upng) == UPNG_EOK) {
        // TODO - Maybe copy the buffer so we don't have to store
        // the upng pointer and can free immediately
        textures[i].upng_ptr = upng;
        textures[i].width = upng_get_width(upng);
        textures[i].height = upng_get_height(upng);
        textures[i].texture_buffer = (color_t*)upng_get_buffer(upng);
      } else {
        printf("Error decoding texture %s \n", texture_file_names[i]);
      }
    } else {
      printf("Error loading texture %s \n", texture_file_names[i]);
    }
  }
}

void free_textures(texture_t* textures) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    upng_t* upng = textures[i].upng_ptr;
    upng_free(upng);
  }
}
