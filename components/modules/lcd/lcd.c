#include "esp_log.h"
#include "lcd.h"
#include "esp_camera.h"
#include "esp_lcd_panel_ops.h"
#include <string.h>

#if __has_include("bsp/esp-bsp.h")
#include "bsp/esp-bsp.h"
#include "lvgl.h"

lv_obj_t *camera_canvas;
uint8_t *cam_buff =NULL;
static const char *TAG = "who_lcd";

static esp_lcd_panel_handle_t panel_handle = NULL;
static QueueHandle_t xQueueFrameI = NULL;
static QueueHandle_t xQueueFrameO = NULL;
static bool gReturnFB = true;


uint32_t cam_buff_size;
uint8_t *cam_buff;

static void task_process_handler(void *arg)
{
    camera_fb_t *frame = NULL;

    

    while (true)
    {
        if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
        {
            // esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, frame->width, frame->height, (uint16_t *)frame->buf);
            
            bsp_display_lock(0);
            memcpy(cam_buff, frame->buf, cam_buff_size);
            if (BSP_LCD_BIGENDIAN) {
                /* Swap bytes in RGB565 */
                lv_draw_sw_rgb565_swap(cam_buff, cam_buff_size);
            }
            lv_obj_invalidate(camera_canvas);
            bsp_display_unlock();
            if (xQueueFrameO)
            {
                xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
            }
            else if (gReturnFB)
            {
                esp_camera_fb_return(frame);
            }
            else
            {
                free(frame);
            }
        }
    }
}

esp_err_t register_lcd(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb)
{

    cam_buff_size = BSP_LCD_H_RES * BSP_LCD_V_RES * 2;
    cam_buff = heap_caps_malloc(cam_buff_size, MALLOC_CAP_SPIRAM);
    assert(cam_buff);

    bsp_display_start();
    bsp_display_lock(0);
    camera_canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(camera_canvas, cam_buff, BSP_LCD_H_RES, BSP_LCD_V_RES, LV_COLOR_FORMAT_RGB565);
    assert(camera_canvas);
    lv_obj_center(camera_canvas);
    bsp_display_unlock();

    bsp_display_backlight_on();

    // app_lcd_set_color(0x000000);
    // vTaskDelay(pdMS_TO_TICKS(200));

    xQueueFrameI = frame_i;
    xQueueFrameO = frame_o;
    gReturnFB = return_fb;
    xTaskCreatePinnedToCore(task_process_handler, TAG, 4 * 1024, NULL, 5, NULL, 0);

    return ESP_OK;
}


void app_lcd_set_color(int color)
{
    uint16_t *buffer = (uint16_t *)malloc(BSP_LCD_H_RES * sizeof(uint16_t));
    if (NULL == buffer)
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
    }
    else
    {
        for (size_t i = 0; i < BSP_LCD_H_RES; i++)
        {
            buffer[i] = color;
        }

        for (int y = 0; y < BSP_LCD_V_RES; y++)
        {
            esp_lcd_panel_draw_bitmap(panel_handle, 0, y, BSP_LCD_H_RES, y+1, buffer);
        }

        free(buffer);
    }
}
#endif // __has_include("bsp/display.h")
