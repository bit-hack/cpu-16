#include "dev_ppu.h"
#include "config.h"
#include "cpu16.h"

// 1mhz / 25fps = 40khz
// 40000 instructions per frame
// 40khz / 120 = 333hz
// 333 instructions per scan line

static const uint32_t CYCLES_PER_HSCAN = CPU_CLOCK / (VIDEO_FPS * VIDEO_HEIGHT);

struct sprite_t {

    uint32_t index_;
    int16_t x_, y_;
};

struct dev_ppu_t {

    uint32_t * screen_;
    uint32_t   pitch_;

    // active scanline
    uint32_t   row_;
};

void dev_ppu_hsync(dev_ppu_t * dev) {

}

void dev_ppu_vsync(dev_ppu_t * dev) {

}

void dev_ppu_tick(cpu16_t *cpu, cpu16_device_t * dev) {

    dev_ppu_t * ppu = (dev_ppu_t*)dev->user_;

    dev->cycles_till_preempt_ = CYCLES_PER_HSCAN;

    if (ppu->row_ < VIDEO_HEIGHT) {
        cpu16_interrupt(cpu, INT_HSYNC);
        ppu->row_++;
    }
    else {
        cpu16_interrupt(cpu, INT_VSYNC);
        ppu->row_ = 0;
        cpu16_yield(cpu);
    }
}

bool dev_ppu_init(state_t * state) {

    cpu16_device_t * dev = new cpu16_device_t;
    dev->cycles_till_preempt_ = 0;
    dev->init_ = nullptr;
    dev->tick_ = dev_ppu_tick;

    dev_ppu_t * ppu = new dev_ppu_t;
    dev->user_ = ppu;
    ppu->row_ = 0;

    cpu16_add_device(state->cpu_, dev);

    return true;
}
