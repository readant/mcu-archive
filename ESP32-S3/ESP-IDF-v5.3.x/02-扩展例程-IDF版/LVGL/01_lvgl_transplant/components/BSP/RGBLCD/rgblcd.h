/**
 ****************************************************************************************************
 * @file        rgblcd.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-01
 * @brief       LTDC 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __RGBLCD_H
#define __RGBLCD_H

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_ldo_regulator.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lcd.h"


/* RGBLCD引脚 */
#define GPIO_LCD_DE     (GPIO_NUM_4)
#define GPIO_LCD_VSYNC  (GPIO_NUM_NC)
#define GPIO_LCD_HSYNC  (GPIO_NUM_NC)
#define GPIO_LCD_PCLK   (GPIO_NUM_5)

#define GPIO_LCD_R3     (GPIO_NUM_45)
#define GPIO_LCD_R4     (GPIO_NUM_48)
#define GPIO_LCD_R5     (GPIO_NUM_47)
#define GPIO_LCD_R6     (GPIO_NUM_21)
#define GPIO_LCD_R7     (GPIO_NUM_14)

#define GPIO_LCD_G2     (GPIO_NUM_10)
#define GPIO_LCD_G3     (GPIO_NUM_9)
#define GPIO_LCD_G4     (GPIO_NUM_46)
#define GPIO_LCD_G5     (GPIO_NUM_3)
#define GPIO_LCD_G6     (GPIO_NUM_8)
#define GPIO_LCD_G7     (GPIO_NUM_18)

#define GPIO_LCD_B3     (GPIO_NUM_17)
#define GPIO_LCD_B4     (GPIO_NUM_16)
#define GPIO_LCD_B5     (GPIO_NUM_15)
#define GPIO_LCD_B6     (GPIO_NUM_7)
#define GPIO_LCD_B7     (GPIO_NUM_6)


/* LCD RGBLCD重要参数集 */
typedef struct  
{
    uint32_t pwidth;        /* RGBLCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入 */
    uint32_t pheight;       /* RGBLCD面板的高度,固定参数,不随显示方向改变 */
    uint16_t hsw;           /* 水平同步宽度 */
    uint16_t vsw;           /* 垂直同步宽度 */
    uint16_t hbp;           /* 水平后廊 */
    uint16_t vbp;           /* 垂直后廊 */
    uint16_t hfp;           /* 水平前廊 */
    uint16_t vfp;           /* 垂直前廊  */
    uint8_t activelayer;    /* 当前层编号:0/1 */
    uint8_t dir;            /* 0,竖屏;1,横屏; */
    uint16_t id;            /* RGBLCD ID */
    uint32_t pclk_hz;       /* 设置像素时钟 */
    uint16_t width;         /* RGBLCD宽度 */
    uint16_t height;        /* RGBLCD高度 */
} _rgblcd_dev; 

extern _rgblcd_dev rgbdev;                  /* 管理RGBLCD重要参数 */

/* 函数声明 */
esp_lcd_panel_handle_t rgblcd_init(void);   /* 初始化RGBLCD */
void rgblcd_display_dir(uint8_t dir);       /* 设置rgb方向 */

#endif
