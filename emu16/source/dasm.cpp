#include <assert.h>
#include <string.h>

#include "dasm.h"

struct format_t {

    const char * mnemonic_;
    const char * format_;
};

uint8_t size[] = {
    4, 4, 4, 4, 
    4, 4, 4, 4,  
    0, 0, 0, 0,
    0, 0, 0, 0,

    2, 2, 2, 2, 
    2, 2, 2, 2,  
    2, 2, 2, 2, 
    0, 0, 0, 0,

    4, 4, 4, 4, 
    4, 4, 4, 4,  
    4, 4, 4, 4, 
    0, 0, 0, 0,

    2, 2, 0, 0, 
    0, 0, 0, 0,  
    0, 0, 0, 0, 
    0, 0, 0, 0,

    4, 4, 4, 4, 
    4, 4, 4, 4, 
    0, 0, 0, 0,  
    0, 0, 0, 0,

    2, 2, 0, 0,  
    0, 0, 0, 0, 
    0, 0, 0, 0,  
    0, 0, 0, 0,
};

format_t format[] = {

    { "LDW ", "IYX"    },   { "LDB ", "IYX"    },   { "LDW+", "IYX"    },   { "LDB+", "IYX"    },
    { "STW ", "YIX"    },   { "STB ", "YIX"    },   { "STW+", "YIX"    },   { "STB+", "YIX"    },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },

    { "ADD ", "YX"     },   { "SUB ", "YX"     },   { "MUL ", "YX"     },   { "MLH ", "YX"     },
    { "DIV ", "YX"     },   { "MOD ", "YX"     },   { "SHL ", "YX"     },   { "SHR ", "YX"     },
    { "AND ", "YX"     },   { "OR  ", "YX"     },   { "XOR ", "YX"     },   { "MOV ", "YX"     },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },

    { "ADD ", "IX"     },   { "SUB ", "IX"     },   { "MUL ", "IX"     },   { "MLH ", "IX"     },
    { "DIV ", "IX"     },   { "MOD ", "IX"     },   { "SHL ", "IX"     },   { "SHR ", "IX"     },
    { "AND ", "IX"     },   { "OR  ", "IX"     },   { "XOR ", "IX"     },   { "MOV ", "IX"     },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    
    { "PUSH", "X"      },   { "POP ", "X"      },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },

    { "JMP ", "I"      },   { "JNE ", "YXI"    },   { "JEQ ", "YXI"    },   { "JL  ", "YXI"    },
    { "JG  ", "YXI"    },   { "JLE ", "YXI"    },   { "JGE ", "YXI"    },   { "CALL", "I"      },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    
    { "RET ", ""       },   { "BRK ", ""       },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
};

uint32_t put_dec(uint8_t * out, uint32_t v) {

    uint8_t ary[16];
    uint32_t i = 0;

    if (v == 0) {
        memcpy(out, "0", 2);
        return 1;
    }
    while (v > 0) {
        uint8_t r = v % 10;
        ary[i++] = r;
        v /= 10;
    }
    for (uint32_t j = 0; j < i; ++j) {
        out[j] = '0' + ary[i-(j+1)];
    }

    out[i] = '\0';
    return i;
}

uint32_t put_hex(uint8_t * out, uint16_t w) {

    out[0] = 'h';
    ++out;
    for (uint32_t i = 0; i < 4; ++i) {
        uint16_t n = (w >> 12) & 0xf;
        char h = '0' + n;
        if (n >= 10) h = 'a' + n - 10;
        out[i] = h;
        w <<= 4;
    }
    out[4] = '\0';
    return 5;
}

void decode_imediate(uint8_t * out, uint16_t num) {
    put_hex(out, num);
}

void decode_reg(uint8_t * out, uint8_t num) {

    switch (num) {
    case (0) :
        memcpy(out, "ZR", 3);
        break;
    case (1) :
        memcpy(out, "PC", 3);
        break;
    case (2) :
        memcpy(out, "SP", 3);
        break;
    case (3) :
        memcpy(out, "CR", 3);
        break;
    default:
        out[0] = 'R';
        put_dec(out+1, num);
    }
}

uint32_t dasm_length(uint8_t * stream) {
    if (*stream >= 0x60)
        return 0;
    return size[*stream];
}

bool dasm_decode(uint8_t * stream, dasm_inst_t * out) {

    uint8_t op = stream[0];

    if (op >= 0x60)
        return false;
    const format_t & fmt = format[op];
    if (!fmt.mnemonic_)
        return false;
      
    // output mnemonic
    out->mnemonic_ = fmt.mnemonic_;
    out->operands_ = 0;
    out->size_ = 2;
    uint8_t rx = stream[1] & 0xf;
    uint8_t ry = stream[1] >> 4;

    // special case
    if (op == 0x2a && stream[1] == 0) {
        out->mnemonic_ = "nop ";
        return true;
    }

    // output operands
    for (int i = 0; fmt.format_[i]; ++i, ++out->operands_) {
        assert(i < 3);

        switch (fmt.format_[i]) {
        case ('X') :
            decode_reg(out->operand_[i], rx);
            break;

        case ('Y') :
            decode_reg(out->operand_[i], ry);
            break;

        case ('I') : {
            uint16_t imm = (stream[3] << 8) | stream[2];
            decode_imediate(out->operand_[i], imm);
            out->size_ = 4;
            }
            break;
        }        
    }
    return true;
}
