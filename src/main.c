#include "constants.h"
#include "graphics.h"
#include "map.h"
#include "textures.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <memory.h>
#include <limits.h>
#include <SDL2/SDL.h>

struct Player {
  float x;
  float y;
  float width;
  float height;
  int turn_direction; // -1 for left, +1 for right
  int walk_direction; // -1 for backwards, +1 forwards
  float rotation_angle;
  float rotation_speed;
  float move_speed;
} player;

struct Ray {
  float ray_angle;
  float wall_hit_x;
  float wall_hit_y;
  float distance;
  bool was_hit_vertical;
  int wall_hit_content;
} rays[NUM_RAYS];

bool is_game_running = false;
int ticks_last_frame = 0;
bool should_display_minimap = false;
float distance_to_projection_plane = 0;
texture_t wall_textures[NUM_TEXTURES];

/// HELPER FUNCTIONS
float degrees_to_radians(float degrees) {
  return degrees * (PI / 180.0f);
}



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
  distance_to_projection_plane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);

  player.x = WINDOW_WIDTH / 2.0f;
  player.y = WINDOW_HEIGHT / 2.0f;
  player.width = 1;
  player.height = 1;
  player.turn_direction = 0;
  player.walk_direction = 0;
  player.move_speed = 200.0f;
  player.rotation_angle = degrees_to_radians(90);
  player.rotation_speed = degrees_to_radians(45);

  load_wall_textures(wall_textures);
}

void move_player(float delta_time) {
  player.rotation_angle += player.turn_direction * player.rotation_speed * delta_time;

  float move_step = player.walk_direction * player.move_speed * delta_time;
  float new_player_x = player.x + cos(player.rotation_angle) * move_step;
  float new_player_y = player.y + sin(player.rotation_angle) * move_step;

  if (!map_has_wall_at(new_player_x, new_player_y)) {
    player.x = new_player_x;
    player.y = new_player_y;
  }
}

float normalize_angle(float angle) {
  angle = remainder(angle, TWO_PI);

  if (angle < 0) {
    angle += TWO_PI;
  }

  return angle;
}

float distance_between_points(float x1, float y1, float x2, float y2) {
  return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void cast_ray(float ray_angle, int strip_id) {
  ray_angle = normalize_angle(ray_angle);

  bool is_ray_facing_down = ray_angle > 0 && ray_angle < PI;
  bool is_ray_facing_up = !is_ray_facing_down;

  bool is_ray_facing_right = ray_angle < degrees_to_radians(90) || ray_angle > degrees_to_radians(270);
  bool is_ray_facing_left = !is_ray_facing_right;

  float x_intercept, y_intercept;
  float x_step, y_step;

  // Horizontal cast: Walk the map grid, not individual pixels
  bool found_horizontal_wall_hit = false;
  int horizontal_wall_hit_content = 0;
  float horizontal_wall_hit_x = 0;
  float horizontal_wall_hit_y = 0;

  // Find y coordinate of the closest horizontal GRID intersection
  y_intercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
  y_intercept += is_ray_facing_down ? TILE_SIZE : 0;

  // Find x coordinate of the closest horizontal GRID intersection
  x_intercept = player.x + (y_intercept - player.y) / tan(ray_angle);

  y_step = TILE_SIZE;
  y_step *= is_ray_facing_up ? -1 : 1;

  x_step = TILE_SIZE / tan(ray_angle);
  x_step *= (is_ray_facing_left && x_step > 0) ? -1 : 1;
  x_step *= (is_ray_facing_right && x_step < 0) ? -1 : 1;

  float next_horizontal_touch_x = x_intercept;
  float next_horizontal_touch_y = y_intercept;

  // Increment x/y Steps until we find a wall
  while (
    next_horizontal_touch_x >= 0 
    && next_horizontal_touch_x <= MAP_WIDTH
    && next_horizontal_touch_y >= 0 
    && next_horizontal_touch_y <= MAP_HEIGHT 
  ) {
    int grid_content = map_get_wall_content_at(
      next_horizontal_touch_x,
      next_horizontal_touch_y + (is_ray_facing_up ? -1 : 0)
    );

    if (grid_content > 0) {
      // Found a wall hit
      horizontal_wall_hit_x = next_horizontal_touch_x;
      horizontal_wall_hit_y = next_horizontal_touch_y;
      horizontal_wall_hit_content = grid_content;
      found_horizontal_wall_hit = true;
      break;
    } else {
      next_horizontal_touch_x += x_step;
      next_horizontal_touch_y += y_step;
    }
  }

  // Vertical cast: Walk the map grid, not individual pixels
  int vertical_wall_hit_content = 0;
  bool found_vertical_wall_hit = false;
  float vertical_wall_hit_x = 0;
  float vertical_wall_hit_y = 0;

  // Find x coordinate of the closest horizontal GRID intersection
  x_intercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
  x_intercept += is_ray_facing_right ? TILE_SIZE : 0;

  // Find y coordinate of the closest horizontal GRID intersection
  y_intercept = player.y + (x_intercept - player.x) * tan(ray_angle);

  x_step = TILE_SIZE;
  x_step *= is_ray_facing_left ? -1 : 1;

  y_step = TILE_SIZE * tan(ray_angle);
  y_step *= (is_ray_facing_up && y_step > 0) ? -1 : 1;
  y_step *= (is_ray_facing_down && y_step < 0) ? -1 : 1;

  float next_vertical_touch_x = x_intercept;
  float next_vertical_touch_y = y_intercept;

  // Increment x/y Steps until we find a wall
  while (
    next_vertical_touch_x >= 0 
    && next_vertical_touch_x <= MAP_WIDTH
    && next_vertical_touch_y >= 0 
    && next_vertical_touch_y <= MAP_HEIGHT 
  ) {
    int grid_content = map_get_wall_content_at(
      next_vertical_touch_x + (is_ray_facing_left ? -1 : 0),
      next_vertical_touch_y
    );

    if (grid_content > 0) {
      // Found a wall hit
      vertical_wall_hit_x = next_vertical_touch_x;
      vertical_wall_hit_y = next_vertical_touch_y;
      vertical_wall_hit_content = grid_content;
      found_vertical_wall_hit = true;
      break;
    } else {
      next_vertical_touch_x += x_step;
      next_vertical_touch_y += y_step;
    }
  }

  // Calculate both horizontal & vertical distances and choose smallest distance
  float horizontal_hit_distance = found_horizontal_wall_hit 
    ? distance_between_points(player.x, player.y, horizontal_wall_hit_x, horizontal_wall_hit_y) 
    : FLT_MAX;

  float vertical_hit_distance = found_vertical_wall_hit 
    ? distance_between_points(player.x, player.y, vertical_wall_hit_x, vertical_wall_hit_y) 
    : FLT_MAX;

  // TODO: I should probably pass in a ref to the Ray rather than the strip id
  rays[strip_id].ray_angle = ray_angle;

  if (vertical_hit_distance < horizontal_hit_distance) {
    rays[strip_id].distance = vertical_hit_distance;
    rays[strip_id].wall_hit_x = vertical_wall_hit_x;
    rays[strip_id].wall_hit_y = vertical_wall_hit_y;
    rays[strip_id].wall_hit_content = vertical_wall_hit_content;
    rays[strip_id].was_hit_vertical = true;
  } else {
    rays[strip_id].distance = horizontal_hit_distance;
    rays[strip_id].wall_hit_x = horizontal_wall_hit_x;
    rays[strip_id].wall_hit_y = horizontal_wall_hit_y;
    rays[strip_id].wall_hit_content = horizontal_wall_hit_content;
    rays[strip_id].was_hit_vertical = false;
  }
}

void cast_all_rays() {
  for (int col = 0; col < NUM_RAYS; col++) {
    // Get the angle of increment. This is based on the fact that the "hits" will always be equally distanced apart,
    // reducing FOV distortions that curve walls at the extreme of our FOV.
    float angle = player.rotation_angle + atan((col - NUM_RAYS / 2) / distance_to_projection_plane);
    cast_ray(angle, col);
  }
}

void render_rays() {
  // SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

  // for (int i = 0; i < NUM_RAYS; i++) {
  //   SDL_RenderDrawLine(
  //     renderer,
  //     MINIMAP_SCALE_FACTOR * player.x,
  //     MINIMAP_SCALE_FACTOR * player.y,
  //     MINIMAP_SCALE_FACTOR * rays[i].wall_hit_x,
  //     MINIMAP_SCALE_FACTOR * rays[i].wall_hit_y
  //   );
  // }
}


void render_player() {
  // SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
  // SDL_Rect player_rect = {
  //   player.x * MINIMAP_SCALE_FACTOR,
  //   player.y * MINIMAP_SCALE_FACTOR,
  //   player.width * MINIMAP_SCALE_FACTOR,
  //   player.height * MINIMAP_SCALE_FACTOR
  // };

  // SDL_RenderFillRect(renderer, &player_rect);

  // int line_length_px = 32;
  // SDL_RenderDrawLine(
  //   renderer,
  //   player.x * MINIMAP_SCALE_FACTOR,
  //   player.y * MINIMAP_SCALE_FACTOR,
  //   MINIMAP_SCALE_FACTOR * player.x + cos(player.rotation_angle) * line_length_px,
  //   MINIMAP_SCALE_FACTOR * player.y + sin(player.rotation_angle) * line_length_px
  // );
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
    float projected_wall_height = (TILE_SIZE / perpendicular_distance) * distance_to_projection_plane;

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
  // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  // SDL_RenderClear(renderer);

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
