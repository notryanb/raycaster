#include "map.h"

static const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
  {1, 0, 0, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 1, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 1},
  {1, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

int map_get_wall_content_at(float x, float y) {
  if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
    return 0;
  }  

  int map_grid_idx_x = floor(x / TILE_SIZE);
  int map_grid_idx_y = floor(y / TILE_SIZE);

  return map[map_grid_idx_y][map_grid_idx_x];
}

bool map_has_wall_at(float x, float y) {
  if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
    return true;
  }  

  return map_get_wall_content_at(x, y) != 0;
}

bool map_contains_point(float x, float y) {
    return x >= 0 && x <= MAP_WIDTH && y >= 0 &&  y<= MAP_HEIGHT;
}

void render_map(void) {
  for (int i = 0; i < MAP_NUM_ROWS; i++) {
    for (int j = 0; j < MAP_NUM_COLS; j++) {
      int tile_x = j * TILE_SIZE;
      int tile_y = i * TILE_SIZE;
      int tile_color = map[i][j] != 0 ? 0xFFFFFFFF : 0xFF000000;

      draw_rect(
        MINIMAP_SCALE_FACTOR * tile_x,
        MINIMAP_SCALE_FACTOR * tile_y,
        MINIMAP_SCALE_FACTOR * TILE_SIZE,
        MINIMAP_SCALE_FACTOR * TILE_SIZE,
        tile_color
      );
    }
  }
}
