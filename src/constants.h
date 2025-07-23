#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <math.h>
#include <stdint.h>

#define PI 3.14159265
#define TWO_PI 6.28318530

#define TILE_SIZE 64
#define MAP_NUM_ROWS 13 
#define MAP_NUM_COLS 20
#define MINIMAP_SCALE_FACTOR 0.3
#define MAP_WIDTH MAP_NUM_COLS * TILE_SIZE
#define MAP_HEIGHT MAP_NUM_ROWS * TILE_SIZE

// 16:10 ratio
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800
#define NUM_TEXTURES 9

// In Radians
#define FOV_ANGLE (60 * PI / 180)

#define NUM_RAYS WINDOW_WIDTH
#define DISTANCE_TO_PROJECTION_PLANE ((WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2))

#define FPS 30
#define FRAME_TIME_LENGTH (1000 / FPS)

typedef uint32_t color_t;

#endif
