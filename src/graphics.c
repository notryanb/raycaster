#include "constants.h"
#include "graphics.h"

#include <SDL2/SDL.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* color_buffer_texture = NULL;
static uint32_t* color_buffer = NULL;

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
  color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * (uint32_t)WINDOW_WIDTH * (uint32_t)WINDOW_HEIGHT);
  
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

void clear_color_buffer(uint32_t color) {
  if (color_buffer) {
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
        color_buffer[i] = color;
    }
  }
}

void render_color_buffer() {
  int pitch = (int)(sizeof(uint32_t) * (uint32_t)WINDOW_WIDTH);
  SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, pitch);
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
  SDL_RenderPresent(renderer); 
}

void draw_pixel(int x, int y, uint32_t color) {
  color_buffer[(WINDOW_WIDTH * y) + x] = color;
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
  for (int i = x; i <= (x + width); i++) {
    for (int j = y; j <= (y + height); j++) {
      draw_pixel(i, j, color);
    }
  }  
}
