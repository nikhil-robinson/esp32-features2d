#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "__base__.hpp"
#include "camera.hpp"
#include "esp_log.h"



class AppFeature2D : public Observer, public Frame
{
private:
public:
    bool switch_on;

    AppFeature2D(QueueHandle_t queue_i = nullptr,
                QueueHandle_t queue_o = nullptr,
                bool show_img,
                void (*callback)(camera_fb_t *) = esp_camera_fb_return);

    void update();

    void run();
};
