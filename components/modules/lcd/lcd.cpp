#include "esp_log.h"
#include "lcd.hpp"
#include "esp_camera.h"
#include "esp_lcd_panel_ops.h"
#include <string.h>

#if __has_include("bsp/esp-bsp.h")
#include "bsp/esp-bsp.h"
#include "lvgl.h"

static const char TAG[] = "App/LCD";

AppLCD::AppLCD(QueueHandle_t queue_i,QueueHandle_t queue_o,void (*callback)(camera_fb_t *)) : Frame(queue_i, queue_o, callback)
{
    bsp_display_start();

    this->cam_buff_size = BSP_LCD_H_RES * BSP_LCD_V_RES * 2;
    this->cam_buff = (uint8_t *)heap_caps_malloc(this->cam_buff_size, MALLOC_CAP_SPIRAM);
    assert(this->cam_buff);

    bsp_display_lock(0);
    this->camera_canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(this->camera_canvas, this->cam_buff, BSP_LCD_H_RES, BSP_LCD_V_RES, LV_COLOR_FORMAT_RGB565);
    assert(this->camera_canvas);
    lv_obj_center(this->camera_canvas);
    bsp_display_unlock();
    bsp_display_backlight_on();
}


void AppLCD::update()
{

}

static void task(AppLCD *self)
{
    ESP_LOGD(TAG, "Start");

    camera_fb_t *frame = nullptr;
    while (true)
    {
        if (self->queue_i == nullptr)
            break;

        if (xQueueReceive(self->queue_i, &frame, portMAX_DELAY))
        {

            bsp_display_lock(0);
            ESP_LOGI(TAG,"MEMCPY");
            memcpy(self->cam_buff, frame->buf, self->cam_buff_size);
            if (BSP_LCD_BIGENDIAN)
            {
                /* Swap bytes in RGB565 */
                lv_draw_sw_rgb565_swap(self->cam_buff, self->cam_buff_size);
            }
            lv_canvas_set_buffer(this->camera_canvas, self->cam_buff, frame->width, frame->height, LV_COLOR_FORMAT_RGB565);
            bsp_display_unlock();


            if (self->queue_o)
                xQueueSend(self->queue_o, &frame, portMAX_DELAY);
            else
                self->callback(frame);
        }
    }
    ESP_LOGD(TAG, "Stop");
    vTaskDelete(NULL);
}


void AppLCD

void AppLCD::run()
{
    xTaskCreatePinnedToCore((TaskFunction_t)task, TAG, 2 * 1024, this, 5, NULL, 1);
}

#endif // __has_include("bsp/display.h")