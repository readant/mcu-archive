/**
 ****************************************************************************************************
 * @file        led.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       LED 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20230805
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __LED_H
#define __LED_H

#include "Arduino.h"

/* 引脚定义 */
#define LED_PIN       1   /* 开发板上LED连接到GPIO1引脚 */

/* 宏函数定义 */
#define LED(x)        digitalWrite(LED_PIN, x)
#define LED_TOGGLE()  digitalWrite(LED_PIN, !digitalRead(LED_PIN))

/* 函数声明 */
void led_init(void);      /* led引脚初始化函数 */

#endif
