#include <assert.h>
#include <string.h>

#include "dasm.h"

/*
| 0xCC   | 0x0_          | 0x1_        | 0x2_      | 0x3_       | 0x4_    | 0x5_          | 0x6_     | 0x7_   |
| ------ | ------------- | ----------- | --------- | ---------- | ------- | ------------- | -------- | ------ |
| 0x_0   | LDW   RY  RX  | LDW IMM RX  | ADD RY RX | ADD IMM RX | PUSH RX | JMP       IMM | CALL IMM | RETI   |
| 0x_1   | LDB   RY  RX  | LDB IMM RX  | MUL RY RX | MUL IMM RX | POP  RX | JNE RY RX IMM | INT  IMM | CLI    |
| 0x_2   | LDW+  RY  RX  |             | SHL RY RX | SHL IMM RX |         | JEQ RY RX IMM |          | STI    |
| 0x_3   | LDB+  RY  RX  |             | SHR RY RX | SHR IMM RX |         | JL  RY RX IMM |          | RET    |
| 0x_4   | STW   RY  RX  | STW RX IMM  | SUB RY RX | SUB IMM RX |         | JG  RY RX IMM |          | BRK    |
| 0x_5   | STB   RY  RX  | STB RX IMM  | DIV RY RX | DIV IMM RX |         | JLE RY RX IMM |          |        |
| 0x_6   | STW+  RY  RX  |             | MOD RY RX | MOD IMM RX |         | JGE RY RX IMM |          |        |
| 0x_7   | STB+  RY  RX  |             | AND RY RX | AND IMM RX |         |               |          |        |
| 0x_8   |               |             | OR  RY RX | OR  IMM RX |         |               |          |        |
| 0x_9   |               |             | XOR RY RX | XOR IMM RX |         |               |          |        |
| 0x_A   |               |             | MOV RY RX | MOV IMM RX |         |               |          |        |
| 0x_B   |               |             | MLH RY RX | MLH IMM RX |         |               |          |        |
*/

struct format_t {

    const char * mnemonic_;
    const char * format_;
};

format_t format[] = {

    { "LDW ", "YX"     },   { "LDB ", "YX"     },   { "LDW+", "YX"     },   { "LDB+", "YX"     },
    { "STW ", "YX"     },   { "STB ", "YX"     },   { "STW+", "YX"     },   { "STB+", "YX"     },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },

    { "LDW ", "IX"     },   { "LDB ", "IX"     },   { nullptr, nullptr },   { nullptr, nullptr },
    { "STW ", "XI"     },   { "STB ", "XI"     },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },

    { "ADD ", "YX"     },   { "MUL ", "YX"     },   { "SHL ", "YX"     },   { "SHR ", "YX"     },
    { "SUB ", "YX"     },   { "DIV ", "YX"     },   { "MOD ", "YX"     },   { "AND ", "YX"     },
    { "OR  ", "YX"     },   { "XOR ", "YX"     },   { "MOV ", "YX"     },   { "MLH ", "YX"     },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    
    { "ADD ", "IX"     },   { "MUL ", "IX"     },   { "SHL ", "IX"     },   { "SHR ", "IX"     },
    { "SUB ", "IX"     },   { "DIV ", "IX"     },   { "MOD ", "IX"     },   { "AND ", "IX"     },
    { "OR  ", "IX"     },   { "XOR ", "IX"     },   { "MOV ", "IX"     },   { "MLH ", "IX"     },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    
    { "PUSH", "X"      },   { "POP ", "X"      },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },

    { "JMP ", "I"      },   { "JNE ", "YXI"    },   { "JEQ ", "YXI"    },   { "JL  ", "YXI"    },
    { "JG  ", "YXI"    },   { "JLE ", "YXI"    },   { "JGE ", "YXI"    },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    
    { "CALL", "I"      },   { "INT ", "I"      },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
    
    { "RETI", ""       },   { "CLI ", ""       },   { "STI ", ""       },   { "RET ", ""       },
    { "BRK ", ""       },   { nullptr, nullptr },   { nullptr, nullptr },   { nullptr, nullptr },
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
    default:
        out[0] = 'R';
        put_dec(out+1, num);
    }
}

bool cpu16_dasm(uint8_t * stream, cpu16_inst_t * out) {

    uint8_t op = stream[0];

    if (op >= 0x80)
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
