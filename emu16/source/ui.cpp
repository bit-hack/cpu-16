#define _SDL_main_h
#include <SDL.h>
#include <assert.h>

#include "ui.h"
#include "main.h"
#include "dasm.h"

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

void ui_draw_reg(state_t * state) {
    assert(state);

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

void ui_draw_mem(state_t * state) {
    assert(state);

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

void ui_draw_dis(state_t * state) {
    assert(state);

    console_t * con = state->console_;
    cpu16_t   * cpu = state->cpu_;
    uint8_t   * mem = cpu16_get_memory(cpu);
    uint16_t    adr = cpu16_get_register(cpu, 1);

    for (int i = 0; i < 16; ++i) {
        con_set_caret(con, 15, i);
        write_word(con, adr);
        con_set_caret(con, 21, i);

        cpu16_inst_t dis;
        if (!cpu16_dasm(mem + (adr&0xffff), &dis)) {

            con_putc(con, 'h');
            write_word(con, mem[adr]);
            adr += 2;
        }
        else {

            con_puts(con, (char*)dis.mnemonic_, 32);

            for (uint32_t j = 0; j < dis.operands_; j++) {
                con_set_caret(con, 26 + j * 6, i);
                uint8_t * op = dis.operand_[j];
                con_puts(con, (char*)op, 8);
            }
            adr += dis.size_;
        }
    }
}
