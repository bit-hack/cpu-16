#pragma once

#include <stdint.h>

/* The asm16 address bus is divided up into 1k pages.
 * Each page can have a separate asm16_bus_t associated with it
 * allowing easy integration of peripherals.
 */

struct asm16_t;

struct asm16_bus_map_t {

    typedef uint16_t (*read_word_t )(asm16_t *, uint16_t addr, asm16_bus_map_t * map);
    typedef uint8_t  (*read_byte_t )(asm16_t *, uint16_t addr, asm16_bus_map_t * map);
    read_word_t  read_word_;
    read_byte_t  read_byte_;

    typedef void (*write_word_t)(asm16_t *, uint16_t addr, uint16_t val, asm16_bus_map_t * map);
    typedef void (*write_byte_t)(asm16_t *, uint16_t addr, uint8_t  val, asm16_bus_map_t * map);
    write_word_t write_word_;
    write_byte_t write_byte_;

    void * user_;
};

struct asm16_device_t {

    uint32_t cycles_till_preempt_;

    typedef void (*init_t)(asm16_t *, asm16_device_t * device);
    init_t init_;

    typedef void (*tick_t)(asm16_t *, asm16_device_t * device);
    tick_t tick_;

    void * user_;
};

// create a new asm16 context
extern
asm16_t * asm16_new ();

// free a asm16 context
extern
void asm16_free(asm16_t *);

// execute a asm16 context for 'count' instructions
extern
void asm16_run(asm16_t * cpu, uint32_t count);

// load an memory image from disk into a asm16 context
extern
bool asm16_load_image(asm16_t * cpu, const char * path);

// hard reset a asm16 context
extern
void asm16_reset(asm16_t * cpu);

// add a device to the cpu
extern
void asm16_add_device(
    asm16_t * cpu,
    asm16_device_t * device);

// add a memory page controller
extern
void asm16_map_bus(
    asm16_t * cpu,
    asm16_bus_map_t * ctrl,
    uint16_t page);

// read a byte from asm16 memory
extern
uint8_t asm16_read_byte(asm16_t *cpu, uint16_t addr);

// access the memory space
extern
uint8_t * asm16_get_memory(asm16_t *cpu);

// read a register from asm16
extern
uint16_t asm16_get_register(asm16_t *cpu, uint32_t index);

//
extern
bool asm16_add_breakpoint(asm16_t *cpu, uint16_t addr);

//
extern
bool asm16_remove_breakpoint(asm16_t *cpu, uint16_t addr);

//
extern
bool asm16_patch(asm16_t * cpu, uint8_t * in, uint32_t in_len, uint8_t * out, uint32_t * out_len);

//
extern
void asm16_yield(asm16_t *cpu);

//
extern
void asm16_interrupt(asm16_t *cpu, uint8_t num);
