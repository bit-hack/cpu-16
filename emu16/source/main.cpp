#include <stdint.h>
#include <stdio.h>

#define _SDL_main_h
#include <SDL.h>

#include "cpu16.h"

struct state_t {

    cpu16_t * cpu_;
    SDL_Surface * screen_;
};

static
void serial_write_8(cpu16_t *, uint16_t addr, uint8_t val, void * user) {

    putchar(val);
}

static
bool app_init(state_t * state, const char * path) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return false;

    state->cpu_ = cpu16_new();
    if (!state->cpu_)
        return false;

    cpu16_bus_t bus = {
        nullptr,
        nullptr,
        nullptr,
        serial_write_8,
        nullptr
    };
    cpu16_add_peripheral(state->cpu_, &bus, 32, 32);

    if (!cpu16_load_image(state->cpu_, path)) {
        printf("failed to load image\n");
        return false;
    }
    cpu16_reset(state->cpu_);

    SDL_WM_SetCaption("CPU-16", nullptr);
    state->screen_ = SDL_SetVideoMode(256, 256, 32, 0);
    if (!state->screen_)
        return false;

    return true;
}

static
bool app_tick(state_t * state) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return false;
    }

    for (uint32_t i = 0; i < (64 * 64); ++i) {

        uint32_t x = i % 64;
        uint32_t y = i / 64;

        uint8_t pix = cpu16_read_byte(state->cpu_, 0x4000 + i);

        SDL_Rect rect = { x*4, y*4, 4, 4 };
        SDL_FillRect(state->screen_, &rect, (pix << 16) | (pix << 8) | pix);
    }

    SDL_Flip(state->screen_);
    return true;
}

int main(int argc, const char ** args) {

    if (argc != 2) {
        printf("%s <input.img>\n", args[0]);
        return -1;
    }

    state_t state;

    if (!app_init(&state, args[1])) {
        return false;
    }

    while (app_tick(&state)) {
        cpu16_run(state.cpu_, 1024);
    }

    return 0;
}
