#include "player.h"

player_t player = {
  .x = WINDOW_WIDTH / 2.0f,
  .y = WINDOW_HEIGHT / 2.0f,
  .width = 1,
  .height = 1,
  .turn_direction = 0,
  .walk_direction = 0,
  .move_speed = 200.0f,
  .rotation_angle = 215.0f * (PI / 180.0f),
  .rotation_speed = 60.0f * (PI / 180.0f)
};

void move_player(float delta_time) {
  player.rotation_angle += player.turn_direction * player.rotation_speed * delta_time;
  normalize_angle(&player.rotation_angle);

  float move_step = player.walk_direction * player.move_speed * delta_time;
  float new_player_x = player.x + cos(player.rotation_angle) * move_step;
  float new_player_y = player.y + sin(player.rotation_angle) * move_step;

  if (!map_has_wall_at(new_player_x, new_player_y)) {
    player.x = new_player_x;
    player.y = new_player_y;
  }
}

void render_player(void) {
  draw_rect(
    MINIMAP_SCALE_FACTOR * player.x,
    MINIMAP_SCALE_FACTOR * player.y,
    MINIMAP_SCALE_FACTOR * player.width,
    MINIMAP_SCALE_FACTOR * player.height,
    0xFF00FF00
  );
}
