#include <stdint.h>
#include <stdio.h>

#define _SDL_main_h
#include <SDL.h>

#include "console.h"
#include "cpu16.h"

struct state_t {

    console_t * console_;
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

    state->console_ = con_new(64, 32);

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
    state->screen_ = SDL_SetVideoMode(640, 480, 32, 0);
    if (!state->screen_)
        return false;

    return true;
}

static inline
void plot(uint32_t * pixels, int x, int y, int c, uint32_t pitch) {
    c |= 0x101010;
    x <<= 1;
    y <<= 1;
    pixels[x   + y * pitch        ] = c;
    pixels[x+1 + y * pitch        ] = c;
    pixels[x   + y * pitch + pitch] = c;
    pixels[x+1 + y * pitch + pitch] = c;
}

static
void draw_console(state_t * state) {

    extern uint8_t * __cdecl font_3x5_get(uint8_t ch);

    SDL_FillRect(state->screen_, nullptr, 0x101010);
    uint32_t * pixels = (uint32_t*)state->screen_->pixels;

    con_buffer_t buffer;
    con_get_buffer(state->console_, &buffer);

    uint8_t * ch = buffer.char_;

    const uint32_t pitch = state->screen_->w;

    for (uint32_t y = 0; y < buffer.height_; ++y) {
        for (uint32_t x = 0; x < buffer.width_; ++x, ++ch) {
            uint8_t * font = font_3x5_get(*ch);
            if (*ch == ' ')
                continue;
            uint32_t cx = x * 4 + 48;
            uint32_t cy = y * 6 + 24;
            for (uint32_t i = 0; i < 5; i++) {
                plot(pixels, cx + 0, cy + i, ~font[0] + 1, pitch);
                plot(pixels, cx + 1, cy + i, ~font[1] + 1, pitch);
                plot(pixels, cx + 2, cy + i, ~font[2] + 1, pitch);
                font += 3;
            }
        }
    }
}

void write_word(console_t * con, uint16_t w) {

    for (uint32_t i = 0; i < 4; ++i) {
        uint16_t n = (w >> 12) & 0xf;
        char h = '0' + n;
        if (n >= 10) h = 'a' + n - 10;
        con_putc(con, h);
        w <<= 4;
    }
}

void write_byte(console_t * con, uint8_t b) {

    uint8_t hi = b >> 4;
    if (hi >= 10) con_putc(con, 'a' + (hi - 10));
    else con_putc(con, '0' + hi);

    uint8_t lo = b & 0xf;
    if (lo >= 10) con_putc(con, 'a' + (lo - 10));
    else con_putc(con, '0' + lo);
}

static
void draw_registers(state_t * state) {

    console_t * con = state->console_;
    cpu16_t * cpu = state->cpu_;

    static
    const char * reg[] = {
        "ZR", "PC", "SP", "R3",
        "R4", "R5", "R6", "R7",
        "R8", "R9", "R10", "R11",
        "R12", "R13", "R14", "R15",
    };

    for (int i = 0; i < 16; ++i) {

        con_set_caret(con, 0, i);
        con_puts(con, reg[i], 3);
        con_set_caret(con, 5, -1);
        write_word(con, cpu16_get_register(state->cpu_, i));
    }
}

static
void draw_memory(state_t * state) {

    console_t * con = state->console_;
    cpu16_t * cpu = state->cpu_;

    uint16_t mem = 0;
    for (int y = 0; y < 8; y++) {
        con_set_caret(con, 0, 17 + y);
        for (int x = 0; x < 16; x++) {
            uint8_t byte = cpu16_read_byte(cpu, mem + x);
            write_byte(con, byte);
            con_putc(con, ' ');
        }
        for (int x = 0; x < 16; x++) {
            uint8_t byte = cpu16_read_byte(cpu, mem + x);
            con_putc(con, byte);
        }
        mem += 16;
    }
}

static
void draw_framebuffer(state_t * state) {

    const uint32_t width = state->screen_->w;
    const uint32_t height = state->screen_->w;

    uint32_t * py = (uint32_t*)state->screen_->pixels;

    uint8_t * mem = cpu16_get_memory(state->cpu_);
    mem += 0x4000;

    py += 16 + width * 16;

    for (uint32_t y = 0; y < 64; y++) {

        uint32_t * px = py;

        for (uint32_t x = 0; x < 64; x++) {

            uint8_t c = *mem;

            *px = (c<<16) | (c<<8) | c;

            px++;
            mem++;
        }

        py += width;
    }
}

static
void draw_state(state_t * state) {

    console_t * con = state->console_;
    cpu16_t * cpu = state->cpu_;

    con_fill(con, nullptr, ' ');

    draw_registers(state);
    draw_memory(state);
    draw_console(state);
    draw_framebuffer(state);
}

static
bool app_tick(state_t * state) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return false;
        if (event.type == SDL_KEYDOWN)
            if (event.key.keysym.sym == SDLK_ESCAPE)
                return false;
    }

#if 0
    for (uint32_t i = 0; i < (64 * 64); ++i) {
        uint32_t x = i % 64;
        uint32_t y = i / 64;
        uint8_t pix = cpu16_read_byte(state->cpu_, 0x4000 + i);
        SDL_Rect rect = { x*4, y*4, 4, 4 };
        SDL_FillRect(state->screen_, &rect, (pix << 16) | (pix << 8) | pix);
    }
#endif

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
        cpu16_run(state.cpu_, 4);
    }

    return 0;
}
