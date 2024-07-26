set(requires    
                nvs_flash
                esp_adc
                esp_lcd
                esp_timer
                esp32-camera
                opencv
                esp32_s3_eye)

idf_component_register( SRC_DIRS ai camera lcd fb_gfx
                        INCLUDE_DIRS ai camera lcd fb_gfx
                        REQUIRES ${requires})