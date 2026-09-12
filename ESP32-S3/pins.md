# 正点原子 DNESP32S3 板载引脚速查表

> 适用于 `ESP32-S3/ESP-IDF-v5.3.x` 与 `ESP32-S3/Arduino` 下的例程。  
> 数据来源：正点原子《DNESP32S3 硬件参考手册》、ESP32-S3 使用指南 IDF 版 V1.6，以及仓库内各 BSP 头文件。

---

## 一、通用 GPIO

| 功能 | GPIO | 说明 |
| :--- | :--- | :--- |
| 用户 LED | **GPIO 1** | 板载 LED，低电平点亮（代码里一般高电平也点亮，取决于宏） |
| BOOT 按键 | **GPIO 0** | 下载/BOOT 按键；Arduino 例程里把它当作独立按键用 |
| XL9555 INT | **GPIO 0** | 扩展 IO 芯片中断输出，需用 P5 上的 `IIC_INT` 跳线帽短接到 GPIO0 |

> ⚠️ **GPIO 0 是 strapping 引脚**：上电瞬间的电平决定启动模式，平时只用作输入（BOOT/INT）没问题，但不要在启动阶段把它强拉为输出高。

---

## 二、I2C

### I2C0 —— 板载外设总线（XL9555 / ES8388 / 触摸屏等）

| 信号 | GPIO | 说明 |
| :--- | :--- | :--- |
| SDA | **GPIO 41** | I2C0_SDA |
| SCL | **GPIO 42** | I2C0_SCL |

挂载设备：
- `XL9555` 扩展 IO（地址 `0x20`）
- `ES8388` 音频 Codec（I2C 控制）

### I2C1 —— 摄像头 SCCB / 电容触摸

| 信号 | GPIO | 说明 |
| :--- | :--- | :--- |
| SDA | **GPIO 39** | 摄像头 OV_SDA / 触摸 CT_SDA |
| SCL | **GPIO 38** | 摄像头 OV_SCL / 触摸 CT_SCL |

> 摄像头实验里，`esp_camera` 使用 `I2C_NUM_1` 作为 SCCB 总线，引脚由 `myiic.c` 配置为 39/38。

---

## 三、SPI2（SD 卡 + SPILCD 共享）

| 信号 | GPIO | 说明 |
| :--- | :--- | :--- |
| SCK  | **GPIO 12** | SPI2_SCLK |
| MOSI | **GPIO 11** | SPI2_MOSI（SDA） |
| MISO | **GPIO 13** | SPI2_MISO |
| SD CS   | **GPIO 2** | SD 卡片选 |
| LCD CS  | **GPIO 21** | SPILCD 片选 |
| LCD DC  | **GPIO 40** | SPILCD 数据/命令；P5 上需用跳线帽短接 `IO_SET` 与 `LCD_DC` |
| LCD PWR | XL9555 `P1_3` | 由扩展 IO 控制 SPILCD 电源 |
| LCD RST | XL9555 `P1_2` | 由扩展 IO 控制 SPILCD 复位 |
| LCD BL  | XL9555 `P1_0` | 由扩展 IO 控制 SPILCD 背光 |

---

## 四、摄像头 DVP（P2 接口）

正点原子 OV5640/OV2640 模块，8 位 DVP 接口。

| 信号 | GPIO | 说明 |
| :--- | :--- | :--- |
| D0 | **GPIO 4** | 数据位 0 |
| D1 | **GPIO 5** | 数据位 1 |
| D2 | **GPIO 6** | 数据位 2 |
| D3 | **GPIO 7** | 数据位 3 |
| D4 | **GPIO 15** | 数据位 4 |
| D5 | **GPIO 16** | 数据位 5 |
| D6 | **GPIO 17** | 数据位 6 |
| D7 | **GPIO 18** | 数据位 7 |
| VSYNC | **GPIO 47** | 帧同步 |
| HREF  | **GPIO 48** | 行同步 |
| PCLK  | **GPIO 45** | 像素时钟 |
| SDA   | **GPIO 39** | SCCB 数据（同 I2C1_SDA） |
| SCL   | **GPIO 38** | SCCB 时钟（同 I2C1_SCL） |
| PWDN  | XL9555 `P0_4` | 摄像头掉电，扩展 IO 控制 |
| RESET | XL9555 `P0_5` | 摄像头复位，扩展 IO 控制 |
| XCLK  | NC / 模块自带 | 正点原子 OV 模块自带 24M 有源晶振，无需 ESP32 输出 XCLK |

---

## 五、RGB LCD（FPC 接口，与 SPILCD 二选一）

> 这些引脚与摄像头 D0~D3 等复用，同一工程里不能同时接 RGB LCD 和摄像头。

| 信号 | GPIO | 说明 |
| :--- | :--- | :--- |
| DE    | **GPIO 4**  | 数据使能 |
| PCLK  | **GPIO 5**  | 像素时钟 |
| R3    | **GPIO 45** | 红高 3 |
| R4    | **GPIO 48** | 红高 4 |
| R5    | **GPIO 47** | 红高 5 |
| R6    | **GPIO 21** | 红高 6（与 SPILCD_CS 复用） |
| R7    | **GPIO 14** | 红高 7 |
| G2    | **GPIO 10** | 绿高 2 |
| G3    | **GPIO 9**  | 绿高 3 |
| G4    | **GPIO 46** | 绿高 4 |
| G5    | **GPIO 3**  | 绿高 5 |
| G6    | **GPIO 8**  | 绿高 6 |
| G7    | **GPIO 18** | 绿高 7（与摄像头 D7 复用） |
| B3    | **GPIO 17** | 蓝高 3（与摄像头 D6 复用） |
| B4    | **GPIO 16** | 蓝高 4（与摄像头 D5 复用） |
| B5    | **GPIO 15** | 蓝高 5（与摄像头 D4 复用） |
| B6    | **GPIO 7**  | 蓝高 6（与摄像头 D3 复用） |
| B7    | **GPIO 6**  | 蓝高 7（与摄像头 D2 复用） |

---

## 六、电容触摸屏（GT9xxx）

| 信号 | GPIO / 扩展 IO | 说明 |
| :--- | :--- | :--- |
| SDA  | **GPIO 39** | I2C1_SDA |
| SCL  | **GPIO 38** | I2C1_SCL |
| INT  | **GPIO 40** | 触摸中断 |
| RST  | XL9555 `P1_1` (`CT_RST_IO`) | 扩展 IO 控制复位 |

---

## 七、音频 ES8388

| 信号 | GPIO | 说明 |
| :--- | :--- | :--- |
| I2C_SDA | **GPIO 41** | 控制总线 |
| I2C_SCL | **GPIO 42** | 控制总线 |
| I2S_BCK | **GPIO 46** | I2S 位时钟 |
| I2S_WS  | **GPIO 9**  | I2S 字选择 |
| I2S_DO  | **GPIO 10** | I2S 数据输出 |
| I2S_DI  | **GPIO 14** | I2S 数据输入 |
| I2S_MCLK| **GPIO 3**  | I2S 主时钟 |

---

## 八、UART0（USB 转串口）

| 信号 | GPIO | 说明 |
| :--- | :--- | :--- |
| U0RXD | **GPIO 44** | 串口接收 |
| U0TXD | **GPIO 43** | 串口发送 |

---

## 九、XL9555 扩展 IO 位定义

P0 端口：

| 位 | 名称 | 功能 |
| :--- | :--- | :--- |
| P0_0 | `AP_INT_IO`   | AP 中断 |
| P0_1 | `QMA_INT_IO`  | QMA 中断 |
| P0_2 | `SPK_EN_IO`   | 喇叭使能 |
| P0_3 | `BEEP_IO`     | 蜂鸣器 |
| P0_4 | `OV_PWDN_IO`  | 摄像头 PWDN |
| P0_5 | `OV_RESET_IO` | 摄像头 RESET |
| P0_6 | `GBC_LED_IO`  | GBC LED |
| P0_7 | `GBC_KEY_IO`  | GBC 按键 |

P1 端口：

| 位 | 名称 | 功能 |
| :--- | :--- | :--- |
| P1_0 | `LCD_BL_IO`   | SPILCD 背光 |
| P1_1 | `CT_RST_IO`   | 触摸屏复位 |
| P1_2 | `SLCD_RST_IO` | SPILCD 复位 |
| P1_3 | `SLCD_PWR_IO` | SPILCD 电源 |
| P1_4 | `KEY3_IO`     | 按键 KEY3 |
| P1_5 | `KEY2_IO`     | 按键 KEY2 |
| P1_6 | `KEY1_IO`     | 按键 KEY1 |
| P1_7 | `KEY0_IO`     | 按键 KEY0 |

---

## 十、容易踩的坑

1. **XL9555_INT 别写成 GPIO40**：GPIO40 是 SPILCD_DC / 触摸 INT。XL9555 中断在 **GPIO0**，但需要 P5 跳线帽短接 `IIC_INT` 才有效。
2. **SPILCD 不亮先查跳线帽**：P5 上的 `IO_SET` 与 `LCD_DC` 必须短接，否则 DC 信号没连到 GPIO40。
3. **RGB LCD 与摄像头/SD 复用**：接 RGB LCD 时，D0~D3、D4~D7 引脚会冲突，不能同时跑摄像头例程。
4. **I2C1 是摄像头和触摸共享**：跑摄像头或 LVGL 触摸例程时，不要把它再接到别的设备。
5. **BOOT 按键就是 GPIO0**：Arduino `02_key` 里用的 `KEY_PIN 0` 就是这个按键，不是 KEY0~KEY3（KEY0~3 在 XL9555 上）。

---

[← 返回 ESP32-S3 首页](README.md)
