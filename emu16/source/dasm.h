#pragma once
#include <stdint.h>

struct cpu16_inst_t {

    uint32_t     size_;
    const char * mnemonic_;
    uint32_t     operands_;
    uint8_t      operand_[3][8];
};

extern
bool cpu16_dasm(uint8_t * stream, cpu16_inst_t * out);
