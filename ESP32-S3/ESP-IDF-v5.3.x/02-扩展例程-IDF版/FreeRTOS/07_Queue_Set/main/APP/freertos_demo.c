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
 
 static QueueSetHandle_t xQueueSet;          /* 定义队列集 */
 QueueHandle_t           xQueue1;            /* 定义队列1 */
 QueueHandle_t           xQueue2;            /* 定义队列2 */
 SemaphoreHandle_t       xSemaphore;         /* 定义二值信号量 */
 
 #define QUEUE_LENGTH            1                   /* 队列支持的消息个数 */
 #define QUEUE_ITEM_SIZE         sizeof(uint8_t)     /* 队列中每条消息的大小 */
 #define SEMAPHORE_BINARY_LENGTH 1                   /* 二值信号量的有效长度 */
 #define QUEUESET_LENGTH         ((2 * QUEUE_LENGTH) + SEMAPHORE_BINARY_LENGTH)  /* 队列集支持的消息个数 */
 
 static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;
 
 /******************************************************************************************************/
 
 /**
  * @brief       FreeRTOS例程入口函数
  * @param       无
  * @retval      无
  */
 void freertos_demo(void)
 {
     spilcd_show_string(10, 10, 220, 32, 32, "ESP32-S3", RED);
     spilcd_show_string(10, 47, 220, 24, 24, "Queue set", RED);
     spilcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
     
     taskENTER_CRITICAL(&my_spinlock);
     /* 创建队列集 */
     xQueueSet = xQueueCreateSet(QUEUESET_LENGTH);
     /* 创建队列 */
     xQueue1 = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
     xQueue2 = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
     /* 创建二值信号量 */
     xSemaphore = xSemaphoreCreateBinary();
     /* 将队列和二值信号量添加到队列集 */
     xQueueAddToSet(xQueue1, xQueueSet);
     xQueueAddToSet(xQueue2, xQueueSet);
     xQueueAddToSet(xSemaphore, xQueueSet);
 
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
     
     taskEXIT_CRITICAL(&my_spinlock);
 }
 
 /**
  * @brief       task1
  * @param       pvParameters : 传入参数(未用到)
  * @retval      无
  */
 void task1(void *pvParameters)
 {
     pvParameters = pvParameters;
     uint8_t key = 0;
     uint8_t timer = 0;
 
     while (1)
     {
         key = xl9555_key_scan(0);
 
         switch (key)
         {
             case KEY0_PRES:                     /* 队列1发送消息 */
             {
                 xQueueSend(xQueue1, &key, portMAX_DELAY);
                 break;
             }
             case KEY1_PRES:                     /* 队列2发送消息 */
             {
                 xQueueSend(xQueue2, &key, portMAX_DELAY);
                 break;
             }
             case KEY2_PRES:                     /* 释放二值信号量 */
             {
                 xSemaphoreGive(xSemaphore);
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
 
 /**
  * @brief       task2
  * @param       pvParameters : 传入参数(未用到)
  * @retval      无
  */
 void task2(void *pvParameters)
 {
     pvParameters = pvParameters;
     QueueSetMemberHandle_t activate_member = NULL;
     uint8_t queue_recv = 0;
     
     while (1)
     {
         activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);/* 等待队列集中的队列接收到消息 */
         
         if (activate_member == xQueue1)
         {
             xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
             ESP_LOGI("QUEUE SET","接收到来自xQueue1的消息:%d", queue_recv);
         }
         else if (activate_member == xQueue2)
         {
             xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
             ESP_LOGI("QUEUE SET","接收到来自xQueue2的消息:%d", queue_recv);
         }
         else if (activate_member == xSemaphore)
         {
             xSemaphoreTake(activate_member, portMAX_DELAY);
             ESP_LOGI("QUEUE SET","获取到二值信号量: xSemaphore");
         }
     }
 }
 