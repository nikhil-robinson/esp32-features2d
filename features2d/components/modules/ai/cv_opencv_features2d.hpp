#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


void init_opencv_features2d();
void test_opencv_features2d(uint8_t* data, int width, int heigth);
void register_cv_2d_feature(QueueHandle_t frame_i,
                                   QueueHandle_t event,
                                   QueueHandle_t result,
                                   QueueHandle_t frame_o = NULL,
                                   const bool camera_fb_return = false);
