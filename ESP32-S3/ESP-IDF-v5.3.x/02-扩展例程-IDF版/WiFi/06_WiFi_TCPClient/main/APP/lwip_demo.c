/**
 ****************************************************************************************************
 * @file        udp.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       LWIP实验
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

 #include "lwip_demo.h"


 /* 需要自己设置远程IP地址 */
 #define IP_ADDR   "192.168.2.152"
 
 #define LWIP_DEMO_RX_BUFSIZE         128                        /* 最大接收数据长度 */
 #define LWIP_DEMO_PORT               8080                       /* 连接的本地端口号 */
 #define LWIP_SEND_THREAD_PRIO       ( tskIDLE_PRIORITY + 3 )    /* 发送数据线程优先级 */
 /* 接收数据缓冲区 */
 char g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE]; 
 
 /* 发送数据内容 */
 uint8_t g_lwip_demo_sendbuf[] = "ALIENTEK DATA \r\n";
 /* 数据发送标志位 */
 uint8_t g_lwip_send_flag;
 int g_sock = -1;
 int g_lwip_connect_state = 0;
 static void lwip_send_thread(void *arg);
 
 
 /**
  * @brief       发送数据线程
  * @param       无
  * @retval      无
  */
 void lwip_data_send(void)
 {
     xTaskCreate(lwip_send_thread, "lwip_send_thread", 4096, NULL, LWIP_SEND_THREAD_PRIO, NULL);
 }
 
 /**
  * @brief       lwip_demo实验入口
  * @param       无
  * @retval      无
  */
 void lwip_demo(void)
 {
     struct sockaddr_in atk_client_addr;
     err_t err;
     int recv_data_len;
     char host_ip[] = IP_ADDR;
     lwip_data_send();                                           /* 创建发送数据线程 */

     while (1)
     {
 sock_start:
         g_lwip_connect_state = 0;
         inet_pton(AF_INET, host_ip, &atk_client_addr.sin_addr);
         atk_client_addr.sin_family = AF_INET;                   /* 表示IPv4网络协议 */
         atk_client_addr.sin_port = htons(LWIP_DEMO_PORT);       /* 端口号 */
         g_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);               /* 可靠数据流交付服务既是TCP协议 */
         memset(&(atk_client_addr.sin_zero), 0, sizeof(atk_client_addr.sin_zero));
         
         /* 连接远程IP地址 */
         err = connect(g_sock, (struct sockaddr *)&atk_client_addr, sizeof(atk_client_addr));
 
         if (err == -1)
         {
             ESP_LOGE("TAG", "Socket unable to connect: errno %d", errno);
             goto sock_start;
         }
 
         g_lwip_connect_state = 1;
         ESP_LOGI("TAG", "Successfully connected");
         while (1)
         {
             recv_data_len = recv(g_sock,g_lwip_demo_recvbuf,
                                 sizeof(g_lwip_demo_recvbuf) - 1,0);
        
             if (recv_data_len < 0)
             {
                 g_lwip_connect_state = 0;
                 ESP_LOGE("TAG", "recv failed: errno %d", errno);
                 break;
             }
             else
             {
                g_lwip_demo_recvbuf[recv_data_len] = 0;
                ESP_LOGI("TAG", "Received %d bytes from %s:", recv_data_len, host_ip);
                ESP_LOGI("TAG", "%s", g_lwip_demo_recvbuf);
            }
        }

        if (g_sock != -1)
        {
            shutdown(g_sock, 0);
            close(g_sock);
        }
     }
 }
 
 /**
  * @brief       发送数据线程函数
  * @param       pvParameters : 传入参数(未用到)
  * @retval      无
  */
 void lwip_send_thread(void *pvParameters)
 {
     pvParameters = pvParameters;
     
     err_t err;
     
     while (1)
     {
         while (1)
         {
             if(((g_lwip_send_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) && (g_lwip_connect_state == 1)) /* 有数据要发送 */
             {
                 err = send(g_sock, g_lwip_demo_sendbuf, sizeof(g_lwip_demo_sendbuf),0);
                 
                 if (err < 0)
                 {
                     break;
                 }
                 
                 g_lwip_send_flag &= ~LWIP_SEND_DATA;
             }
             
             vTaskDelay(pdMS_TO_TICKS(10));
         }
         
         shutdown(g_sock, 0);
         close(g_sock);
     }
 }
 