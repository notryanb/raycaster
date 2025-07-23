#include "constants.h"
#include "graphics.h"

#include <math.h>
#include <SDL2/SDL.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* color_buffer_texture = NULL;
static color_t* color_buffer = NULL;

bool initialize_window() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "error initializing SDL.\n");
    return false;
  }

  // Get full screen resolution
  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  
  window = SDL_CreateWindow(
    NULL, //"Ray Caster",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    display_mode.w,
    display_mode.h,
    SDL_WINDOW_BORDERLESS
  );

  if (!window) {
      fprintf(stderr, "error creating SDL window.\n");
      return false;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  
  if (!renderer) {
      fprintf(stderr, "error creating SDL renderer.\n");
      return false;
  }

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  // allocate the total amount of bytes in memory to hold the color buffer
  color_buffer = (color_t*) malloc(sizeof(color_t) * (color_t)WINDOW_WIDTH * (color_t)WINDOW_HEIGHT);
  
  color_buffer_texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGBA32,
    SDL_TEXTUREACCESS_STREAMING,
    WINDOW_WIDTH,
    WINDOW_HEIGHT
  );

  return true;
}

void destroy_window() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  free(color_buffer);

  SDL_DestroyTexture(color_buffer_texture);
  SDL_Quit();
}

void clear_color_buffer(color_t color) {
  if (color_buffer) {
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
        color_buffer[i] = color;
    }
  }
}

void render_color_buffer() {
  int pitch = (int)(sizeof(color_t) * (color_t)WINDOW_WIDTH);
  SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, pitch);
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
  SDL_RenderPresent(renderer); 
}

void draw_pixel(int x, int y, color_t color) {
  color_buffer[(WINDOW_WIDTH * y) + x] = color;
}

void draw_rect(int x, int y, int width, int height, color_t color) {
  for (int i = x; i <= (x + width); i++) {
    for (int j = y; j <= (y + height); j++) {
      draw_pixel(i, j, color);
    }
  }  
}

void change_color_intensity(color_t* color, float factor) {
  color_t a = (*color & 0xFF000000);
  color_t r = (*color & 0x00FF0000) * factor;
  color_t g = (*color & 0x0000FF00) * factor;
  color_t b = (*color & 0x000000FF) * factor;

  *color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}

void draw_line(int x0, int y0, int x1, int y1, color_t color) {
  // DDA algorithm
  int dx = x1 - x0;
  int dy = y1 - y0;

  int longest_side_length = abs(dx) >= abs(dy) ? abs(dx) : abs(dy);

  float x_inc = dx / (float)longest_side_length;
  float y_inc = dy / (float)longest_side_length;

  float current_x = x0;
  float current_y = y0;

  for (int i = 0; i < longest_side_length; i++) {
    draw_pixel(round(current_x), round(current_y), color);
    current_x += x_inc;
    current_y += y_inc;
  }
  
}
