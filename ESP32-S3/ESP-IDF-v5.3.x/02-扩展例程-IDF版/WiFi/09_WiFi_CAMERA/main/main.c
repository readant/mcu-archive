/**
 ******************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-01
 * @brief       WIFI UDP实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ******************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 ******************************************************************************
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "led.h"
#include "myiic.h"
#include "my_spi.h"
#include "spilcd.h"
#include "xl9555.h"
#include "wifi_config.h"
#include "lwip_demo.h"
#include "esp_camera.h"
#include <stdio.h>


/* 引脚配置 */
#define CAM_PIN_PWDN    GPIO_NUM_NC
#define CAM_PIN_RESET   GPIO_NUM_NC
#define CAM_PIN_VSYNC   GPIO_NUM_47
#define CAM_PIN_HREF    GPIO_NUM_48
#define CAM_PIN_PCLK    GPIO_NUM_45
#define CAM_PIN_XCLK    GPIO_NUM_NC
#define CAM_PIN_SIOD    GPIO_NUM_39
#define CAM_PIN_SIOC    GPIO_NUM_38
#define CAM_PIN_D0      GPIO_NUM_4
#define CAM_PIN_D1      GPIO_NUM_5
#define CAM_PIN_D2      GPIO_NUM_6
#define CAM_PIN_D3      GPIO_NUM_7
#define CAM_PIN_D4      GPIO_NUM_15
#define CAM_PIN_D5      GPIO_NUM_16
#define CAM_PIN_D6      GPIO_NUM_17
#define CAM_PIN_D7      GPIO_NUM_18


#define CAM_PWDN(x)         do{ x ? \
                                (xl9555_pin_write(OV_PWDN_IO, 1)):       \
                                (xl9555_pin_write(OV_PWDN_IO, 0));       \
                            }while(0)

#define CAM_RST(x)          do{ x ? \
                                (xl9555_pin_write(OV_RESET_IO, 1)):       \
                                (xl9555_pin_write(OV_RESET_IO, 0));       \
                            }while(0)

/* 摄像头配置 */
static camera_config_t camera_config = {
    /* 引脚配置 */
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    /* XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental) */
    .xclk_freq_hz = 24000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG,     /* YUV422,GRAYSCALE,RGB565,JPEG */
    .frame_size = FRAMESIZE_QVGA,       /* QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates */

    .jpeg_quality = 12,                 /* 0-63, for OV series camera sensors, lower number means higher quality */
    .fb_count = 2,                      /* When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode */
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

/**
 * @brief       摄像头初始化
 * @param       无
 * @retval      esp_err_t
 */
static esp_err_t init_camera(void)
{
    if (CAM_PIN_PWDN == GPIO_NUM_NC)
    {
        CAM_PWDN(0);
    } 

    if (CAM_PIN_RESET == GPIO_NUM_NC)
    { 
        CAM_RST(0);
        vTaskDelay(pdMS_TO_TICKS(20));
        CAM_RST(1);
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    /* 摄像头初始化 */
    esp_err_t err = esp_camera_init(&camera_config);

    if (err != ESP_OK)
    {
        ESP_LOGE("TAG", "Camera Init Failed");
        return err;
    }

    sensor_t * s = esp_camera_sensor_get();

    /* 如果摄像头模块是OV3660或者是OV5640，则需要以下配置 */
    if (s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 1);         /* 向后翻转 */
        s->set_brightness(s, 1);    /* 亮度提高 */
        s->set_saturation(s, -2);   /* 降低饱和度 */
    }
    else if (s->id.PID == OV5640_PID)
    {
        s->set_vflip(s, 1);         /* 向后翻转 */
    }

    return ESP_OK;
}

/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();     /* 初始化NVS */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    led_init();                 /* LED初始化 */
    my_spi_init();              /* SPI初始化 */
    myiic_init();               /* IIC初始化 */  
    xl9555_init();              /* 初始化按键 */
    spilcd_init();              /* LCD屏初始化 */
 
    spilcd_show_string(0, 0, 240, 32, 32, "ESP32-S3", RED);
    spilcd_show_string(0, 40, 240, 24, 24, "WiFi CAMERA Test", RED);
    spilcd_show_string(0, 70, 240, 16, 16, "ATOM@ALIENTEK", RED);
    wifi_sta_init();

    /* 初始化摄像头 */
    while (init_camera())
    {
        spilcd_show_string(30, 110, 200, 16, 16, "CAMERA Fail!", BLUE);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    lwip_demo();            /* lwip测试代码 */
}
