#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum {
    REG_ZR = 0,
    REG_PC = 1,
    REG_SP = 2,
    REG_OF = 3,
};

struct cpu16_t {

    uint16_t reg_[16];

    // +2 to make reading imm easier
    uint8_t  mem_[0x10002];

    uint32_t cycles_;

    struct {
        bool interrupt_;
    }
    flags_;
};

void write8(cpu16_t & cpu, uint16_t addr, uint16_t val) {

    // serial out port
    if (addr == 0x8000)
        putchar(val & 0xff);

    cpu.mem_[addr] = val & 0xff;
}

void write16(cpu16_t & cpu, uint16_t addr, uint16_t val) {
    cpu.mem_[addr+0] = val & 0xff;
    cpu.mem_[addr+1] = val >> 8;
}

uint16_t read8(cpu16_t & cpu, uint16_t addr) {
    return cpu.mem_[addr];
}

uint16_t read16(cpu16_t & cpu, uint16_t addr) {
    return cpu.mem_[addr] | (cpu.mem_[addr+1] << 8);
}

inline
void push16(cpu16_t & cpu, uint16_t val) {
    write16(cpu, cpu.reg_[REG_SP], val);
    cpu.reg_[REG_SP] -= 2;
}

inline
uint16_t pop16(cpu16_t & cpu) {
    cpu.reg_[REG_SP] += 2;
    return read16(cpu, cpu.reg_[REG_SP]);
}

void emu(cpu16_t & cpu, int32_t cycles) {

#define DO( X, LEN ) { X; pc += LEN; } break;

    uint16_t & zr = cpu.reg_[REG_ZR];
    uint16_t & pc = cpu.reg_[REG_PC];
    uint16_t & sp = cpu.reg_[REG_SP];

    while (--cycles > 0) {

        cpu.cycles_ += 1;

        zr = 0; // clear zero register

        uint8_t    op  = cpu.mem_[pc+0];
        uint8_t    rx_ = cpu.mem_[pc+1] & 0x0f;
        uint8_t    ry_ = cpu.mem_[pc+1] >> 4;
        uint16_t & rx  = cpu.reg_[rx_];
        uint16_t & ry  = cpu.reg_[ry_];

        uint16_t imm = read16(cpu, pc+2);

        switch (op) {
        case (0x00) : DO(rx=read16(cpu, ry), 2);            // LDW  RY RX
        case (0x01) : DO(rx=read8 (cpu, ry), 2);            // LDB  RY RX
        case (0x02) : DO(rx=read16(cpu, ry); ry+=2, 2);     // LDW+ RY RX
        case (0x03) : DO(rx=read8 (cpu, ry); ry+=1, 2);     // LDB+ RY RX
        case (0x04) : DO(write16(cpu, rx, ry), 2);          // STW  RY RX
        case (0x05) : DO(write8 (cpu, rx, ry), 2);          // STB  RY RX
        case (0x06) : DO(write16(cpu, rx, ry); rx+=2, 2);   // STW+ RY RX
        case (0x07) : DO(write8 (cpu, rx, ry); rx+=1, 2);   // STB+ RY RX

        case (0x10) : // LDW IMM RX
            rx = read16(cpu, imm);
            pc += 4;
            break;

        case (0x11) : // LDB IMM RX
            rx = read8(cpu, imm);
            pc += 4;
            break;

        case (0x14) : // STW RX IMM
            write16(cpu, imm, rx);
            pc += 4;
            break;

        case (0x15) : // STB RX IMM
            write8(cpu, imm, rx);
            pc += 4;
            break;

        case (0x20) : DO(rx  += ry, 2); // ADD RY RX
        case (0x21) : DO(rx  *= ry, 2); // MUL RY RX
        case (0x22) : DO(rx <<= ry, 2); // SHL RY RX
        case (0x23) : DO(rx >>= ry, 2); // SHR RY RX
        case (0x24) : DO(rx  -= ry, 2); // SUB RY RX
        case (0x25) : DO(rx  /= ry, 2); // DIV RY RX
        case (0x26) : DO(rx  %= ry, 2); // MOD RY RX
        case (0x27) : DO(rx  &= ry, 2); // AND RY RX
        case (0x28) : DO(rx  |= ry, 2); // OR  RY RX
        case (0x29) : DO(rx  ^= ry, 2); // XOR RY RX
        case (0x2A) : DO(rx   = ry, 2); // MOV RY RX

        case (0x30) : DO(rx  += imm, 4); // ADD IMM RX
        case (0x31) : DO(rx  *= imm, 4); // MUL IMM RX
        case (0x32) : DO(rx <<= imm, 4); // SHL IMM RX
        case (0x33) : DO(rx >>= imm, 4); // SHR IMM RX
        case (0x34) : DO(rx  -= imm, 4); // SUB IMM RX
        case (0x35) : DO(rx  /= imm, 4); // DIV IMM RX
        case (0x36) : DO(rx  %= imm, 4); // MOD IMM RX
        case (0x37) : DO(rx  &= imm, 4); // AND IMM RX
        case (0x38) : DO(rx  |= imm, 4); // OR  IMM RX
        case (0x39) : DO(rx  ^= imm, 4); // XOR IMM RX
        case (0x3A) : DO(rx   = imm, 4); // MOV IMM RX

        case (0x40) : DO(push16(cpu, rx), 2); // PUSH RX
        case (0x41) : DO(rx=pop16(cpu), 2); // POP  RX

        case (0x50) : pc = imm; break; // JMP RY RX IMM

        case (0x51) : if (ry!=rx) pc = imm; else pc+=4; break; // JNE RY RX IMM
        case (0x52) : if (ry==rx) pc = imm; else pc+=4; break; // JEQ RY RX IMM
        case (0x53) : if (ry< rx) pc = imm; else pc+=4; break; // JL  RY RX IMM
        case (0x54) : if (ry> rx) pc = imm; else pc+=4; break; // JG  RY RX IMM
        case (0x55) : if (ry<=rx) pc = imm; else pc+=4; break; // JLE RY RX IMM
        case (0x56) : if (ry>=rx) pc = imm; else pc+=4; break; // JGE RY RX IMM

        case (0x60) : // JRO RX
            pc += cpu.reg_[rx];
            break;

        case (0x70) : // CALL IMM
            push16(cpu, pc);
            pc = imm;
            break;

        case (0x71) : // INT IMM
            return;

        case (0x80) : DO(cpu.flags_.interrupt_ = true, 2); // STI
        case (0x81) : DO(cpu.flags_.interrupt_ = false, 2); // CLI     
        case (0x82) : // RETI
            cpu.flags_.interrupt_ = true;
            break;

        default:
            // unknown opcode
            return;
        }
    } // while
}

bool load_image(cpu16_t & cpu, const char * path) {

    FILE * fp = nullptr;
    fopen_s(&fp, path, "rb");
    if (!fp)
        return nullptr;

    if (fread_s(cpu.mem_, 0x10000, 1, 0x10000, fp) != 0x10000)
        return false;

    fclose(fp);
    return true;
}

int main(int argc, const char ** args) {

    if (argc != 2) {
        printf("%s <input.img>\n", args[0]);
        return -1;
    }
    
    cpu16_t cpu;
    memset(cpu.reg_, 0, sizeof(cpu.reg_));

    if (!load_image(cpu, args[1])) {
        printf("failed to load image\n");
        return -2;
    }

    // read the reset vector
    cpu.reg_[REG_PC] = read16(cpu, 0xfffe);

    emu(cpu, 1024);

    return 0;
}
