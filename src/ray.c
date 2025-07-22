#include "ray.h"

ray_t rays[NUM_RAYS];

float degrees_to_radians(float degrees) {
  return degrees * (PI / 180.0f);
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

  bool is_ray_facing_right = ray_angle < degrees_to_radians(90.0f) || ray_angle > degrees_to_radians(270.0f);
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
    map_contains_point(next_horizontal_touch_x, next_horizontal_touch_y)
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
    map_contains_point(next_vertical_touch_x, next_vertical_touch_y)
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
    float angle = player.rotation_angle + atan((col - NUM_RAYS / 2) / DISTANCE_TO_PROJECTION_PLANE);
    cast_ray(angle, col);
  }
}

void render_rays(void) {
  for (int i = 0; i < NUM_RAYS; i++) {
    draw_line(
      MINIMAP_SCALE_FACTOR * player.x,
      MINIMAP_SCALE_FACTOR * player.y,
      MINIMAP_SCALE_FACTOR * rays[i].wall_hit_x,
      MINIMAP_SCALE_FACTOR * rays[i].wall_hit_y,
      0xFFAAFFAA
    );
  }
}
