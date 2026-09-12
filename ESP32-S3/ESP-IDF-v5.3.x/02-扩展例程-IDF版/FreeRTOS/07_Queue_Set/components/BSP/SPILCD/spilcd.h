/**
 ****************************************************************************************************
 * @file        spilcd.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-01
 * @brief       SPILCD驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef __SPILCD_H
#define __SPILCD_H

#include "driver/gpio.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "esp_log.h"
#include "esp_lcd_panel_st7789.h"
#include "xl9555.h"
#include "math.h"


/* 管脚声明 */
#define LCD_RST_PIN         GPIO_NUM_NC
#define LCD_PWR_PIN         GPIO_NUM_NC
#define LCD_DC_PIN          GPIO_NUM_40
#define LCD_CS_PIN          GPIO_NUM_21

#define LCD_PWR(x)          do { x ?                                \
                                 xl9555_pin_write(SLCD_PWR_IO, 1):  \
                                 xl9555_pin_write(SLCD_PWR_IO, 0);  \
                            } while(0)

#define LCD_RST(x)          do { x ?                                 \
                                 xl9555_pin_write(SLCD_RST_IO, 1):   \
                                 xl9555_pin_write(SLCD_RST_IO, 0);   \
                            } while(0)

#define LCD_HOST            SPI2_HOST

/* 常用颜色值 */
#define WHITE               0xFFFF      /* 白色 */
#define BLACK               0x0000      /* 黑色 */
#define RED                 0xF800      /* 红色 */
#define GREEN               0x07E0      /* 绿色 */
#define BLUE                0x001F      /* 蓝色 */ 
#define MAGENTA             0XF81F      /* 洋红色 */
#define YELLOW              0XFFE0      /* 黄色 */
#define CYAN                0X07FF      /* 蓝绿色 */

/* 非常用颜色 */
#define BROWN               0XBC40      /* 棕色 */
#define BRRED               0XFC07      /* 棕红色 */
#define GRAY                0X8430      /* 灰色 */ 
#define DARKBLUE            0X01CF      /* 深蓝色 */
#define LIGHTBLUE           0X7D7C      /* 浅蓝色 */ 
#define GRAYBLUE            0X5458      /* 灰蓝色 */ 
#define LIGHTGREEN          0X841F      /* 浅绿色 */  
#define LGRAY               0XC618      /* 浅灰色(PANNEL),窗体背景色 */ 
#define LGRAYBLUE           0XA651      /* 浅灰蓝色(中间层颜色) */ 
#define LBBLUE              0X2B12      /* 浅棕蓝色(选择条目的反色) */ 

typedef struct  
{
    uint32_t pwidth;    /* 临时设定值（宽度） */
    uint32_t pheight;   /* 临时设定值（高度） */
    uint8_t  dir;       /* 屏幕方向 */
    uint16_t width;     /* 宽度 */
    uint16_t height;    /* 高度 */
} _spilcd_dev; 

extern _spilcd_dev spilcddev;;
extern esp_lcd_panel_handle_t panel_handle;

/* 函数声明 */
esp_err_t spilcd_init(void);                /* spilcd初始化 */
void spilcd_display_dir(uint8_t dir);       /* 设置屏幕方向 */
void spilcd_clear(uint16_t color);          /* 清屏 */
void spilcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);           /* 在指定区域内填充单个颜色 */
void spilcd_draw_point(uint16_t x, uint16_t y, uint16_t color);                                 /* 绘画一个像素点 */
void spilcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);      /* 画线函数(直线、斜线) */
void spilcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);                   /* 画水平线 */
void spilcd_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color);  /* 画一个矩形 */
void spilcd_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);                  /* 画一个圆 */
void spilcd_show_char(uint16_t x, uint16_t y, uint8_t chr, uint8_t size, uint8_t mode, uint16_t color);                     /* 在指定位置显示一个字符 */
void spilcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);                      /* 显示len个数字 */
void spilcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);       /* 扩展显示len个数字(高位是0也显示) */
void spilcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);    /* 显示字符串 */



#endif
