#pragma once
#include <stdint.h>

struct dasm_inst_t {

    uint32_t     size_;
    const char * mnemonic_;
    uint32_t     operands_;
    uint8_t      operand_[3][8];
};

extern
uint32_t dasm_length(uint8_t * stream);

extern
bool dasm_decode(uint8_t * stream, dasm_inst_t * out);
