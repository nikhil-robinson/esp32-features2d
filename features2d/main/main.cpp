#include "camera.h"
#include "cv_opencv_features2d.hpp"
#include "lcd.h"

static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueLCDFrame = NULL;

extern "C" void app_main(void)
{
    xQueueAIFrame = xQueueCreate(1, sizeof(camera_fb_t *));
    xQueueLCDFrame = xQueueCreate(1, sizeof(camera_fb_t *));

    register_camera(PIXFORMAT_GRAYSCALE, FRAMESIZE_240X240, 1, xQueueAIFrame);
    register_cv_2d_feature(xQueueAIFrame, NULL, NULL, NULL, true);
    // register_lcd(xQueueLCDFrame, NULL, true);
}
