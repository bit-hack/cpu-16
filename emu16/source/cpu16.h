#pragma once

#include <stdint.h>

struct cpu16_t;

extern
cpu16_t * cpu16_new ();

extern
void cpu16_free (cpu16_t *);

extern
void cpu16_run (cpu16_t * cpu, int32_t count);

extern
bool cpu16_load_image (cpu16_t * cpu, const char * path);

extern
void cpu16_reset (cpu16_t * cpu);
