#pragma once

#include <stdint.h>

struct console_t;

struct con_rect_t {

    int32_t x1_, y1_;
    int32_t x2_, y2_;
};

struct con_buffer_t {

    uint32_t width_, height_;
    uint8_t * char_;
    uint8_t * attrib_;
};

console_t * con_new( uint32_t width, uint32_t height );

void con_free( console_t * );

void con_set_caret(console_t *, uint32_t x, uint32_t y);

void con_puts(console_t *, const char * str, uint32_t max);

void con_putc(console_t *, const char ch);

void con_printf( console_t *, const char * fmt, ... );

void con_fill( console_t *, con_rect_t * area, const char ch );

void con_scroll( console_t *, con_rect_t * area, int dx, int dy );

void con_get_caret( console_t *, int * x, int * y );

void con_set_window( console_t *, con_rect_t * area );

void con_get_buffer(console_t *, con_buffer_t * info);
