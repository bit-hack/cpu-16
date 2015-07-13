#pragma once

#include <stdint.h>

/* The CPU16 address bus is divided up into 1k pages.
 * Each page can have a separate cpu16_bus_t associated with it
 * allowing easy integration of peripherals.
 */

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

// create a new CPU16 context
extern
cpu16_t * cpu16_new ();

// free a CPU16 context
extern
void cpu16_free(cpu16_t *);

// execute a CPU16 context for 'count' instructions
extern
void cpu16_run(cpu16_t * cpu, int32_t count);

// load an memory image from disk into a CPU16 context
extern
bool cpu16_load_image(cpu16_t * cpu, const char * path);

// hard reset a CPU16 context
extern
void cpu16_reset(cpu16_t * cpu);

// add a peripheral to the CPU16 address/data bus
extern
void cpu16_add_peripheral(
    cpu16_t *cpu, 
    cpu16_bus_t * bus, 
    uint16_t page_start,
    uint16_t page_end);
