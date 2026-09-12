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
 
 List_t                  TestList;           /* 定义测试列表 */
 ListItem_t              ListItem1;          /* 定义测试列表项1 */
 ListItem_t              ListItem2;          /* 定义测试列表项2 */
 ListItem_t              ListItem3;          /* 定义测试列表项3 */
 /******************************************************************************************************/
 
 /**
  * @brief       FreeRTOS例程入口函数
  * @param       无
  * @retval      无
  */
 void freertos_demo(void)
 {
     spilcd_show_string(10, 10, 220, 32, 32, "ESP32-S3", RED);
     spilcd_show_string(10, 47, 220, 24, 24, "List & ListItem", RED);
     spilcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
     
     /* 创建任务1 */
     xTaskCreatePinnedToCore((TaskFunction_t )task1,                 /* 任务函数 */
                             (const char*    )"task1",               /* 任务名称 */
                             (uint16_t       )TASK1_STK_SIZE,        /* 任务堆栈大小 */
                             (void*          )NULL,                  /* 传入给任务函数的参数 */
                             (UBaseType_t    )TASK1_PRIO,            /* 任务优先级 */
                             (TaskHandle_t*  )&Task1Task_Handler,    /* 任务句柄 */
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
     /* 第一步：初始化列表和列表项 */
     vListInitialise(&TestList);                 /* 初始化列表 */
     vListInitialiseItem(&ListItem1);            /* 初始化列表项1 */
     vListInitialiseItem(&ListItem2);            /* 初始化列表项2 */
     vListInitialiseItem(&ListItem3);            /* 初始化列表项3 */
     
     /* 第二步：打印列表和其他列表项的地址 */
     ESP_LOGI("freertos_demo", "/**************第二步: 打印列表和列表项的地址**************/");
     ESP_LOGI("freertos_demo", "项目\t\t\t\t地址");
     ESP_LOGI("freertos_demo", "TestList\t\t0x%p", &TestList);
     ESP_LOGI("freertos_demo", "TestList->pxIndex\t0x%p", TestList.pxIndex);
     ESP_LOGI("freertos_demo", "TestList->xListEnd\t0x%p", &TestList.xListEnd);
     ESP_LOGI("freertos_demo", "ListItem1\t\t0x%p  ", &ListItem1);
     ESP_LOGI("freertos_demo", "ListItem2\t\t0x%p  ", &ListItem2);
     ESP_LOGI("freertos_demo", "ListItem3\t\t0x%p  ", &ListItem3);
     ESP_LOGI("freertos_demo", "/**************************结束***************************/");
     ESP_LOGI("freertos_demo", "按下KEY0键继续!");
 
     while (xl9555_key_scan(0) != KEY0_PRES)
     {
         vTaskDelay(pdMS_TO_TICKS(10));
     }
 
     /* 第三步：列表项1插入列表 */
     ESP_LOGI("freertos_demo", "/*****************第三步: 列表项1插入列表******************/");
     vListInsert((List_t*    )&TestList,         /* 列表 */
                 (ListItem_t*)&ListItem1);       /* 列表项 */
     ESP_LOGI("freertos_demo", "项目\t\t\t\t地址");
     ESP_LOGI("freertos_demo", "TestList->xListEnd->pxNext\t0x%p", (TestList.xListEnd.pxNext));
     ESP_LOGI("freertos_demo", "ListItem1->pxNext\t\t0x%p", (ListItem1.pxNext));
     ESP_LOGI("freertos_demo", "TestList->xListEnd->pxPrevious\t0x%p", (TestList.xListEnd.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem1->pxPrevious\t\t0x%p", (ListItem1.pxPrevious));
     ESP_LOGI("freertos_demo", "/**************************结束***************************/");
     ESP_LOGI("freertos_demo", "按下KEY0键继续!");
 
     while (xl9555_key_scan(0) != KEY0_PRES)
     {
         vTaskDelay(pdMS_TO_TICKS(10));
     }
 
     /* 第四步：列表项2插入列表 */
     ESP_LOGI("freertos_demo", "/*****************第四步: 列表项2插入列表******************/");
     vListInsert((List_t*    )&TestList,         /* 列表 */
                 (ListItem_t*)&ListItem2);       /* 列表项 */
     ESP_LOGI("freertos_demo", "项目\t\t\t\t地址\r\n");
     ESP_LOGI("freertos_demo", "TestList->xListEnd->pxNext\t0x%p", (TestList.xListEnd.pxNext));
     ESP_LOGI("freertos_demo", "ListItem1->pxNext\t\t0x%p", (ListItem1.pxNext));
     ESP_LOGI("freertos_demo", "ListItem2->pxNext\t\t0x%p", (ListItem2.pxNext));
     ESP_LOGI("freertos_demo", "TestList->xListEnd->pxPrevious\t0x%p", (TestList.xListEnd.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem1->pxPrevious\t\t0x%p", (ListItem1.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem2->pxPrevious\t\t0x%p", (ListItem2.pxPrevious));
     ESP_LOGI("freertos_demo", "/**************************结束***************************/");
     ESP_LOGI("freertos_demo", "按下KEY0键继续!");
 
     while (xl9555_key_scan(0) != KEY0_PRES)
     {
         vTaskDelay(pdMS_TO_TICKS(10));
     }
 
     /* 第五步：列表项3插入列表 */
     ESP_LOGI("freertos_demo", "/*****************第五步: 列表项3插入列表******************/");
     vListInsert((List_t*    )&TestList,         /* 列表 */
                 (ListItem_t*)&ListItem3);       /* 列表项 */
     ESP_LOGI("freertos_demo", "项目\t\t\t\t地址");
     ESP_LOGI("freertos_demo", "TestList->xListEnd->pxNext\t0x%p", (TestList.xListEnd.pxNext));
     ESP_LOGI("freertos_demo", "ListItem1->pxNext\t\t0x%p", (ListItem1.pxNext));
     ESP_LOGI("freertos_demo", "ListItem2->pxNext\t\t0x%p", (ListItem2.pxNext));
     ESP_LOGI("freertos_demo", "ListItem3->pxNext\t\t0x%p", (ListItem3.pxNext));
     ESP_LOGI("freertos_demo", "TestList->xListEnd->pxPrevious\t0x%p", (TestList.xListEnd.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem1->pxPrevious\t\t0x%p", (ListItem1.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem2->pxPrevious\t\t0x%p", (ListItem2.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem3->pxPrevious\t\t0x%p", (ListItem3.pxPrevious));
     ESP_LOGI("freertos_demo", "/**************************结束***************************/");
     ESP_LOGI("freertos_demo", "按下KEY0键继续!");
 
     while (xl9555_key_scan(0) != KEY0_PRES)
     {
         vTaskDelay(pdMS_TO_TICKS(10));
     }
 
     /* 第六步：移除列表项2 */
     ESP_LOGI("freertos_demo", "/*******************第六步: 移除列表项2********************/");
     uxListRemove((ListItem_t*   )&ListItem2);   /* 移除列表项 */
     ESP_LOGI("freertos_demo", "项目\t\t\t\t地址\r\n");
     ESP_LOGI("freertos_demo", "TestList->xListEnd->pxNext\t0x%p", (TestList.xListEnd.pxNext));
     ESP_LOGI("freertos_demo", "ListItem1->pxNext\t\t0x%p", (ListItem1.pxNext));
     ESP_LOGI("freertos_demo", "ListItem3->pxNext\t\t0x%p", (ListItem3.pxNext));
     ESP_LOGI("freertos_demo", "TestList->xListEnd->pxPrevious\t0x%p", (TestList.xListEnd.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem1->pxPrevious\t\t0x%p", (ListItem1.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem3->pxPrevious\t\t0x%p", (ListItem3.pxPrevious));
     ESP_LOGI("freertos_demo", "/**************************结束***************************/");
     ESP_LOGI("freertos_demo", "按下KEY0键继续!");
 
     while (xl9555_key_scan(0) != KEY0_PRES)
     {
         vTaskDelay(pdMS_TO_TICKS(10));
     }
 
     /* 第七步：列表末尾添加列表项2 */
     ESP_LOGI("freertos_demo", "/****************第七步: 列表末尾添加列表项2****************/");
     vListInsertEnd((List_t*     )&TestList,     /* 列表 */
                    (ListItem_t* )&ListItem2);   /* 列表项 */
     ESP_LOGI("freertos_demo", "项目\t\t\t\t地址");
     ESP_LOGI("freertos_demo", "TestList->pxIndex\t\t0x%p", TestList.pxIndex);
     ESP_LOGI("freertos_demo", "TestList->xListEnd->pxNext\t0x%p", (TestList.xListEnd.pxNext));
     ESP_LOGI("freertos_demo", "ListItem1->pxNext\t\t0x%p", (ListItem1.pxNext));
     ESP_LOGI("freertos_demo", "ListItem2->pxNext\t\t0x%p", (ListItem2.pxNext));
     ESP_LOGI("freertos_demo", "ListItem3->pxNext\t\t0x%p", (ListItem3.pxNext));
     ESP_LOGI("freertos_demo", "TestList->xListEnd->pxPrevious\t0x%p", (TestList.xListEnd.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem1->pxPrevious\t\t0x%p", (ListItem1.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem2->pxPrevious\t\t0x%p", (ListItem2.pxPrevious));
     ESP_LOGI("freertos_demo", "ListItem3->pxPrevious\t\t0x%p", (ListItem3.pxPrevious));
     ESP_LOGI("freertos_demo", "/************************实验结束***************************/");
     
     while(1)
     {
         LED0_TOGGLE();
         vTaskDelay(pdMS_TO_TICKS(1000));
     }
 }
 
 