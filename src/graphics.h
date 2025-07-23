#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "constants.h"
#include <stdbool.h>
#include <stdint.h>

bool initialize_window(void);
void destroy_window(void);
void clear_color_buffer(color_t color);
void render_color_buffer(void);
void draw_pixel(int x, int y, color_t color);
void draw_rect(int x, int y, int width, int height, color_t color);
void change_color_intensity(color_t* color, float factor);
void draw_line(int x0, int y0, int x1, int y1, color_t color);


#endif
