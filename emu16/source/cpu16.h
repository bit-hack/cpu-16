#pragma once

#include <stdint.h>

struct cpu16_t;

struct cpu16_bus_t {

    uint16_t (*read16)(
        cpu16_t *, 
        uint16_t addr, 
        void * user);

    uint8_t (*read8)(
        cpu16_t *,
        uint16_t addr,
        void * user);

    void (*write16)(
        cpu16_t *,
        uint16_t addr,
        uint16_t val,
        void * user);

    void (*write8)(
        cpu16_t *,
        uint16_t addr,
        uint8_t val, 
        void * user);

    void * user_;
};

extern
cpu16_t * cpu16_new ();

extern
void cpu16_free(cpu16_t *);

extern
void cpu16_run(cpu16_t * cpu, int32_t count);

extern
bool cpu16_load_image(cpu16_t * cpu, const char * path);

extern
void cpu16_reset(cpu16_t * cpu);

extern
void cpu16_add_peripheral(
    cpu16_t *cpu, 
    cpu16_bus_t * bus, 
    uint16_t page_start,
    uint16_t page_end);
