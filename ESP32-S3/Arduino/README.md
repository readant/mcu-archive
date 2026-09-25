# ESP32-S3 Arduino 项目集合

本目录存放所有 ESP32-S3 芯片的 Arduino 项目代码，按具体板子型号分文件夹。

## 目录结构

```
Arduino/
├── libraries/                  # 公共库（所有板子共用）
├── alientek-dnesp32s3/         # 正点原子 DNESP32-S3 开发板
│   └── README.md               # 该板子的详细说明
├── [未来其他板子]/             # 比如：espressif-devkitc, custom-board-v1
│   └── README.md
└── README.md                   # 本文件
```

## 命名规范

板子文件夹格式：`厂商-板子型号`
- `alientek-dnesp32s3` — 正点原子 DNESP32-S3
- `espressif-devkitc-1` — 乐鑫官方 DevKitC
- `custom-board-v1.0` — 自画板

## 公共库

`libraries/` 目录是所有板子共用的，不用每个板子单独存。

**注意：`libraries/` 不随仓库收录**（已在 `.gitignore` 中排除，避免第三方库撑爆仓库体积）。
clone 后请在 Arduino IDE → `项目 → 加载库 → 管理库` 里自行安装：

| 库 | 用途 | 用在哪 |
| :--- | :--- | :--- |
| **TFT_eSPI** | ST7789 SPI 彩屏驱动 | `alientek-dnesp32s3/03-tft/*` |
| **TJpg_Decoder** | JPEG 解码 | TFT 视频流 / 图片显示 |
| **U8g2** | OLED 字库与图形 | `alientek-dnesp32s3/02-oled/*` |

> TFT_eSPI 装好后**必须改 `User_Setup.h`**，本板要用的宏已抄录在
> [`alientek-dnesp32s3/README.md`](./alientek-dnesp32s3/README.md#tft_espi-库配置)，
> 照着改一遍再编译，否则花屏/白屏。

## 命名与编码

- 目录结构：`板卡/分组/sketch名/sketch名.ino`（`.ino` 文件名必须与所在目录同名）
- 源码编码：**UTF-8**（与 C51/C52 的 GBK 分轨，勿混用）
