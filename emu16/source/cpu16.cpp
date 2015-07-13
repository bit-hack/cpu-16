#include "cpu16.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum {
    IR_DIV_ZERO = 0,        // divide by zero
    IR_VSYNC    ,           // vertical sync
    IR_HSYNC    ,           // horizontal sync
    IR_KEYBOARD ,           // keyboard controller
    IR_BADINST  ,           // bad instruction
};

enum {
    VEC_RESET   = 0xfffc,   // reset vector address
};

enum {
    REG_ZR = 0,
    REG_PC = 1,
    REG_SP = 2,
};

struct cpu16_t {

    uint16_t reg_[16];

    // +2 to make reading imm easier
    uint8_t  mem_[0x10002];

    struct {
        bool interrupt_;
    }
    flags_;
};

static inline
void write8(cpu16_t & cpu, uint16_t addr, uint16_t val) {
    // serial out port
    if (addr == 0x8000)
        putchar(val & 0xff);
    cpu.mem_[addr] = val & 0xff;
}

static inline
void write16(cpu16_t & cpu, uint16_t addr, uint16_t val) {
    cpu.mem_[addr+0] = val & 0xff;
    cpu.mem_[addr+1] = val >> 8;
}

static inline
uint16_t read8(cpu16_t & cpu, uint16_t addr) {
    return cpu.mem_[addr];
}

static inline
uint16_t read16(cpu16_t & cpu, uint16_t addr) {
    return cpu.mem_[addr] | (cpu.mem_[addr+1] << 8);
}

static inline
void push16(cpu16_t & cpu, uint16_t val) {
    write16(cpu, cpu.reg_[REG_SP], val);
    cpu.reg_[REG_SP] -= 2;
}

static inline
uint16_t pop16(cpu16_t & cpu) {
    cpu.reg_[REG_SP] += 2;
    return read16(cpu, cpu.reg_[REG_SP]);
}

static inline
void raise(cpu16_t & cpu, uint16_t ir) {
    //todo: raise an interrupt
}

static inline
void alu_add(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    reg = uint16_t(int16_t(reg) + int16_t(operand));
}

static inline
void alu_mul(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    reg = uint16_t(int16_t(reg) * int16_t(operand));
}

static inline
void alu_shl(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    reg = int16_t(reg) << operand;
}

static inline
void alu_shr(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    reg = int16_t(reg) >> operand;
}

static inline
void alu_sub(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    reg = uint16_t(int16_t(reg) - int16_t(operand));
}

static inline
void alu_div(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    if (operand == 0) {
        raise (cpu, IR_DIV_ZERO);
    }
    else {
        reg = uint16_t(int16_t (reg) / int16_t (operand));
    }
}

static inline
void alu_mod(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    if (operand == 0) {
        raise (cpu, IR_DIV_ZERO);
    }
    else {
        reg = uint16_t(int16_t (reg) % int16_t (operand));
    }
}

static inline
void alu_and(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    reg &= operand;
}

static inline
void alu_or(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    reg &= operand;
}
static inline
void alu_xor(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    reg ^= operand;
}

static inline
void alu_mov(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    reg = operand;
}

static inline
void alu_mulh(cpu16_t & cpu, uint16_t & reg, uint16_t operand) {
    reg = uint16_t((int32_t(int16_t(reg)) * int32_t(int16_t(operand))) >> 16);
}

extern
void cpu16_run(cpu16_t * cpu_, int32_t count) {

    cpu16_t & cpu = *cpu_;

    uint16_t & zr = cpu.reg_[REG_ZR];
    uint16_t & pc = cpu.reg_[REG_PC];
    uint16_t & sp = cpu.reg_[REG_SP];

    while (--count > 0) {

        zr = 0; // clear zero register

        uint8_t    op  = cpu.mem_[pc+0];
        uint8_t    rx_ = cpu.mem_[pc+1] & 0x0f;
        uint8_t    ry_ = cpu.mem_[pc+1] >> 4;
        uint16_t & rx  = cpu.reg_[rx_];
        uint16_t & ry  = cpu.reg_[ry_];

        uint16_t imm = read16(cpu, pc+2);

        switch (op) {

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x00) : // LDW  RY RX (load word)
            rx = read16 (cpu, ry);
            pc += 2;
            break;

        case (0x01) : // LDB  RY RX (load byte)
            rx = read8 (cpu, ry);
            pc += 2;
            break;

        case (0x02) : // LDW+ RY RX (load word, increment)
            rx = read16(cpu, ry);
            ry += 2;
            pc += 2;
            break;

        case (0x03) : // LDB+ RY RX (load byte, increment)
            rx = read8 (cpu, ry);
            ry += 1;
            pc += 2;
            break;

        case (0x04) : // STW  RY RX (store word)
            write16 (cpu, rx, ry);
            pc += 2;
            break;

        case (0x05) : // STB  RY RX (store byte)
            write8 (cpu, rx, ry);
            pc += 2;
            break;

        case (0x06) : // STW+ RY RX (store word, increment)
            write16 (cpu, rx, ry);
            rx += 2;
            pc += 2;
            break;

        case (0x07) : // STB+ RY RX (store byte, increment)
            write8 (cpu, rx, ry);
            rx += 1;
            pc += 2;
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x10) : // LDW IMM RX (load word)
            rx = read16(cpu, imm);
            pc += 4;
            break;

        case (0x11) : // LDB IMM RX (load byte)
            rx = read8(cpu, imm);
            pc += 4;
            break;

        case (0x14) : // STW RX IMM (store word)
            write16(cpu, imm, rx);
            pc += 4;
            break;

        case (0x15) : // STB RX IMM (store byte)
            write8(cpu, imm, rx);
            pc += 4;
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x20) : // ADD RY RX
            alu_add (cpu, rx, ry);
            pc += 2;
            break;

        case (0x21) : // MUL RY RX (multiply low)
            alu_mul (cpu, rx, ry);
            pc += 2;
            break;

        case (0x22) : // SHL RY RX (shift left arithmetic)
            alu_shl (cpu, rx, ry);
            pc += 2;
            break;

        case (0x23) : // SHR RY RX (shift right arithmetic)
            alu_shr (cpu, rx, ry);
            pc += 2;
            break;

        case (0x24) : // SUB RY RX
            alu_sub (cpu, rx, ry);
            pc += 2;
            break;

        case (0x25) : // DIV RY RX
            alu_div (cpu, rx, ry);
            pc += 2;
            break;

        case (0x26) : // MOD RY RX (modulo)
            alu_mod (cpu, rx, ry);
            pc += 2;
            break;

        case (0x27) : // AND RY RX
            alu_and (cpu, rx, ry);
            pc += 2;
            break;

        case (0x28) : // OR  RY RX
            alu_or (cpu, rx, ry);
            pc += 2;
            break;

        case (0x29) : // XOR RY RX
            alu_xor (cpu, rx, ry);
            pc += 2;
            break;

        case (0x2A) : // MOV RY RX (move)
            alu_mov (cpu, rx, ry);
            pc += 2;
            break;

        case (0x2B) : // MULH RY RX (multiply high)
            alu_mulh (cpu, rx, ry);
            pc += 2;
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x30) : // ADD IMM RX
            alu_add (cpu, rx, imm);
            pc += 4;
            break;

        case (0x31) : // MUL IMM RX (multiply low)
            alu_mul (cpu, rx, imm);
            pc += 4;
            break;

        case (0x32) : // SHL IMM RX (shift left arithmetic)
            alu_shl (cpu, rx, imm);
            pc += 4;
            break;

        case (0x33) : // SHR IMM RX (shift right arithmetic)
            alu_shr (cpu, rx, imm);
            pc += 4;
            break;

        case (0x34) : // SUB IMM RX
            alu_sub (cpu, rx, imm);
            pc += 4;
            break;

        case (0x35) : // DIV IMM RX
            alu_div (cpu, rx, imm);
            pc += 4;
            break;

        case (0x36) : // MOD IMM RX (modulo)
            alu_mod (cpu, rx, imm);
            pc += 4;
            break;

        case (0x37) : // AND IMM RX
            alu_and (cpu, rx, imm);
            pc += 4;
            break;

        case (0x38) : // OR  IMM RX
            alu_or (cpu, rx, imm);
            pc += 4;
            break;

        case (0x39) : // XOR IMM RX
            alu_xor (cpu, rx, imm);
            pc += 4;
            break;

        case (0x3A) : // MOV IMM RX (move)
            alu_mov (cpu, rx, imm);
            pc += 4;
            break;

        case (0x3B) : // MULH IMM RX (multiply high)
            alu_mulh (cpu, rx, imm);
            pc += 4;
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x40) : // PUSH RX
            push16 (cpu, rx);
            pc += 2;
            break;

        case (0x41) : // POP  RX
            rx=pop16(cpu);
            pc += 2;
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x50) : // JMP RY RX IMM
            pc = imm; 
            break; 

        case (0x51) : // JNE RY RX IMM
            if (ry!=rx) { pc = imm; } 
            else        { pc+=4; }
            break; 

        case (0x52) : // JEQ RY RX IMM
            if (ry==rx) { pc = imm; } 
            else        { pc+=4; } 
            break; 

        case (0x53) : // JL  RY RX IMM
            if (ry< rx) { pc = imm; } 
            else        { pc+=4; } 
            break;

        case (0x54) : // JG  RY RX IMM
            if (ry> rx) { pc = imm; } 
            else { pc+=4; } 
            break;

        case (0x55) : // JLE RY RX IMM
            if (ry<=rx) { pc = imm; } 
            else        { pc += 4; } 
            break;

        case (0x56) : // JGE RY RX IMM
            if (ry>=rx) { pc = imm; } 
            else        { pc += 4; } 
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

#if 0
        case (0x60) : // JRO RX
            // NOT NEEDED SINCE WE CAN 'add #8 pc'
            break;
#endif

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x70) : // CALL IMM
            push16(cpu, pc + 4);
            pc = imm;
            break;

        case (0x71) : // INT IMM
            return;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x80) : // STI (enable interrupts)
            cpu.flags_.interrupt_ = true; 
            pc += 2;
            break;

        case (0x81) : // CLI (disable interrupts)
            cpu.flags_.interrupt_ = false;
            pc += 2;

        case (0x82) : // RETI (return from interrupts)
            cpu.flags_.interrupt_ = true;
            pc = pop16 (cpu);
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        default:
            // unknown opcode
            raise (cpu, IR_BADINST);
            return;
        }
    } // while
}

extern
bool cpu16_load_image(cpu16_t * cpu_, const char * path) {
    cpu16_t & cpu = *cpu_;
    FILE * fp = nullptr;
    fopen_s(&fp, path, "rb");
    if (!fp)
        return false;
    if (fread_s(cpu.mem_, 0x10000, 1, 0x10000, fp) != 0x10000)
        return false;
    fclose(fp);
    return true;
}

extern
cpu16_t * cpu16_new () {
    cpu16_t * cpu = new cpu16_t ();
    memset (cpu, 0, sizeof (cpu16_t));
    return cpu;
}

extern
void cpu16_free (cpu16_t * arg) {
    delete arg;
}

extern
void cpu16_reset (cpu16_t * cpu) {
    memset (cpu->reg_, 0, sizeof(cpu->reg_));
    cpu->reg_[REG_PC] = VEC_RESET;
}
