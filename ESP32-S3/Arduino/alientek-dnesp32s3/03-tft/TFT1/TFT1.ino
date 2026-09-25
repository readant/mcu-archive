/*
 * ST7789 240x320 图片/动画演示
 * 用 TFT_eSPI Sprite 双缓冲，做流畅动画
 * 接线: DC=GPIO7, SCK=GPIO18, SDA=GPIO16, CS=GPIO10
 */

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);  // 双缓冲精灵

// 粒子状态
#define NUM_PARTICLES 60
int8_t px[NUM_PARTICLES], py[NUM_PARTICLES];
int8_t pvx[NUM_PARTICLES], pvy[NUM_PARTICLES];
uint16_t pcolor[NUM_PARTICLES];

unsigned long frameCount = 0;
unsigned long fpsTimer = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  // 创建全屏 sprite（用 PSRAM 加速，否则用 RAM）
  sprite.createSprite(240, 320);

  // 初始化粒子
  randomSeed(analogRead(0));
  for (int i = 0; i < NUM_PARTICLES; i++) {
    px[i] = random(20, 220);
    py[i] = random(40, 300);
    pvx[i] = random(-3, 4);
    pvy[i] = random(-3, 4);
    pcolor[i] = tft.color565(random(0,255), random(50,200), random(100,255));
  }

  fpsTimer = millis();
}

void loop() {
  // ===== 1. 彩虹背景渐变 =====
  for (int y = 0; y < 320; y += 2) {
    uint16_t c = tft.color565(
      (y + frameCount) % 256,
      128,
      255 - ((y + frameCount) % 256)
    );
    sprite.drawFastHLine(0, y, 240, c);
    sprite.drawFastHLine(0, y+1, 240, c);
  }

  // ===== 2. 粒子动画 =====
  for (int i = 0; i < NUM_PARTICLES; i++) {
    // 擦除旧位置
    sprite.fillCircle(px[i], py[i], 3, TFT_BLACK);

    // 更新位置
    px[i] += pvx[i];
    py[i] += pvy[i];

    // 边界反弹
    if (px[i] < 5 || px[i] > 235) pvx[i] = -pvx[i];
    if (py[i] < 35 || py[i] > 315) pvy[i] = -pvy[i];

    // 画新位置
    sprite.fillCircle(px[i], py[i], 3, pcolor[i]);
  }

  // ===== 3. 顶部 HUD =====
  sprite.fillRect(0, 0, 240, 28, TFT_BLACK);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.setTextSize(2);
  sprite.setCursor(8, 6);
  sprite.print("PARTICLE");

  // FPS
  char buf[16];
  snprintf(buf, sizeof(buf), "%.1fFPS",
           frameCount * 1000.0 / (millis() - fpsTimer + 1));
  sprite.setTextColor(TFT_CYAN, TFT_BLACK);
  sprite.setTextSize(2);
  sprite.setCursor(150, 6);
  sprite.print(buf);

  // ===== 4. 绘制到屏幕 =====
  sprite.pushSprite(0, 0);

  // ===== FPS 统计 =====
  frameCount++;
  if (millis() - fpsTimer >= 1000) {
    Serial.printf("FPS: %.1f, particles: %d\n",
                  frameCount, NUM_PARTICLES);
    frameCount = 0;
    fpsTimer = millis();
  }

  delay(16);  // ~60fps
}
