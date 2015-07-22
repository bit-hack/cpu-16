#include "cpu16.h"

#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <vector>

enum {
    IR_DIV_ZERO = 0,        // divide by zero
    IR_BADINST  ,           // bad instruction
};

enum {
    VEC_RESET   = 0xfffc,   // reset vector address
    VEC_IR      = 0xffe0,   // interupt vector
};

enum {
    REG_ZR = 0,
    REG_PC = 1,
    REG_SP = 2,
    REG_CR = 3,
};

enum {
    CR_INTERRUPT = 1
};

struct cpu16_t {

    uint16_t reg_[16];

    // +2 to make reading imm easier
    uint8_t  mem_[0x10002];

    // the bus is split into 1k chunks
    cpu16_bus_map_t * bus_[64];

    // device list
    std::vector<cpu16_device_t*> device_;

    // yield to main loop
    bool yield_;
};

static inline
void write8(cpu16_t & cpu, uint16_t addr, uint16_t val) {

    uint16_t page = (addr >> 10) & 0x3f;

    if (cpu.bus_[page]) {
        cpu16_bus_map_t & bus = *cpu.bus_[page];
        if (bus.write_byte_) {
            bus.write_byte_ (&cpu, addr, uint8_t(val), &bus);
            return;
        }
    }
    cpu.mem_[addr] = val & 0xff;
}

static inline
void write16(cpu16_t & cpu, uint16_t addr, uint16_t val) {

    uint16_t page = (addr >> 10) & 0x3f;
    
    if (cpu.bus_[page]) {
        cpu16_bus_map_t& bus = *cpu.bus_[page];
        if (bus.write_word_) {
            bus.write_word_ (&cpu, addr, val, &bus);
            return;
        }
    }
    cpu.mem_[addr+0] = val & 0xff;
    cpu.mem_[addr+1] = val >> 8;
}

static inline
uint16_t read8(cpu16_t & cpu, uint16_t addr) {

    uint16_t page = (addr >> 10) & 0x3f;

    if (cpu.bus_[page]) {
        cpu16_bus_map_t & bus = *cpu.bus_[page];
        if (bus.read_byte_) {
            return bus.read_byte_ (&cpu, addr, &bus);
        }
    }
    return cpu.mem_[addr];
}

static inline
uint16_t read16(cpu16_t & cpu, uint16_t addr) {

    uint16_t page = (addr >> 10) & 0x3f;

    if (cpu.bus_[page]) {
        cpu16_bus_map_t & bus = *cpu.bus_[page];
        if (bus.read_word_) {
            return bus.read_word_ (&cpu, addr, &bus);
        }
    }
    return cpu.mem_[addr] | (cpu.mem_[addr+1] << 8);
}

static inline
void push16(cpu16_t & cpu, uint16_t val) {
    cpu.reg_[REG_SP] -= 2;
    write16(cpu, cpu.reg_[REG_SP], val);
}

static inline
uint16_t pop16(cpu16_t & cpu) {
    uint16_t v = read16(cpu, cpu.reg_[REG_SP]);
    cpu.reg_[REG_SP] += 2;
    return v;
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
    reg |= operand;
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

static
int cpu16_interp(cpu16_t * cpu_, uint32_t count) {
    assert (cpu_);
    assert (count > 0);

    cpu16_t & cpu = *cpu_;

    uint16_t & zr = cpu.reg_[REG_ZR];
    uint16_t & pc = cpu.reg_[REG_PC];
    uint16_t & sp = cpu.reg_[REG_SP];

    // number of instructions retired
    uint32_t retired = 0;

    for (; retired < count; ++retired)
    {
        zr = 0; // clear zero register

        uint8_t    op  = cpu.mem_[pc+0];
        uint8_t    rx_ = cpu.mem_[pc+1] & 0x0f;
        uint8_t    ry_ = cpu.mem_[pc+1] >> 4;
        uint16_t & rx  = cpu.reg_[rx_];
        uint16_t & ry  = cpu.reg_[ry_];

        uint16_t imm = read16(cpu, pc+2);

        switch (op) {

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x00) : // LDW IMM RY RX (load word)
            rx = read16(cpu, (imm + ry) & 0xffff);
            pc += 4;
            break;

        case (0x01) : // LDB IMM RY RX (load byte)
            rx = read8(cpu, (imm + ry) & 0xffff);
            pc += 4;
            break;

        case (0x02) : // LDW+ IMM RY RX (load word, increment)
            rx = read16(cpu, (imm + ry) & 0xffff);
            ry += 2;
            pc += 4;
            break;

        case (0x03) : // LDB+ IMM RY RX (load byte, increment)
            rx = read8(cpu, (imm + ry) & 0xffff);
            ry += 1;
            pc += 4;
            break;

        case (0x04) : // STW RY IMM RX (store word)
            write16(cpu, (imm + rx) & 0xffff, ry);
            pc += 4;
            break;

        case (0x05) : // STB RY IMM RX (store byte)
            write8(cpu, (imm + rx) & 0xffff, ry);
            pc += 4;
            break;

        case (0x06) : // STW+ RY IMM RX (store word, increment)
            write16(cpu, (imm + rx) & 0xffff, ry);
            rx += 2;
            pc += 4;
            break;

        case (0x07) : // STB+ RY IMM RX (store byte, increment)
            write8(cpu, (imm + rx) & 0xffff, ry);
            rx += 1;
            pc += 4;
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x10) : // ADD RY RX
            alu_add (cpu, rx, ry);
            pc += 2;
            break;

        case (0x11) : // SUB RY RX
            alu_sub(cpu, rx, ry);
            pc += 2;
            break;

        case (0x12) : // MUL RY RX (multiply low)
            alu_mul (cpu, rx, ry);
            pc += 2;
            break;

        case (0x13) : // MULH RY RX (multiply high)
            alu_mulh(cpu, rx, ry);
            pc += 2;
            break;

        case (0x14) : // DIV RY RX
            alu_div(cpu, rx, ry);
            pc += 2;
            break;

        case (0x15) : // SHL RY RX (shift left arithmetic)
            alu_shl (cpu, rx, ry);
            pc += 2;
            break;

        case (0x16) : // SHR RY RX (shift right arithmetic)
            alu_shr (cpu, rx, ry);
            pc += 2;
            break;

        case (0x17) : // MOD RY RX (modulo)
            alu_mod (cpu, rx, ry);
            pc += 2;
            break;

        case (0x18) : // AND RY RX
            alu_and (cpu, rx, ry);
            pc += 2;
            break;

        case (0x19) : // OR  RY RX
            alu_or (cpu, rx, ry);
            pc += 2;
            break;

        case (0x1a) : // XOR RY RX
            alu_xor (cpu, rx, ry);
            pc += 2;
            break;

        case (0x1b) : // MOV RY RX (move)
            alu_mov(cpu, rx, ry);
            pc += 2;
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x20) : // ADD IMM RX
            alu_add (cpu, rx, imm);
            pc += 4;
            break;

        case (0x21) : // SUB IMM RX
            alu_sub(cpu, rx, imm);
            pc += 4;
            break;

        case (0x22) : // MUL IMM RX (multiply low)
            alu_mul (cpu, rx, imm);
            pc += 4;
            break;

        case (0x23) : // MULH IMM RX (multiply high)
            alu_mulh(cpu, rx, imm);
            pc += 4;
            break;

        case (0x24) : // DIV IMM RX
            alu_div(cpu, rx, imm);
            pc += 4;
            break;

        case (0x25) : // MOD IMM RX (modulo)
            alu_mod(cpu, rx, imm);
            pc += 4;
            break;

        case (0x26) : // SHL IMM RX (shift left arithmetic)
            alu_shl (cpu, rx, imm);
            pc += 4;
            break;

        case (0x27) : // SHR IMM RX (shift right arithmetic)
            alu_shr (cpu, rx, imm);
            pc += 4;
            break;

        case (0x28) : // AND IMM RX
            alu_and (cpu, rx, imm);
            pc += 4;
            break;

        case (0x29) : // OR  IMM RX
            alu_or (cpu, rx, imm);
            pc += 4;
            break;

        case (0x2a) : // XOR IMM RX
            alu_xor (cpu, rx, imm);
            pc += 4;
            break;

        case (0x2b) : // MOV IMM RX (move)
            alu_mov (cpu, rx, imm);
            pc += 4;
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x30) : // PUSH RX
            push16 (cpu, rx);
            pc += 2;
            break;

        case (0x31) : // POP  RX
            rx=pop16(cpu);
            pc += 2;
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x40) : // JMP RY RX IMM
            pc = imm; 
            break; 

        case (0x41) : // JNE RY RX IMM
            if (ry!=rx) { pc = imm; } 
            else        { pc+=4; }
            break; 

        case (0x42) : // JEQ RY RX IMM
            if (ry==rx) { pc = imm; } 
            else        { pc+=4; } 
            break; 

        case (0x43) : // JL  RY RX IMM
            if (ry< rx) { pc = imm; } 
            else        { pc+=4; } 
            break;

        case (0x44) : // JG  RY RX IMM
            if (ry> rx) { pc = imm; } 
            else { pc+=4; } 
            break;

        case (0x45) : // JLE RY RX IMM
            if (ry<=rx) { pc = imm; } 
            else        { pc += 4; } 
            break;

        case (0x46) : // JGE RY RX IMM
            if (ry>=rx) { pc = imm; } 
            else        { pc += 4; } 
            break;

        case (0x47) : // CALL IMM
            push16(cpu, pc + 4);
            pc = imm;
            break;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        case (0x50) : // RET (return)
            pc = pop16(cpu);
            break;

        case (0x51) : // BRK (debug break)
            return retired;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

        default:
            // unknown opcode
            raise (cpu, IR_BADINST);
        }
    } // while

    return retired;
}

extern
void cpu16_run(cpu16_t * cpu, uint32_t cycles) {
    assert (cpu);

    cpu->yield_ = false;
    while (cycles > 0) {
        uint32_t ticks = cycles;
        // find max ticks till preemption
        for ( cpu16_device_t * & device : cpu->device_ ) {
            assert (device);
            uint32_t & ctp = device->cycles_till_preempt_;
            if (device->tick_) {
                if (ctp <= 0)
                    device->tick_ (cpu, device);
                ticks = ticks < ctp ? ticks : ctp;
            }
        }
        // 
        if (cpu->yield_)
            break;
        // try to interpret for given number of ticks
        uint32_t retired = cpu16_interp (cpu, ticks);
        // advance preemption by number of retired instructions
        for ( cpu16_device_t * & device : cpu->device_ )
            if (device->tick_)
                device->cycles_till_preempt_ -= retired;
        // we should never retire more then we wanted to
        assert (retired <= cycles);
        cycles -= retired;
    }
}

extern
bool cpu16_load_image(cpu16_t * cpu_, const char * path) {
    assert (cpu_);
    assert (path);
    cpu16_t & cpu = *cpu_;
    FILE * fp = nullptr;
    fp = fopen(path, "rb");
    if (!fp)
        return false;
    if (fread(cpu.mem_, 1, 0x10000, fp) != 0x10000)
        return false;
    fclose(fp);
    return true;
}

extern
cpu16_t * cpu16_new() {
    cpu16_t * cpu = new cpu16_t ();
    assert (cpu);
    memset (cpu->mem_, 0, sizeof (cpu->mem_));
    memset (cpu->reg_, 0, sizeof (cpu->reg_));
    memset (cpu->bus_, 0, sizeof (cpu->bus_));
    cpu16_reset(cpu);
    return cpu;
}

extern
void cpu16_free(cpu16_t * cpu) {
    assert (cpu);
    delete cpu;
}

extern
void cpu16_reset(cpu16_t * cpu) {
    assert (cpu);
    memset (cpu->reg_, 0, sizeof(cpu->reg_));
    cpu->reg_[REG_PC] = VEC_RESET;
    cpu->yield_ = false;
    // init all devices attached
    for (cpu16_device_t * & device : cpu->device_) {
        if (device->init_)
            device->init_ (cpu, device);
    }
}

extern
void cpu16_add_device(cpu16_t *cpu, cpu16_device_t * device)
{
    assert (cpu);
    cpu->device_.push_back (device);
}

extern
void cpu16_map_bus(cpu16_t *cpu, cpu16_bus_map_t * map, uint32_t ix) {
    assert(cpu);
    assert (ix < 64);
    cpu16_bus_map_t * & page = cpu->bus_[ix];
    page = map;
}

extern
uint8_t cpu16_read_byte(cpu16_t *cpu, uint16_t addr) {
    assert(cpu);
    return cpu->mem_[addr];
}

extern
uint16_t cpu16_get_register(cpu16_t *cpu, uint32_t index) {
    assert(cpu);
    return cpu->reg_[index & 0xf];
}

extern
uint8_t * cpu16_get_memory(cpu16_t *cpu) {
    assert(cpu);
    return cpu->mem_;
}

extern
void cpu16_yield(cpu16_t *cpu) {
    cpu->yield_ = true;
}

extern
void cpu16_interrupt(cpu16_t *cpu, uint8_t num) {

    uint16_t can_interrupt = cpu->reg_[REG_CR] & CR_INTERRUPT;

    // check if we can interrupt
    if (can_interrupt) {
        // 8 interrupts max
        num &= 0x07;
        push16(*cpu, cpu->reg_[REG_PC]);
        cpu->reg_[REG_PC] = 0xffe0 + (num * 4);
    }
}
