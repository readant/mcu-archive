/**
 ****************************************************************************************************
 * @file        freertos.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-01
 * @brief       FreeRTOS 实验
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

#ifndef __FREERTOS_DEMO_H
#define __FREERTOS_DEMO_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spilcd.h"
#include "led.h"
#include "key.h"
#include "xl9555.h"


/* 函数声明 */
void freertos_demo(void);

#endif
