#define _SDL_main_h
#include <SDL.h>
#include <assert.h>

#include "ui.h"
#include "main.h"
#include "console.h"
#include "cpu16.h"
#include "dasm.h"

enum e_pane {
    e_disasm    = 0,
    e_memory    ,
    e_registers ,
    e_stack     ,
    e_screen    ,
};

struct pane_t {

    const char * name_;
    uint32_t x_, y_, w_, h_;
    void (*tick_)(state_t *, pane_t *);
};

extern void ui_draw_reg(state_t *, pane_t *);
extern void ui_draw_mem(state_t *, pane_t *);
extern void ui_draw_dis(state_t *, pane_t *);
extern void ui_draw_stk(state_t *, pane_t *);

static
pane_t pane_g[] = {
    { "registers",      0, 0, 0, 0, ui_draw_reg },
    { "memory",         0, 0, 0, 0, ui_draw_mem },
    { "disassembly",    0, 0, 0, 0, ui_draw_dis },
    { "stack",          0, 0, 0, 0, ui_draw_stk },
    { nullptr,          0, 0, 0, 0, nullptr }
};

static
void write_word(console_t * con, uint16_t w) {

    for (uint32_t i = 0; i < 4; ++i) {
        uint16_t n = (w >> 12) & 0xf;
        char h = '0' + n;
        if (n >= 10) h = 'a' + n - 10;
        con_putc(con, h);
        w <<= 4;
    }
}

static
void write_byte(console_t * con, uint8_t b) {

    uint8_t hi = b >> 4;
    if (hi >= 10) con_putc(con, 'a' + (hi - 10));
    else con_putc(con, '0' + hi);

    uint8_t lo = b & 0xf;
    if (lo >= 10) con_putc(con, 'a' + (lo - 10));
    else con_putc(con, '0' + lo);
}

void ui_draw_reg(state_t * state, pane_t * pane) {
    assert(state);

    uint32_t ox = 24;
    uint32_t oy = 0;

    console_t * con = state->console_;
    cpu16_t   * cpu = state->cpu_;

    static const char * reg[] = {
        "ZR", "PC", "SP", "R3",
        "R4", "R5", "R6", "R7",
        "R8", "R9", "R10", "R11",
        "R12", "R13", "R14", "R15",
    };

    con_set_caret(con, ox, oy);
    con_set_attr(con, 0x24);
    con_puts(con, "registers", 9);
    con_set_attr(con, 0x52);

    for (int i = 0; i < 16; ++i) {

        con_set_caret(con, ox, oy + i + 1);
        con_puts(con, reg[i], 3);
        con_set_caret(con, ox + 5, -1);
        write_word(con, cpu16_get_register(state->cpu_, i));
    }
}

void ui_draw_mem(state_t * state, pane_t * pane) {
    assert(state);

    const uint32_t ox     = 0;
    const uint32_t oy     = 18;
    const uint32_t width  = 8;
    const uint32_t height = 8;

    console_t * con = state->console_;
    cpu16_t   * cpu = state->cpu_;

    con_set_caret(con, ox, oy);
    con_set_attr(con, 0x24);
    con_puts(con, "memory", 6);
    con_set_attr(con, 0x52);

    uint16_t mem = 0;
    for (uint32_t y = 0; y < height; y++) {

        con_set_caret(con, ox, oy + y + 1);
        write_word(con, mem);
        con_move(con, 2, 0);

        for (uint32_t x = 0; x < width; x++) {
            uint8_t byte = cpu16_read_byte(cpu, mem + x);
            write_byte(con, byte);
            con_move(con, 1, 0);
        }
        con_move(con, 1, 0);
        for (uint32_t x = 0; x < width; x++) {
            uint8_t byte = cpu16_read_byte(cpu, mem + x);
            con_putc(con, byte);
        }
        mem += width;
    }
}

void ui_draw_dis(state_t * state, pane_t * pane) {
    assert(state);

    const uint32_t ox     = 0;
    const uint32_t oy     = 0;
    const uint32_t height = 16;

    console_t * con = state->console_;
    cpu16_t   * cpu = state->cpu_;
    uint8_t   * mem = cpu16_get_memory(cpu);
    uint16_t    adr = cpu16_get_register(cpu, 1);

    con_set_caret(con, ox, oy);
    con_set_attr( con, 0x24 );
    con_puts(con, "disassembly", 11);
    con_set_attr(con, 0x52);

    for (int i = 0; i < height; ++i) {

        con_set_caret(con, ox, oy+i+1);
        write_word(con, adr);
        con_set_caret(con, ox+6, oy+i+1);

        dasm_inst_t dis;
        if (!dasm_decode(mem + (adr&0xffff), &dis)) {

            con_putc(con, 'h');
            write_word(con, mem[adr]);
            adr += 2;
        }
        else {

            con_puts(con, (char*)dis.mnemonic_, 32);

            for (uint32_t j = 0; j < dis.operands_; j++) {
                con_set_caret(con, ox + 11 + j * 6, oy+i+1);
                uint8_t * op = dis.operand_[j];
                con_puts(con, (char*)op, 8);
            }
            adr += dis.size_;
        }
    }
}

void ui_draw_stk(state_t *, pane_t *) {
}

void ui_draw(state_t * state) {

    pane_t * pane = pane_g;
    for (;pane->name_; ++pane) {
        if (pane->tick_)
            pane->tick_(state, pane);
    }
}
