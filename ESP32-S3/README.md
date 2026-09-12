# ESP32-S3 · 万物互联的起点

> *从控制一颗 LED，到连接整个世界。*

[← 返回首页](../README.md)

> **编码约定：本阶段源码一律 UTF-8**（与 C51/C52 的 GBK 分轨，勿混用）。
> 环境总表见 [根 README](../README.md#-环境与编码约定)。

---

## 一句话现状

正点原子 ESP32-S3 开发板（DNESP32S3）的配套学习仓库，**ESP-IDF v5.3.x 为主，Arduino 为辅**。

- 硬件平台：正点原子 ESP32-S3 开发板
- 主工具链：ESP-IDF v5.3.x（兼容 v5.4，见 [99-资料包/支持5.3,5.4编译.txt](ESP-IDF-v5.3.x/99-资料包/支持5.3,5.4编译.txt)）
- 辅助工具链：Arduino IDE（`ESP32S3 Dev Module`，板载 LED = GPIO48）
- 工程总数：**31 个**（28 个 IDF 工程 + 3 个 Arduino 工程）

---

## 📁 目录结构

```
ESP32-S3/
├── README.md                       ← 本文件
│
├── Arduino/                        ← Arduino 框架（快速原型）
│   ├── 00_hello_world/             串口打印 hello world
│   ├── 01_led/                     GPIO 输出：LED 闪烁（led.cpp / led.h）
│   └── 02_key/                     GPIO 输入：按键检测 + LED（key.cpp / key.h）
│
└── ESP-IDF-v5.3.x/                 ← ESP-IDF 原生开发
    ├── 01-基础例程-camera/          摄像头采集 + LCD 显示（含 esp32-camera 组件）
    ├── 02-扩展例程-IDF版/
    │   ├── FreeRTOS/               18 个 RTOS 实验（见下表）
    │   ├── WiFi/                   9 个网络实验（见下表）
    │   ├── AI/                     ⚠️ 占位目录，源码在 99-资料包/esp-who.zip
    │   └── LVGL/                   1 个 GUI 实验
    ├── 03-小智AI工程/               ⚠️ 占位目录，源码在 99-资料包/xiaozhi-esp32.rar
    └── 99-资料包/                  原始压缩包 + 零散驱动（约 500 MB，见文末说明）
```

---

## 🧵 FreeRTOS 例程（18 个）

目录：`ESP-IDF-v5.3.x/02-扩展例程-IDF版/FreeRTOS/`

| # | 目录 | 内容 |
|:--|:---|:---|
| 01 | `01_Trends_task_creation_and_deletion` | 任务创建与删除（**动态**方法） |
| 02 | `02_Static_task_creation_and_deletion` | 任务创建与删除（**静态**方法） |
| 03 | `03_Task_suspension_and_recovery` | 任务挂起与恢复 |
| 04 | `04_Insertion_and_deletion_list_items` | 列表项的插入与删除 |
| 05 | `05_Time_slice_scheduling` | 时间片调度 |
| 06 | `06_Queue_operation` | 队列操作 |
| 07 | `07_Queue_Set` | 队列集 |
| 08 | `08_Queue_Set_Simulation_Event_Flags` | 队列集模拟事件标志位 |
| 09 | `09_Binary_semaphore_operation` | 二值信号量 |
| 10 | `10_Counting_type_semaphore_operation` | 计数型信号量 |
| 11 | `11_Priority_inversion` | 优先级翻转（经典反例） |
| 12 | `12_Mutex_semaphore` | 互斥信号量（解决优先级翻转） |
| 13 | `13_Software_Timers` | 软件定时器 |
| 14 | `14_Event_flag` | 事件标志组 |
| 15 | `15_Task_notification_simulation_binary_semaphore` | 任务通知模拟二值信号量 |
| 16 | `16_Task_notification_analog_count_semaphore` | 任务通知模拟计数型信号量 |
| 17 | `17_Task_notification_simulation_message_email` | 任务通知模拟消息邮箱 |
| 18 | `18_Task_notification_simulation_event_flag_group` | 任务通知模拟事件标志组 |

> 📌 每个工程的核心逻辑都在 `main/APP/freertos_demo.c`，入口 `main/main.c`。
> 11 → 12 是配套的：先看优先级翻转有多坑，再看互斥量怎么救。

---

## 📡 WiFi 例程（9 个）

目录：`ESP-IDF-v5.3.x/02-扩展例程-IDF版/WiFi/`

| # | 目录 | 内容 | 关键技术 |
|:--|:---|:---|:---|
| 01 | `01_WiFi_SCAN` | WiFi 扫描 | STA 模式 |
| 02 | `02_WiFi_STA` | 连接路由器 | STA 模式 |
| 03 | `03_WiFi_AP` | 创建热点 | AP 模式 |
| 04 | `04_WiFi_SmartConfig` | 一键配网 | SmartConfig / Airkiss |
| 05 | `05_WiFi_UDP` | UDP 通信 | lwIP socket |
| 06 | `06_WiFi_TCPClient` | TCP 客户端 | lwIP socket |
| 07 | `07_WiFi_TCPServer` | TCP 服务器 | lwIP socket |
| 08 | `08_WiFi_MQTT_ALIYUN` | 接入阿里云 IoT | MQTT 协议 |
| 09 | `09_WiFi_CAMERA` | 网络摄像头 | 摄像头 + TCP 推流 |

---

## 🖥️ 其他 IDF 工程

| 目录 | 内容 |
|:---|:---|
| `01-基础例程-camera` | OV 系列摄像头采集 → SPI LCD 实时显示。自带 `esp32-camera` 组件（位于 `components/Middlewares/`） |
| `02-扩展例程-IDF版/LVGL/01_lvgl_transplant` | RGB 接口 LCD 驱动 + LVGL 移植，含触摸（TOUCH）与 XL9555 扩展 IO |

---

## 🛠️ 怎么用

### ESP-IDF 工程（推荐 VS Code + ESP-IDF 插件）

每个工程都是**独立的标准 IDF 工程**，可直接打开编译：

```bash
cd "ESP32-S3/ESP-IDF-v5.3.x/02-扩展例程-IDF版/FreeRTOS/01_Trends_task_creation_and_deletion"
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor      # COMx 换成实际串口
```

工程自带 `.vscode/` 与 `.devcontainer/`，VS Code 打开即用。

**关于 `sdkconfig`**：仓库根目录 `.gitignore` 忽略了 `sdkconfig`（保留 `sdkconfig.defaults`），
所以 clone 下来需要 `idf.py build` 重新生成一次配置。分区表用 `partitions-16MiB.csv`（16MB Flash）。

### Arduino 工程

1. Arduino IDE → 首选项 → 附加开发板管理器网址：
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
2. 开发板管理器搜 `esp32` 并安装，选择 **ESP32S3 Dev Module**
3. 打开 `Arduino/01_led/01_led.ino` 上传（Arduino 要求 `.ino` 文件名与所在目录同名，改名时记得同步）

---

## 📚 从 51 到 ESP32，思维方式的转变

| | 51 单片机 | ESP32-S3 |
|:---|:---|:---|
| 编程模型 | 裸机轮询 | 多任务 / RTOS |
| 资源管理 | 手动配置寄存器 | HAL 库 / 驱动框架 |
| 通信方式 | UART 串口 | WiFi + MQTT + HTTP |
| 开发节奏 | 编译→烧录→看现象 | 编译→烧录→看串口日志 |
| 调试手段 | 看 LED / 数码管 | 串口打印 + 逻辑分析仪 |

**核心转变**：从"控制硬件"到"构建系统"。FreeRTOS 那 18 个实验就是为这个转变准备的。

---

## 🎯 学习进度

### 已完成
- [x] 环境搭建（Arduino IDE + ESP-IDF 双环境）
- [x] GPIO 输入输出（LED / 按键）
- [x] FreeRTOS 全套机制（任务 / 队列 / 信号量 / 事件组 / 任务通知）
- [x] WiFi STA / AP / SmartConfig 配网
- [x] TCP / UDP socket 编程
- [x] MQTT 上云（阿里云 IoT）
- [x] 摄像头采集 + LCD 显示
- [x] LVGL 移植

### 进行中
- [ ] esp-who 人脸识别（源码在 `99-资料包/esp-who.zip`，待解压学习）
- [ ] 小智 AI 语音助手（源码在 `99-资料包/xiaozhi-esp32.rar`，待解压学习）

### 想做的项目
- [ ] 智能环境监测（温湿度 + 气压 + 光照，数据上云）
- [ ] 远程灯控（手机 APP 控制 LED / 继电器）
- [ ] 数据可视化看板

---

## ⚠️ 关于 `99-资料包/`（约 500 MB）

这个目录放的是**厂商原始压缩包**，占仓库体积的 98%，但**不会进 git**（根目录 `.gitignore` 已排除 `*.zip` / `*.rar`）。

| 文件 | 大小 | 说明 |
|:---|:---|:---|
| `4，扩展例程-IDF版.zip` | 170.8 MB | 与已解压的 `02-扩展例程-IDF版/` 内容重复，**可安全删除** |
| `esp-who.zip` | 168.7 MB | Espressif 官方 AI 框架，对应 `02-扩展例程-IDF版/AI/` |
| `xiaozhi-esp32.rar` | 124.5 MB | 小智 AI 语音助手源码，对应 `03-小智AI工程/` |
| `1，标准例程-IDF(v5.3.x).zip` | 36.1 MB | 标准例程原始包（尚未解压学习） |
| `零散驱动/` | — | `myiic` / `xl9555` 驱动副本（各工程 `components/BSP/` 里都有） |

> 💡 **想给仓库瘦身**：删掉 `4，扩展例程-IDF版.zip`（170.8 MB，内容已解压）就能省下三分之一。
> 其余三个是尚未展开的学习资料，建议保留到学完为止。

---

*这一阶段从"点亮一颗 LED"走到了"跑 FreeRTOS + 连 WiFi + 上云"。代码会持续更新。*

[← 返回首页](../README.md)
