/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>


long int sysconf(int);
void init_opencv_features2d();
void test_opencv_features2d(uint8_t* data, int width, int heigth);
void register_cv_2d_feature(const QueueHandle_t frame_i,
                                 const QueueHandle_t event,
                                 const QueueHandle_t result,
                                 const QueueHandle_t frame_o,const bool camera_fb_return);