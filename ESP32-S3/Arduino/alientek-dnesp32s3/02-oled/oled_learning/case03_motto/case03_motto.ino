/*
 * 进阶案例 03: [L君] 朋友圈文案轮播(署名 readant-)
 * 知识点:
 *   1) 中文 UTF-8 的字符计数 / 截断(一个汉字 3 字节, 不能按字节切)
 *   2) 中英混排按像素宽度自动换行 getUTF8Width()
 *   3) millis() 非阻塞状态机: 打字机逐字显示 -> 整句停留 -> 自动下一页
 *   4) 顶栏双字体(中文标题 + 英文 ID)
 * 接线同 oled_final: SCL=38 SDA=39 VCC=3V3 GND=GND
 */

#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 38, 39, U8X8_PIN_NONE);

// ---------------- 顶栏署名 ----------------
const char* TITLE = " L君 ";       // 左上标题
const char* AUTHOR = "readant-";   // 右上 ID

// ---------------- (每句一页, 自动逐字打出) ----------------
const char* const QUOTES[] = {
  "逆流河上不回头，debug 路上不罢休。",
  "仙尊悔而我不悔，代码崩而我不退。",
  "任他 error 漫天，我自执键向前。",
  "宿命天成命中败，千行红标我重来。"
};
const int N = sizeof(QUOTES) / sizeof(QUOTES[0]);

// ---------------- 排版与节奏参数 ----------------
const int   BODY_X = 4, BODY_Y = 40, BODY_W = 120;
const int   FONT_H = 13, MAX_ROWS = 3;
const unsigned long TYPE_MS = 110;   // 打字机: 每个字符间隔(ms)
const unsigned long HOLD_MS = 3600;  // 整句打完后停留(ms)
const unsigned long FRAME_MS = 33;   // 约 30fps 刷新节流

int   idx = 0;            // 当前第几页
int   shown = 0;          // 当前已显示的字符数
unsigned long lastStep = 0, fullAt = 0, lastDraw = 0;

// ================= UTF-8 工具函数 =================
int utf8CharLen(const char* p) {
  byte b = (byte)*p;
  if (b < 0x80) return 1;
  if ((b >> 5) == 0x6) return 2;
  if ((b >> 4) == 0xE) return 3;
  return 4;
}
int utf8Count(const char* s) {
  int c = 0;
  while (*s) { if (((byte)*s & 0xC0) != 0x80) c++; s++; }
  return c;
}
void utf8Prefix(const char* s, int n, char* out) {
  int c = 0;
  while (*s && c < n) {
    int L = utf8CharLen(s);
    for (int i = 0; i < L; i++) *out++ = *s++;
    c++;
  }
  *out = 0;
}

// 按像素宽度自动换行并绘制
int drawWrapped(const char* s) {
  char line[40]; int li = 0, row = 0;
  const char* p = s;
  while (*p && row < MAX_ROWS) {
    int L = utf8CharLen(p);
    memcpy(line + li, p, L);
    line[li + L] = 0;
    if (u8g2.getUTF8Width(line) > BODY_W && li > 0) {
      line[li] = 0;
      u8g2.drawUTF8(BODY_X, BODY_Y + row * FONT_H, line);
      row++;
      memcpy(line, p, L); line[L] = 0; li = L;
    } else {
      li += L;
    }
    p += L;
  }
  if (li > 0 && row < MAX_ROWS) {
    line[li] = 0;
    u8g2.drawUTF8(BODY_X, BODY_Y + row * FONT_H, line);
    row++;
  }
  return row;
}

// ================= 画面渲染 =================
void render(int total) {
  char text[80];
  u8g2.clearBuffer();

  // 顶栏: 左 [L君](中文) / 右 readant-(英文小字体)
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.drawUTF8(4, 11, TITLE);
  u8g2.setFont(u8g2_font_6x12_tr);
  int aw = u8g2.getUTF8Width(AUTHOR);
  u8g2.drawStr(124 - aw, 11, AUTHOR);
  u8g2.drawHLine(0, 15, 128);

  // 正文(切回中文字体, 兼容中英混排)
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  utf8Prefix(QUOTES[idx], shown, text);
  drawWrapped(text);

  // 打字机小方块光标
  if (shown < total) u8g2.drawBox(120, 54, 5, 5);

  u8g2.sendBuffer();
}

void setup() {
  u8g2.begin();
}

void loop() {
  unsigned long now = millis();
  int total = utf8Count(QUOTES[idx]);

  if (shown < total) {                       // 阶段1: 逐字打出
    if (now - lastStep >= TYPE_MS) { shown++; lastStep = now; }
    fullAt = now;
  } else {                                   // 阶段2: 停留后翻页
    if (now - fullAt >= HOLD_MS) {
      idx = (idx + 1) % N;
      shown = 0;
      fullAt = now;
      lastStep = now;
    }
  }

  if (now - lastDraw >= FRAME_MS) {
    lastDraw = now;
    render(total);
  }
}
