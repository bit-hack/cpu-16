#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define _SDL_main_h
#include <SDL.h>

#include "main.h"
#include "ui.h"

const uint32_t con_width  = 40;
const uint32_t con_height = 28;

static
void serial_write_8(cpu16_t *, uint16_t addr, uint8_t val, cpu16_device_t * user) {

    putchar(val);
}

static
bool app_init(state_t * state, const char * path) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return false;

    state->running_ = false;
    state->console_ = con_new(con_width, con_height);
    state->cpu_ = cpu16_new();
    if (!state->cpu_)
        return false;

    cpu16_device_t * serial = new cpu16_device_t;
    assert (serial);
    memset (serial, 0, sizeof (cpu16_device_t));
    serial->cycles_till_preempt_ = 0xffff;
    serial->write_byte_ = serial_write_8;
    cpu16_add_device(state->cpu_, serial, 32, 32);

    if (!cpu16_load_image(state->cpu_, path)) {
        printf("failed to load image\n");
        return false;
    }
    cpu16_reset(state->cpu_);

    uint32_t flags = 0;
    flags |= SDL_FULLSCREEN;

    SDL_WM_SetCaption("CPU-16", nullptr);
    state->screen_ = SDL_SetVideoMode(640, 480, 32, flags);
    if (!state->screen_)
        return false;

    return true;
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

#define RGB(r,g,b) ((r<<16) | (g<<8) | b)

uint32_t palette[16] = {
    0x101010,           // dark grey
    0xffffff,           // white
    RGB( 29,  39,  47), // bgnd blue
    RGB(  0, 185, 160), // title green
    RGB(236, 147,   0), // title yellow
    RGB(166, 180, 171), // code text
    RGB(255,  15,  25), // code red
    RGB( 96,  87,  62), // code dull wheat
};

static
void draw_console(state_t * state) {

    const uint32_t ox = 8;
    const uint32_t oy = 8;

    extern uint8_t * __cdecl font_3x5_get(uint8_t ch);

    SDL_FillRect(state->screen_, nullptr, 0x101010);
    uint32_t * pixels = (uint32_t*)state->screen_->pixels;

    con_buffer_t buffer;
    con_get_buffer(state->console_, &buffer);

    uint8_t * ch = buffer.char_;
    uint8_t * at = buffer.attr_;

    const uint32_t pitch = state->screen_->w;

    for (uint32_t y = 0; y < buffer.height_; ++y) {
        for (uint32_t x = 0; x < buffer.width_; ++x, ++ch, ++at) {
            uint8_t * font = font_3x5_get(*ch);
#if 0
            if (*ch == ' ')
                continue;
#endif

            uint32_t bgnd = palette[(*at) & 0xf];
            uint32_t fgnd = palette[(*at) >> 4];

            uint32_t cx = x * 4 + ox;
            uint32_t cy = y * 6 + oy;
            for (uint32_t i = 0; i < 5; i++) {
                plot(pixels, cx + 0, cy + i, font[0], fgnd, bgnd, pitch);
                plot(pixels, cx + 1, cy + i, font[1], fgnd, bgnd, pitch);
                plot(pixels, cx + 2, cy + i, font[2], fgnd, bgnd, pitch);
                plot(pixels, cx + 3, cy + i,       0, fgnd, bgnd, pitch);
                font += 3;
            }
            plot(pixels, cx + 0, cy + 5, 0, fgnd, bgnd, pitch);
            plot(pixels, cx + 1, cy + 5, 0, fgnd, bgnd, pitch);
            plot(pixels, cx + 2, cy + 5, 0, fgnd, bgnd, pitch);
            plot(pixels, cx + 3, cy + 5, 0, fgnd, bgnd, pitch);
        }
    }
}

static
void draw_framebuffer(state_t * state) {

    const uint32_t ox = 320 + 96 + 16;
    const uint32_t oy = 16;

    const uint32_t width = state->screen_->w;
    const uint32_t height = state->screen_->w;
    uint32_t * py = (uint32_t*)state->screen_->pixels;
    uint8_t * mem = cpu16_get_memory(state->cpu_);
    mem += 0x4000;
    py += ox + width * oy;
    for (uint32_t y = 0; y < 64; y++) {
        uint32_t * px = py;
        for (uint32_t x = 0; x < 64; ++x, px+=3, ++mem) {
            uint8_t c = *mem;
            uint32_t rgb = (c << 16) | (c << 8) | c;

            const int r0 = 0;
            const int r1 = width;
            const int r2 = width + width;

            px[r0 + 0] = rgb;
            px[r0 + 1] = rgb;
            px[r0 + 2] = rgb;
            px[r1 + 0] = rgb;
            px[r1 + 1] = rgb;
            px[r1 + 2] = rgb;
            px[r2 + 0] = rgb;
            px[r2 + 1] = rgb;
            px[r2 + 2] = rgb;
        }
        py += width * 3;
    }
}

static
void draw_state(state_t * state) {

    console_t * con = state->console_;
    cpu16_t * cpu = state->cpu_;

    con_fill(con, nullptr, ' ');

    ui_draw_reg(state);
    ui_draw_mem(state);
    ui_draw_dis(state);

    draw_console(state);
    draw_framebuffer(state);
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
bool app_tick(state_t * state) {

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
            }
        }
    }

    draw_state(state);
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

        if (state.running_) {
            // insert all breakpoints (unless pc is on breakpoint)
            cpu16_run(state.cpu_, 1024);
            // remove all breakpoints
        }
        else {
            SDL_Delay(4);
        }
    }

    return 0;
}
