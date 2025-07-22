#include "constants.h"
#include "graphics.h"
#include "map.h"
#include "player.h"
#include "ray.h"
#include "textures.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <memory.h>
#include <limits.h>
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

void generate_3d_projection() {
  for (int x = 0; x < NUM_RAYS; x++) {
    // Fix "fisheye" distortion
    float perpendicular_distance = rays[x].distance * cos(rays[x].ray_angle - player.rotation_angle);
    float projected_wall_height = (TILE_SIZE / perpendicular_distance) * DISTANCE_TO_PROJECTION_PLANE;

    int wall_top_pixel = (WINDOW_HEIGHT / 2.0f) - (projected_wall_height / 2.0f);
    wall_top_pixel = wall_top_pixel < 0 ? 0 : wall_top_pixel;

    int wall_bottom_pixel = (WINDOW_HEIGHT / 2.0f) + (projected_wall_height / 2.0f);
    wall_bottom_pixel = wall_bottom_pixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wall_bottom_pixel;

    // Ceiling render
    for (int y = 0; y < wall_top_pixel; y++) {
        draw_pixel(x, y, 0xFF565656);
    }

    // Walls render
    int texture_offset_x;
    if (rays[x].was_hit_vertical) {
      texture_offset_x = (int)rays[x].wall_hit_y % TILE_SIZE;
    } else {
      texture_offset_x = (int)rays[x].wall_hit_x % TILE_SIZE;
    }

    int texture_num = rays[x].wall_hit_content - 1; // 0 is empty space, so there is off-by-one issue
    int texture_width = wall_textures[texture_num].width;
    int texture_height = wall_textures[texture_num].height;

    for (int y = wall_top_pixel; y < wall_bottom_pixel; y++) {
      // Project the textures onto the walls
      int distance_from_top = y + ((int)projected_wall_height / 2) - (WINDOW_HEIGHT / 2);
      int texture_offset_y = distance_from_top * ((float)texture_height / projected_wall_height);
      
      uint32_t texel_color = wall_textures[texture_num].texture_buffer[(texture_width * texture_offset_y) + texture_offset_x];
      draw_pixel(x, y, texel_color);
    }

    // Floor render
    for (int y = wall_bottom_pixel; y < WINDOW_HEIGHT; y++) {
        draw_pixel(x, y,  0xFFABABAB);
    }
  } 
}


void render() {
  clear_color_buffer(0xFF000000);
  generate_3d_projection();

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
