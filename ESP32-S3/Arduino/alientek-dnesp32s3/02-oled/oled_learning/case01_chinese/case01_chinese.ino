/*
 * 进阶案例 01: OLED 中文与多字体显示
 * 知识点:
 *   1) U8g2 内置文泉驿中文字体(wqy), 源码文件必须以 UTF-8 保存(Arduino IDE 默认就是)
 *   2) drawUTF8() 画中文, drawStr() 只适合英文
 *   3) 不同字号混排、字体高度与光标定位
 * 接线: 同 oled_final —— SCL=38 SDA=39 VCC=3V3 GND=GND
 */

#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 38, 39, U8X8_PIN_NONE);

void setup() {
  u8g2.begin();
  u8g2.enableUTF8Print();   // 允许输出 UTF-8(中文)
}

void loop() {
  u8g2.clearBuffer();

  // 大字号标题(16px 文泉驿)
  u8g2.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2.drawUTF8(0, 18, "正点原子");

  u8g2.drawHLine(0, 26, 128);

  // 正文中文字号(12px)
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.drawUTF8(0, 42, "OLED 中文显示");
  u8g2.drawUTF8(0, 58, "嵌入式学习，做中学");

  u8g2.sendBuffer();
  delay(1000);

  // 第二屏: 中英文/数字混排演示
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.drawUTF8(0, 14, "运行时间:");
  u8g2.setFont(u8g2_font_7x14B_tr);
  u8g2.setCursor(64, 14);
  u8g2.printf("%lu s", millis() / 1000);
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.drawUTF8(0, 40, "温度: 25.6 C");
  u8g2.drawUTF8(0, 58, "状态: 运行正常");
  u8g2.sendBuffer();
  delay(1000);
}
