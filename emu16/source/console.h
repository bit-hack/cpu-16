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
    uint8_t * attr_;
};

console_t * con_new(uint32_t width, uint32_t height);

void con_free(console_t *);

void con_set_caret(console_t *, uint32_t x, uint32_t y);

void con_puts(console_t *, const char * str, uint32_t max);

void con_putc(console_t *, const char ch);

void con_fill(console_t *, con_rect_t * area, const char ch );

void con_get_buffer(console_t *, con_buffer_t * info);

void con_set_attr(console_t *, uint8_t attr);

void con_move(console_t *, int32_t x, int32_t y);

void con_render(console_t *, uint32_t * dst, uint32_t pitch, uint32_t height );
