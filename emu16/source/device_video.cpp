#include "device_video.h"
#include "config.h"

// 1mhz / 25fps = 40khz
// 40000 instructions per frame
// 40khz / 120 = 333hz
// 333 instructions per scan line

static const uint32_t CYCLES_PER_HSCAN = CPU_CLOCK / (VIDEO_FPS * VIDEO_HEIGHT);

struct sprite_t {

    uint32_t index_;
    int16_t x_, y_;
};

struct device_video_t {

    uint32_t screen_[VIDEO_WIDTH * VIDEO_HEIGHT];
    uint32_t row_;

    // y ordered list of sprites
    sprite_t sprite_[VIDEO_MAX_SPRITES];
};

void dev_video_hsync(device_video_t * dev) {
    // cpu->yield_ = true;
}

void dev_video_vsync(device_video_t * dev) {
    // exit main loop and present to screen
}

void dev_video_tick(device_video_t * dev) {
    if (dev->row_ >= VIDEO_HEIGHT) {
        dev_video_hsync(dev);
        dev->row_++;
    }
    else {
        dev_video_vsync(dev);
        dev->row_ = 0;
    }
}
