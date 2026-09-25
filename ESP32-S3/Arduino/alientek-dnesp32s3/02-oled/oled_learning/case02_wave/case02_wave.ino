/*
 * 进阶案例 02: OLED 实时滚动波形(无外部硬件)
 * 知识点:
 *   1) 用数组保存历史采样值, 左移实现"滚动波形", 这是示波器/数据曲线的通用做法
 *   2) millis() 非阻塞定时替代 delay(), 固定帧率(约30fps)不卡死主循环
 *   3) drawLine 逐点连线、网格绘制
 * 以后把 sinf() 换成 ADC 读数/传感器读数, 就是一台迷你示波器。
 */

#include <U8g2lib.h>
#include <math.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 38, 39, U8X8_PIN_NONE);

static const uint8_t W = 128;
static const uint8_t TOP = 16;          // 波形区顶部
static const uint8_t MID = 40;          // 波形中线
uint8_t wave[W];
float phase = 0.0f;
unsigned long lastFrame = 0;

void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x12_tr);
  for (uint8_t i = 0; i < W; i++) wave[i] = MID;
}

void loop() {
  // 非阻塞: 每 33ms 刷新一帧(~30fps)
  if (millis() - lastFrame < 33) return;
  lastFrame = millis();

  // 1) 历史数据整体左移一格
  memmove(wave, wave + 1, W - 1);

  // 2) 采"新样点": 基波 + 二次谐波叠加; 实战换成 analogRead/传感器
  phase += 0.12f;
  float v = 20.0f * sinf(phase) + 6.0f * sinf(phase * 2.7f + 1.0f);
  wave[W - 1] = (uint8_t)constrain(MID + v, TOP + 1, 63);

  // 3) 绘制
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Real-time Wave 30fps");

  // 横向网格线
  u8g2.setDrawColor(1);
  for (uint8_t y = TOP; y <= 60; y += 10) {
    u8g2.drawHLine(0, y, W);
  }
  // 中线加粗
  u8g2.drawHLine(0, MID, W);

  // 波形连线
  for (uint8_t x = 1; x < W; x++) {
    u8g2.drawLine(x - 1, wave[x - 1], x, wave[x]);
  }

  u8g2.sendBuffer();
}
