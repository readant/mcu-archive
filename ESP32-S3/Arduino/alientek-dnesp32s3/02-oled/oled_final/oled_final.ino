/*
 * 正点原子 DNESP32S3 + 0.96 OLED(SSD1306 128x64, I2C 地址 0x3C) 正式版
 * 库: U8g2(软件 I2C, 引脚可任意映射)
 *
 * 【底板引脚事实(已用 i2c_finder 实测核对)】
 *   底部黄色排针上丝印 SCL/SDA 的针 = 摄像头扩展总线 = GPIO38/39,
 *   按丝印把 OLED 接在这里是正确接法(本程序默认)。
 *   GPIO41/42 是板内主 I2C(挂 24C02/XL9555/AP3216C), 没有引到这组排针。
 *
 * 接线:
 *   OLED GND -> 排针 GND
 *   OLED VCC -> 排针 3V3
 *   OLED SCL -> 丝印 SCL (GPIO38)
 *   OLED SDA -> 丝印 SDA (GPIO39)
 *
 * 若以后插 OV2640 摄像头占用 38/39, 把 OLED 挪到任意空闲 IO
 * (例如 P1 的 IO5/IO7), 改下面两个宏并相应换线即可。
 */

#include <U8g2lib.h>

#define SCL_PIN 38   // 底部排针丝印 SCL
#define SDA_PIN 39   // 底部排针丝印 SDA

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL_PIN, SDA_PIN, U8X8_PIN_NONE);

unsigned long counter = 0;

void setup() {
  Serial.begin(115200);
  u8g2.begin();

  // 开机画面
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tr);
  u8g2.drawStr(0, 14, "OLED connected!");
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.drawStr(0, 34, "SCL=38 SDA=39");
  u8g2.drawStr(0, 50, "addr 0x3C SSD1306");
  u8g2.drawHLine(0, 56, 128);
  u8g2.sendBuffer();
  delay(1500);
}

void loop() {
  char buf[28];
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_7x14B_tr);
  u8g2.drawStr(0, 14, "ESP32-S3 OLED");
  u8g2.setFont(u8g2_font_6x12_tr);

  snprintf(buf, sizeof(buf), "uptime: %lu s", millis() / 1000);
  u8g2.drawStr(0, 34, buf);
  snprintf(buf, sizeof(buf), "count : %lu", counter++);
  u8g2.drawStr(0, 50, buf);

  u8g2.drawFrame(0, 58, 128, 6);                 // 进度条外框
  u8g2.drawBox(1, 59, (counter % 64) * 2, 4);   // 走动的进度条

  u8g2.sendBuffer();
  delay(1000);
}
