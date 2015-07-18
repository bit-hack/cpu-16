#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define _SDL_main_h
#include <SDL.h>

#include "main.h"
#include "console.h"
#include "cpu16.h"
#include "ui.h"

const uint32_t con_width  = 40;
const uint32_t con_height = 28;

struct app_state_t {

    SDL_Surface * screen_;
};

static
bool app_init(state_t * state, app_state_t * app, const char * path) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return false;

    state->running_ = false;
    state->console_ = con_new(con_width, con_height);
    state->cpu_ = cpu16_new();
    if (!state->cpu_)
        return false;

    memset(state->screen_.pixel_, 0x10, 320*240*4);

#if 0
    cpu16_device_t * serial = new cpu16_device_t;
    assert (serial);
    memset (serial, 0, sizeof (cpu16_device_t));
    serial->cycles_till_preempt_ = 0xffff;
    serial->write_byte_ = serial_write_8;
    cpu16_add_device(state->cpu_, serial, 32, 32);
#endif

    if (!cpu16_load_image(state->cpu_, path)) {
        printf("failed to load image\n");
        return false;
    }
    cpu16_reset(state->cpu_);

    uint32_t flags = 0;
#if 0
    flags |= SDL_FULLSCREEN;
#endif

    SDL_WM_SetCaption("CPU-16", nullptr);
    app->screen_ = SDL_SetVideoMode(640, 480, 32, flags);
    if (!app->screen_)
        return false;

    return true;
}

void app_free(state_t * state, app_state_t * app) {

    if (state->cpu_)
        cpu16_free(state->cpu_);
    state->cpu_ = nullptr;

    if (state->console_)
        con_free(state->console_);
    state->console_ = nullptr;

    if (app->screen_)
        SDL_FreeSurface(app->screen_);
    SDL_Quit();
}

static inline
void plot(uint32_t * pixels, int x, int y, uint32_t f, int fgnd, int bgnd, uint32_t pitch) {
    uint32_t c = (((~f) + 1) & fgnd) | ((f-1) & bgnd);
//    uint32_t c = (~f) + 1;
    x <<= 1;
    y <<= 1;
    pixels[x   + y * pitch        ] = c;
    pixels[x+1 + y * pitch        ] = c;
    pixels[x   + y * pitch + pitch] = c;
    pixels[x+1 + y * pitch + pitch] = c;
}

static
void draw_framebuffer(state_t * state) {

    const uint32_t ox = 160;
    const uint32_t oy = 0;

    const uint32_t width = state->screen_.pitch_;
    const uint32_t height = state->screen_.height_;
    uint32_t * py = (uint32_t*)state->screen_.pixel_;

    uint8_t * mem = cpu16_get_memory(state->cpu_);
    mem += 0x4000;
    py += ox + width * oy;
    for (uint32_t y = 0; y < 64; y++) {
        uint32_t * px = py;
        for (uint32_t x = 0; x < 64; ++x, ++px, ++mem) {
            uint8_t c = *mem;
            uint32_t rgb = (c << 16) | (c << 8) | c;
            *px = rgb;
        }
        py += width;
    }
}

static
void draw_state(state_t * state) {

    console_t * con = state->console_;
    cpu16_t   * cpu = state->cpu_;

    con_fill(con, nullptr, ' ');

    // draw ui elements to the console
    ui_draw(state);

    draw_framebuffer(state);

    // draw console to the screen
    con_render(con,
               state->screen_.pixel_,
               state->screen_.pitch_,
               state->screen_.height_);
}

static
void dbg_step_inst(state_t * state) {
    cpu16_run(state->cpu_, 1);
}

static
void dbg_step_over(state_t * state) {
    // insert breakpoint
    state->running_ = true;
}

static
void dbg_continue(state_t * state) {
    state->running_ = true;
}

static
void dbg_pause(state_t * state) {
    state->running_ = false;
}

static
void dbg_breakpoint(state_t * state) {
    // disassemble instruction
    // insert brk
    // if len == 4
    //   insert nop
}

static
bool app_tick(state_t * state, app_state_t * app) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return false;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE)
                return false;

            switch (event.key.keysym.sym) {
            case (SDLK_ESCAPE):
                return false;
            case (SDLK_F11) :
                dbg_step_inst(state);
                break;
            case (SDLK_F10) :
                dbg_step_over(state);
                break;
            case (SDLK_F9):
                dbg_breakpoint(state);
                break;
            case (SDLK_F6) :
                dbg_pause(state);
                break;
            case (SDLK_F5):
                dbg_continue(state);
                break;
            default:
                break;
            }
        }
    }

    draw_state(state);
    SDL_Flip(app->screen_);
    return true;
}

void blit_1x(state_t * state, app_state_t *app) {

    uint32_t * dst_y = (uint32_t*) app->screen_->pixels;
    uint32_t   dst_pitch = app->screen_->w;

    uint32_t * src_y = (uint32_t*) state->screen_.pixel_;
    uint32_t   src_pitch = state->screen_.pitch_;

    for (uint32_t y=0; y<240; ++y) {

        uint32_t * dst_x = dst_y;
        uint32_t * src_x = src_y;

        for (uint32_t x=0; x<320; ++x) {

            uint32_t rgb = *src_x;

            dst_x[0] = rgb;
            dst_x[1] = rgb;
            dst_x[dst_pitch+0] = rgb;
            dst_x[dst_pitch+1] = rgb;

            src_x += 1;
            dst_x += 2;
        }

        src_y += src_pitch;
        dst_y += dst_pitch * 2;
    }
}

int main(int argc, const char ** args) {

    if (argc != 2) {
        printf("%s <input.img>\n", args[0]);
        return -1;
    }

    state_t state;
    app_state_t app;

    if (!app_init(&state, &app, args[1])) {
        return false;
    }

    while (app_tick(&state, &app)) {

        if (state.running_) {
            // insert all breakpoints (unless pc is on breakpoint)
            cpu16_run(state.cpu_, 1024);
            // remove all breakpoints
        }
        else {
            SDL_Delay(4);
        }

        blit_1x(&state, &app);
    }

    app_free(&state, &app);
    return 0;
}
