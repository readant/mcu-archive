# 正点原子 DNESP32-S3 开发板

## 板子信息

| 项目 | 参数 |
|---|---|
| 厂商 | 正点原子 (ALIENTEK) |
| 型号 | DNESP32-S3 |
| 模组 | ATK-ESP32S3 N16R8 |
| Flash | 16MB |
| PSRAM | 8MB OPI PSRAM |
| 芯片 | ESP32-S3-WROOM-1 |

## Arduino IDE 设置

- 开发板：ESP32S3 Dev Module
- Flash Size：16MB (128Mb)
- PSRAM：OPI PSRAM
- Partition Scheme：16M Flash (3MB APP/9.9MB FATFS)
- 上传速度：921600

## 板载资源

- 音频：NS4168 I2S 功放
- 用户 LED：GPIO1
- 不可用 GPIO：35/36/37(PSRAM)、43/44(UART)、45/46(strapping)、1(LED)、38/39(触摸/摄像头I2C)

---

## 目录结构

```
alientek-dnesp32s3/
├── 01-basic/           # 基础外设
│   ├── 00_hello_world/ # 串口打印
│   ├── 01_led/         # LED 闪烁
│   └── 02_key/         # 按键输入
├── 02-oled/            # OLED 屏
│   ├── oled_final/     # OLED 最终版
│   ├── oled_learning/  # OLED 学习实验
│   └── i2c_scanner/    # I2C 地址扫描工具
├── 03-tft/             # TFT 彩屏
│   ├── TFT1/           # 粒子动画 demo
│   ├── TFT_Audio_Demo/ # 音画同步
│   └── TFT_WiFi_Stream/ ⭐ # WiFi 视频流
└── README.md
```

---

## TFT 彩屏模块

### 硬件信息
- 屏幕：2.0寸 XYTFT-2.0'-SLCM ST7789 SPI 彩屏
- 分辨率：240×320

### 完整接线表

电源从 **VOUT1** 排针取 3.3V / GND：

| 屏幕引脚 (8针) | 开发板排针位置 | ESP32 GPIO | 说明 |
|---|---|---|---|
| VCC | VOUT1 - 3V3 | 3.3V | 电源正极 |
| GND | VOUT1 - GND | GND | 电源地 |
| SCL (SCK) | P1黄色排针 | GPIO18 | SPI 时钟线 |
| SDA (MOSI) | P1黄色排针 | GPIO16 | SPI 数据线 |
| DC (RS) | P1黄色排针 | GPIO7 | 数据/命令选择 |
| CS | P1黄色排针 | GPIO10 | SPI 片选 |
| RST | 悬空 | - | 不接硬件复位 |
| BL | VOUT1 - 3V3 | 3.3V | 背光常亮 |

> 注：P1黄色长排针在开发板顶部，电源在右侧 VOUT1 排针。

### TFT_eSPI 库配置
库装在 `../libraries/TFT_eSPI/`（该目录不入库，需自行安装），改其中的 `User_Setup.h`：
```cpp
#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 320
#define TFT_RGB_ORDER TFT_RGB
#define TFT_INVERSION_ON
#define TFT_MOSI 16
#define TFT_SCLK 18
#define TFT_CS   10
#define TFT_DC   7
#define TFT_RST  -1
#define TFT_BL   -1
#define SPI_FREQUENCY  40000000
#define USE_HSPI_PORT
```

---

## I2S 音频模块

板载 NS4168 功放，引脚固定：

| 信号 | ESP32 GPIO |
|---|---|
| MCLK | GPIO3 |
| BCLK | GPIO46 |
| LRCK | GPIO9 |
| DOUT | GPIO14 |

---

## 各模块项目说明

### 01-basic 基础外设
| 项目 | 说明 |
|---|---|
| 00_hello_world | 串口打印 Hello World |
| 01_led | 板载 LED 闪烁 |
| 02_key | 按键输入测试 |

### 02-oled OLED 屏
| 项目 | 说明 |
|---|---|
| oled_final | OLED 屏最终版本 |
| oled_learning | OLED 学习笔记/实验 |
| i2c_scanner | I2C 地址扫描调试工具 |

### 03-tft TFT 彩屏
| 项目 | 说明 |
|---|---|
| TFT1 | 粒子动画 demo，测试屏幕基本功能 |
| TFT_Audio_Demo | TFT 显示 + I2S 音画同步播放 |
| **TFT_WiFi_Stream** ⭐ | WiFi 实时视频流 / 图片 / GIF 显示 |

#### TFT_WiFi_Stream 使用方法
1. 烧录 `TFT_WiFi_Stream.ino`，屏幕显示板子IP
2. 电脑连同一个WiFi
3. 运行对应 Python 脚本：
   - `send_video.py` - 播放视频
   - `send_image.py` - 显示静态图片
   - `send_gif.py` - 播放GIF动图

Python 依赖：
```
pip install opencv-python requests imageio numpy
```

---

## 依赖库

在 Arduino IDE → `项目 → 加载库 → 管理库` 中安装（`libraries/` 不随仓库收录）：

- **TFT_eSPI** — TFT 屏幕驱动（**必须按上方配置改 `User_Setup.h`**）
- **TJpg_Decoder** — JPEG 图片解码（`TFT_WiFi_Stream` 收图用）
- **U8g2** — OLED 字库与图形（`02-oled/*` 用）
