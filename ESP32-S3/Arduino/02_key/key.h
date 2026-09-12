/**
 ****************************************************************************************************
 * @file        key.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       KEY 驱动代码
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
 * V1.0 20231201
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "Arduino.h"

/* 引脚定义 */
#define KEY_PIN       0   /* 开发板上KEY连接到GPIO0引脚 */

/* 宏函数定义 */
#define KEY           digitalRead(KEY_PIN)    /* 读取KEY引脚的状态 */

/* 函数声明 */
void key_init(void);      /* key引脚初始化函数 */

#endif
