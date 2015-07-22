#pragma once

#include <stdint.h>

struct console_t;
struct cpu16_t;

struct screen_t {

    static const uint32_t pitch_ = 320;
    static const uint32_t height_ = 240;
    uint32_t pixel_[ pitch_ * height_ ];
};

struct state_t {

    bool running_;

    console_t * console_;
    cpu16_t * cpu_;
    screen_t screen_;
};
