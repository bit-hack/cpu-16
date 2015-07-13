#include <stdint.h>
#include <stdio.h>

#include "cpu16.h"

void serial_write_8(cpu16_t *, uint16_t addr, uint8_t val, void * user) {
    putchar (val);
}

int main(int argc, const char ** args) {

    if (argc != 2) {
        printf("%s <input.img>\n", args[0]);
        return -1;
    }

    cpu16_t * cpu = cpu16_new ();
    cpu16_bus_t bus = {
        nullptr,
        nullptr,
        nullptr,
        serial_write_8,
    };
    cpu16_add_peripheral(cpu, &bus, 0x20, 0x20);

    if (!cpu16_load_image(cpu, args[1])) {
        printf("failed to load image\n");
        return -2;
    }

    cpu16_reset (cpu);
    cpu16_run(cpu, 1024);

    return 0;
}
