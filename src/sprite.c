#include "sprite.h"


void render_sprite_projection(texture_t *textures) {
  sprite_t visible_sprites[NUM_SPRITES];
  int num_visible_sprites = 0;

  for (int i = 0; i < NUM_SPRITES; i++) {
    float angle_sprite_player = 
      player.rotation_angle - atan2(sprites[i].y - player.y, sprites[i].x - player.x);

    if (angle_sprite_player > PI) {
      angle_sprite_player -= TWO_PI;
    }
    if (angle_sprite_player < -PI) {
      angle_sprite_player += TWO_PI;
    }

    angle_sprite_player = fabs(angle_sprite_player);

    const float EPSILON = 0.2;
    if (angle_sprite_player < (FOV_ANGLE / 2) + EPSILON) {
      sprites[i].is_visible = true;
      sprites[i].angle = angle_sprite_player;
      sprites[i].distance = distance_between_points(
        sprites[i].x,
        sprites[i].y,
        player.x,
        player.y
      );
        
      visible_sprites[num_visible_sprites] = sprites[i];
      num_visible_sprites++;
    } else {
      sprites[i].is_visible = false;
    }
  }

  // Sort before drawing
  for(int i = 0; i < num_visible_sprites - 1; i++) {
    for (int j = i + 1; j < num_visible_sprites; j++) {
      if (visible_sprites[i].distance < visible_sprites[j].distance) {
        sprite_t temp = visible_sprites[i];
        visible_sprites[i] = visible_sprites[j];
        visible_sprites[j] = temp;
      }
    }
  }

  // Render
  for (int i = 0; i < num_visible_sprites; i++) {
    sprite_t sprite = visible_sprites[i];

    // Avoid fisheye distortion in sprite rendering
    float perpendicular_distance = sprite.distance * cos(sprite.angle);
    
    float sprite_height = (TILE_SIZE / perpendicular_distance) * DISTANCE_TO_PROJECTION_PLANE;
    float sprite_width = sprite_height;

    float sprite_top_y = (WINDOW_HEIGHT / 2.0f) - (sprite_height / 2.0f);
    sprite_top_y = (sprite_top_y < 0) ? 0 : sprite_top_y;

    float sprite_bottom_y = (WINDOW_HEIGHT / 2.0f) + (sprite_height / 2.0f);
    sprite_bottom_y = (sprite_bottom_y > WINDOW_HEIGHT) ? WINDOW_HEIGHT : sprite_bottom_y;

    // Calculate x pos
    float sprite_angle = atan2(sprite.y - player.y, sprite.x - player.x) - player.rotation_angle;
    float sprite_pos_x = tan(sprite_angle) * (float)DISTANCE_TO_PROJECTION_PLANE;
    float sprite_left_x = (WINDOW_WIDTH / 2.0f) + sprite_pos_x - (sprite_width / 2.0f); // Start drawing sprite here
    float sprite_right_x = sprite_left_x + sprite_width;

    int texture_width = textures[sprite.texture].width;
    int texture_height = textures[sprite.texture].height;

    for(int x = sprite_left_x; x < sprite_right_x; x++) {
      float texel_width = texture_width / sprite_width;
      int texture_offset_x = (x - sprite_left_x) * texel_width;

      for(int y = sprite_top_y; y < sprite_bottom_y; y++) {
        if (x > 0 && x < WINDOW_WIDTH && y > 0 && y < WINDOW_HEIGHT) {
          int distance_from_top = y + (sprite_height / 2.0f) - (WINDOW_HEIGHT / 2.0f);
          int texture_offset_y = distance_from_top * (texture_height / sprite_height);

          color_t *sprite_texture_buffer = textures[sprite.texture].texture_buffer;
          color_t texel_color = sprite_texture_buffer[texture_width * texture_offset_y + texture_offset_x];

          // Do not draw magenta
          if (texel_color != 0xFFFF00FF && (sprite.distance < rays[x].distance)) {
            draw_pixel(x, y, texel_color);
          }
        }
      }
    }
  }
}

void render_map_sprites(void) {
  for (int i = 0; i < NUM_SPRITES; i++) {
    color_t sprite_color = sprites[i].is_visible ? 0xFF00FF00 : 0xFF0000FF;
    draw_rect(
      MINIMAP_SCALE_FACTOR * sprites[i].x,
      MINIMAP_SCALE_FACTOR * sprites[i].y,
      2, 
      2, 
      sprite_color
    );
  }  
}
