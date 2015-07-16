#pragma once

#include "console.h"
#include "cpu16.h"

struct SDL_Surface;

struct state_t {

    console_t * console_;
    cpu16_t * cpu_;
    SDL_Surface * screen_;
};