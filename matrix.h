#ifndef MATRIX_H
#define MATRIX_H
#include <stdint.h>

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} rgb_color;

//extern rgb_color _binary_image_raw_start[64] ;

void matrix_init(void);
void test_pixels(rgb_color *val);
void set_test_image();
void display_image(const rgb_color *image);
#endif