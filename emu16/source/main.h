#pragma once

#include "console.h"
#include "cpu16.h"

struct SDL_Surface;

struct state_t {

    bool breakpoint_[16];

    bool running_;

    console_t * console_;
    cpu16_t * cpu_;
    SDL_Surface * screen_;
};
