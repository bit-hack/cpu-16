#pragma once
#include <assert.h>
#include <string.h>

#include "console.h"

struct con_point_t {
    int32_t x_, y_;
};

struct console_t {
    con_point_t  caret_;
    con_rect_t   window_;
    con_buffer_t buffer_;
};

template <typename type_t>
static inline
type_t min_(type_t a, type_t b) {
    return a < b ? a : b;
}

template <typename type_t>
static inline
type_t max_(type_t a, type_t b){
    return a > b ? a : b;
}

template <typename type_t>
static inline
type_t clamp_(type_t in, type_t min, type_t max) {
    return max_(min_(in, max), min);
}

console_t * con_new(uint32_t width, uint32_t height) {

    console_t * con = new console_t;
    assert(con);
    memset(con, 0, sizeof(console_t));

    if (!(width && height))
        return nullptr;

    con->buffer_.width_  = width;
    con->buffer_.height_ = height;

    uint32_t area = width * height;
    con->buffer_.char_ = new uint8_t[area];
    con->buffer_.attrib_ = new uint8_t[area];
    memset(con->buffer_.char_, 0, area);
    memset(con->buffer_.attrib_, 0, area);

    return con;
}

void con_free(console_t * con) {
    assert(con);
    assert(con->buffer_.char_);
    assert(con->buffer_.attrib_);
    delete[] con->buffer_.char_;
    delete[] con->buffer_.attrib_;
    delete con;
}

void con_set_caret(console_t *con, uint32_t x, uint32_t y) {
    assert(con);

    const int32_t width = con->buffer_.width_;
    const int32_t height = con->buffer_.height_;

    int32_t & cx = con->caret_.x_;
    int32_t & cy = con->caret_.y_;

    if (x != -1)
        cx = clamp_<uint32_t>( x, 0, width -1 );

    if (y != -1)
        cy = clamp_<uint32_t>( y, 0, height-1 );

    assert(cx >= 0 && cx < width);
    assert(cy >= 0 && cy < height);
}

void con_puts(console_t *con, const char * str, uint32_t max) {
    while (*str && max>0) {
        con_putc(con, *str);
        ++str;
        --max;
    }
}

void con_printf(console_t *, const char * fmt, ...) {

}

void con_putc(console_t *con, const char ch) {
    assert(con);

    const int32_t width  = con->buffer_.width_;
    const int32_t height = con->buffer_.height_;

    int32_t & cx = con->caret_.x_;
    int32_t & cy = con->caret_.y_;

    if (cx >= width) {
        cx  = 0;
        cy += 1;
    }

    if (cy >= height) {
        // scroll the window
        cy -= 1;
    }

    assert(cx >= 0 && cx < width);
    assert(cy >= 0 && cy < height);
    con->buffer_.char_[cx + cy * width] = ch;
    ++cx;
}

void con_fill(console_t *con, con_rect_t * area, const char fill_ch) {
    assert(con);

    const int32_t width = con->buffer_.width_;
    const int32_t height = con->buffer_.height_;

    // fast path for full screen
    if (!area) {
        memset(con->buffer_.char_, fill_ch, width*height);
        return;
    }

    int32_t x1 = clamp_<int32_t>(area->x1_, 0, width - 1);
    int32_t y1 = clamp_<int32_t>(area->y1_, 0, height - 1);
    int32_t x2 = clamp_<int32_t>(area->x2_, 0, width - 1);
    int32_t y2 = clamp_<int32_t>(area->y2_, 0, height - 1);

    uint8_t * cy = con->buffer_.char_;
    cy += y1 * con->buffer_.width_;
    cy += x1;

    for (int32_t y = y1; y <= y2; ++y) {
        uint8_t * cx = cy;
        for (int32_t x = x1; x <= x2; ++x, ++cx)
            *cx = fill_ch;
        cy += width;
    }
}

void con_scroll(console_t *, con_rect_t * area, int dx, int dy) {
}

void con_get_caret(console_t *con, int * x, int * y) {
    assert(con);
    if (x) *x = con->caret_.x_;
    if (y) *y = con->caret_.y_;
}

void con_set_window(console_t *, con_rect_t * area) {
}

void con_get_buffer(console_t *con, con_buffer_t * info) {
    assert(con);
    assert(info);
    *info = con->buffer_;
}
