#include "camera.hpp"
#include "AppFeature2D.hpp"
#include "lcd.hpp"

extern "C" void app_main()
{
    QueueHandle_t xQueueFrame_0 = xQueueCreate(2, sizeof(camera_fb_t *));
    QueueHandle_t xQueueFrame_1 = xQueueCreate(2, sizeof(camera_fb_t *));


    AppCamera *camera = new AppCamera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueFrame_0);
    AppFeature2D *feature = new AppFeature2D(xQueueFrame_0, xQueueFrame_1);
    AppLCD *lcd = new AppLCD(xQueueFrame_1);

    lcd->run();
    feature->run();
    camera->run();
}