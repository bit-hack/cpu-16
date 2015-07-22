#include <assert.h>
#include <string.h>

#include "console.h"

extern
uint8_t * font_3x5_get(uint8_t ch);

struct con_point_t {
    int32_t x_, y_;
};

struct console_t {
    con_point_t  caret_;
    con_buffer_t buffer_;
    uint32_t     palette_[16];
    uint8_t      attr_;
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

#define RGB(r,g,b) ((r<<16) | (g<<8) | b)

static
uint32_t g_palette[16] = {
    RGB( 16,  16,  16), // dark grey
    RGB(255, 255, 255), // white
    RGB( 29,  39,  47), // background blue
    RGB(  0, 185, 160), // title green
    RGB(236, 147,   0), // title yellow
    RGB(166, 180, 171), // code text
    RGB(255,  15,  25), // code red
    RGB( 96,  87,  62), // code dull wheat
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
};

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
    con->buffer_.attr_ = new uint8_t[area];
    memset(con->buffer_.char_, 0, area);
    memset(con->buffer_.attr_, 0x52, area);

    con->attr_ = 0x52;

    memcpy(con->palette_, g_palette, sizeof(uint32_t)*16);

    return con;
}

void con_free(console_t * con) {
    assert(con);
    assert(con->buffer_.char_);
    assert(con->buffer_.attr_);
    delete[] con->buffer_.char_;
    delete[] con->buffer_.attr_;
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
    con->buffer_.attr_[cx + cy * width] = con->attr_;
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
    uint8_t * ay = con->buffer_.attr_;

    cy += y1 * con->buffer_.width_;
    cy += x1;

    ay += y1 * con->buffer_.width_;
    ay += x1;

    for (int32_t y = y1; y <= y2; ++y) {
        uint8_t * cx = cy;
        uint8_t * ax = ay;
        for (int32_t x = x1; x <= x2; ++x, ++cx, ++ax) {
            *cx = fill_ch;
            *ay = con->attr_;
        }
        cy += width;
        ay += width;
    }
}

void con_get_buffer(console_t *con, con_buffer_t * info) {
    assert(con);
    assert(info);
    *info = con->buffer_;
}

void con_set_attr(console_t * con, uint8_t attr) {
    assert(con);
    con->attr_ = attr;
}

void con_move(console_t * con, int32_t dx, int32_t dy) {
    assert(con);
    con->caret_.x_ = clamp_<int32_t>( 0, con->caret_.x_+dx, con->buffer_.width_-1 );
    con->caret_.y_ = clamp_<int32_t>( 0, con->caret_.y_+dy, con->buffer_.height_-1 );
}

void con_render(console_t *con, uint32_t * dst, uint32_t pitch, uint32_t height) {

    //
    const uint32_t * palette = con->palette_;

    // console size
    const uint32_t cwidth = con->buffer_.width_;
    const uint32_t cheight = con->buffer_.height_;

    // font size
    const uint32_t fwidth  = 4;
    const uint32_t fheight = 6;

    // source buffers
    const uint8_t * chr = con->buffer_.char_;
    const uint8_t * att = con->buffer_.attr_;

    // destination pointer
    uint32_t * py = dst;

    // vertical slots
    for (uint32_t y=0; y<cheight; ++y, py+=pitch*fheight) {
        uint32_t * px = py;

        // horizontal chars
        for (uint32_t x = 0; x<cwidth; ++x, ++chr, ++att, px += fwidth) {
            uint32_t * pt = px;

            // lookup foreground and background colours
            uint32_t bgnd = palette[att[0] & 0xf];
            uint32_t fgnd = palette[att[0] >> 4];

            // lookup character
            const uint8_t * fnt = font_3x5_get(*chr);

            // font scan lines
            for (int i=0; i<fheight-1; ++i, pt+=pitch, fnt+=3) {

                uint32_t f0 = fnt[0];
                pt[0] = ((~f0+1) & fgnd) | ((f0-1) & bgnd);

                uint32_t f1 = fnt[1];
                pt[1] = ((~f1+1) & fgnd) | ((f1-1) & bgnd);

                uint32_t f2 = fnt[2];
                pt[2] = ((~f2+1) & fgnd) | ((f2-1) & bgnd);

                pt[3] = bgnd;
            }

            pt[0] = pt[1] = pt[2] = pt[3] = bgnd;
        }
    }
}
