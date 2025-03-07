#pragma once

#include <list>
#include "esp_camera.h"
#include "__base__.hpp"


#define XCLK_FREQ_HZ 15000000

class AppCamera : public Frame
{
public:
    AppCamera(const pixformat_t pixel_fromat,
              const framesize_t frame_size,
              const uint8_t fb_count,
              QueueHandle_t queue_o = nullptr);

    void run();
};