set(requires    
                nvs_flash
                esp_adc
                esp_lcd
                esp_timer
                esp32-camera
                opencv)

idf_component_register( SRC_DIRS camera lcd fb_gfx
                        INCLUDE_DIRS camera lcd fb_gfx
                        REQUIRES ${requires})