#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "lvgl.h"

#include "__base__.hpp"
#include "camera.hpp"

class AppLCD : public Observer, public Frame
{
private:

public:

    lv_obj_t *camera_canvas;
    uint32_t cam_buff_size;
    uint8_t *cam_buff;

    AppLCD(QueueHandle_t xQueueFrameI = nullptr,
           QueueHandle_t xQueueFrameO = nullptr,
           void (*callback)(camera_fb_t *) = esp_camera_fb_return);

    void update();

    void run();
};