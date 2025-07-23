#include "wall.h"

void generate_3d_projection(texture_t* wall_textures) {
  for (int x = 0; x < NUM_RAYS; x++) {
    // Fix "fisheye" distortion
    float perpendicular_distance = rays[x].distance * cos(rays[x].ray_angle - player.rotation_angle);
    float projected_wall_height = (TILE_SIZE / perpendicular_distance) * (float)DISTANCE_TO_PROJECTION_PLANE;

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
      
      color_t texel_color = wall_textures[texture_num].texture_buffer[(texture_width * texture_offset_y) + texture_offset_x];

      if (rays[x].was_hit_vertical) {
        change_color_intensity(&texel_color, 0.55f);
      }

      draw_pixel(x, y, texel_color);
    }

    // Floor render
    for (int y = wall_bottom_pixel; y < WINDOW_HEIGHT; y++) {
        draw_pixel(x, y,  0xFFABABAB);
    }
  } 
}
