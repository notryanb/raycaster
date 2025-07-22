#include "constants.h"
#include "graphics.h"
#include "map.h"
#include "player.h"
#include "ray.h"
#include "textures.h"
#include "wall.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <SDL2/SDL.h>

bool is_game_running = false;
int ticks_last_frame = 0;
bool should_display_minimap = false;
texture_t wall_textures[NUM_TEXTURES];

void process_input() {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
    case SDL_QUIT: {
        is_game_running = false;
        break;
      }
    case SDL_KEYDOWN: {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          is_game_running = false;
        }
        if (event.key.keysym.sym == SDLK_UP) {
          player.walk_direction = +1;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          player.walk_direction = -1;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          player.turn_direction = +1;
        }
        if (event.key.keysym.sym == SDLK_LEFT) {
          player.turn_direction = -1;
        }
        if (event.key.keysym.sym == SDLK_m) {
          if (should_display_minimap) {
            should_display_minimap = 0;
          } else {
            should_display_minimap = 1;
          }
        }
        break;
      }
    case SDL_KEYUP: {
        if (event.key.keysym.sym == SDLK_UP) {
          player.walk_direction = 0;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          player.walk_direction = 0;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          player.turn_direction = 0;
        }
        if (event.key.keysym.sym == SDLK_LEFT) {
          player.turn_direction = 0;
        }
        break;
      }
    default:
      break;
  }
}

void setup() {
  load_wall_textures(wall_textures);
}

void update() {
  /* Delta Time */
  int time_to_wait = FRAME_TIME_LENGTH - (SDL_GetTicks() - ticks_last_frame);
  
  if (time_to_wait > 0 && time_to_wait <= FRAME_TIME_LENGTH) {
    SDL_Delay(time_to_wait);
  }  

  float delta_time = (SDL_GetTicks() - ticks_last_frame) / 1000.0f;
  ticks_last_frame = SDL_GetTicks();

 /* Update Objects */
 move_player(delta_time);
 cast_all_rays();
}

void render() {
  clear_color_buffer(0xFF000000);
  generate_3d_projection(wall_textures);

  if (should_display_minimap) {
    render_map();
    render_rays();
    render_player();
  }

  render_color_buffer();
}

void release_resources(void) {
  free_wall_textures(wall_textures);
  destroy_window();
}

int main() {
  is_game_running = initialize_window();
  setup();
  
  while(is_game_running) {
    process_input();
    update();
    render();
  }

  release_resources();

  return 0;
}
