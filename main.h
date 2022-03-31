#ifndef MAIN_H
#define MAIN_H
#include <mbed.h>

extern uint8_t button_counter ;
extern EventFlags event_flags;

extern Queue<rgb_color, 2> matrix_full;
extern Queue<rgb_color, 2> matrix_empty;

#endif
