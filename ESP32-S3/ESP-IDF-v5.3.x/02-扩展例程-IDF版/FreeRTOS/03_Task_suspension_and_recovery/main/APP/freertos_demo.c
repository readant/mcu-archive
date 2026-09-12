/**
 ****************************************************************************************************
* @file        freertos.c
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

#include "freertos_demo.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

/* TASK1 任务 配置
* 包括: 任务句柄 任务优先级 堆栈大小 创建任务
*/
#define TASK1_PRIO      1                   /* 任务优先级 */
#define TASK1_STK_SIZE  5*1024              /* 任务堆栈大小 */
TaskHandle_t            Task1Task_Handler;  /* 任务句柄 */
void task1(void *pvParameters);             /* 任务函数 */

/* TASK2 任务 配置
* 包括: 任务句柄 任务优先级 堆栈大小 创建任务
*/
#define TASK2_PRIO      2                   /* 任务优先级 */
#define TASK2_STK_SIZE  5*1024              /* 任务堆栈大小 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */

/* TASK3 任务 配置
* 包括: 任务句柄 任务优先级 堆栈大小 创建任务
*/
#define TASK3_PRIO      4                   /* 任务优先级 */
#define TASK3_STK_SIZE  5*1024              /* 任务堆栈大小 */
TaskHandle_t            Task3Task_Handler;  /* 任务句柄 */
void task3(void *pvParameters);             /* 任务函数 */


/******************************************************************************************************/

/* LCD刷屏时使用的颜色 */
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                            MAGENTA, GREEN, CYAN, YELLOW,
                            BROWN, BRRED, GRAY};


/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    spilcd_show_string(10, 10, 220, 32, 32, "ESP32-S3", RED);
    spilcd_show_string(10, 47, 220, 24, 24, "Task Susp & Resum", RED);
    spilcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    /* 创建任务1 */
    xTaskCreatePinnedToCore((TaskFunction_t )task1,                 /* 任务函数 */
                            (const char*    )"task1",               /* 任务名称 */
                            (uint16_t       )TASK1_STK_SIZE,        /* 任务堆栈大小 */
                            (void*          )NULL,                  /* 传入给任务函数的参数 */
                            (UBaseType_t    )TASK1_PRIO,            /* 任务优先级 */
                            (TaskHandle_t*  )&Task1Task_Handler,    /* 任务句柄 */
                            (BaseType_t     ) 0);                   /* 该任务哪个内核运行 */
    /* 创建任务2 */
    xTaskCreatePinnedToCore((TaskFunction_t )task2,                 /* 任务函数 */
                            (const char*    )"task2",               /* 任务名称 */
                            (uint16_t       )TASK2_STK_SIZE,        /* 任务堆栈大小 */
                            (void*          )NULL,                  /* 传入给任务函数的参数 */
                            (UBaseType_t    )TASK2_PRIO,            /* 任务优先级 */
                            (TaskHandle_t*  )&Task2Task_Handler,    /* 任务句柄 */
                            (BaseType_t     ) 0);                   /* 该任务哪个内核运行 */

    /* 创建任务3 */
    xTaskCreatePinnedToCore((TaskFunction_t )task3,                 /* 任务函数 */
                            (const char*    )"task3",               /* 任务名称 */
                            (uint16_t       )TASK3_STK_SIZE,        /* 任务堆栈大小 */
                            (void*          )NULL,                  /* 传入给任务函数的参数 */
                            (UBaseType_t    )TASK3_PRIO,            /* 任务优先级 */
                            (TaskHandle_t*  )&Task3Task_Handler,    /* 任务句柄 */
                            (BaseType_t     ) 0);                   /* 该任务哪个内核运行 */
}

/**
 * @brief       task1
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task1(void *pvParameters)
{
    pvParameters = pvParameters;
    uint32_t task1_num = 0;
    
    while (1)
    {
        task1_num ++;
        ESP_LOGI("task1","task1:%ld.................",task1_num);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief       task2
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task2(void *pvParameters)
{
    pvParameters = pvParameters;
    uint32_t task2_num = 0;
    
    while (1)
    {
        task2_num ++;
        ESP_LOGI("task2","task2:%ld.................",task2_num);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief       task3
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task3(void *pvParameters)
{
    pvParameters = pvParameters;
    uint8_t key = 0;
    uint8_t timer = 0;

    while (1)
    {
        key = xl9555_key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                ESP_LOGW("task1","vTask1 Suspend");
                vTaskSuspend(Task1Task_Handler);
                break;
            }
            case KEY1_PRES:
            {
                ESP_LOGW("task1","vTask1 Resume");
                vTaskResume(Task1Task_Handler);
                break;
            }
            default:
            {
                break;
            }
        }
        
        timer ++;

        if (timer % 30 == 0)
        {
            LED0_TOGGLE();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
