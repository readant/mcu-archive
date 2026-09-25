/*
 * TFT 坦克大战
 * ESP32-S3 + TFT 彩屏 + 红外遥控器
 *
 * 接线（沿用本目录 TFT_WiFi_Stream 的屏配置）：
 *   DC=GPIO7, SCK=GPIO18, SDA=GPIO16, CS=GPIO10
 *
 * 红外接收头引脚：见下面 IR_RECV_PIN，改成你板子实际接的 GPIO
 * 遥控器键值：先跑"按键测试"模式，按方向键和开火键，串口会打印键值，
 *             把打印出来的值填到下面 KEY_xxx 常量里
 */

#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// ====================== 用户配置区 ======================

// 红外接收头 GPIO（正点原子 ESP32-S3 一般是 RX 引脚对应的 GPIO，查原理图）
#define IR_RECV_PIN   2

// 遥控器键值 —— 先跑下面的"按键测试模式"（TEST_MODE 改成 1），
// 按遥控器上的键，串口监视器会打印键值，把打印的十六进制填到这里
#define TEST_MODE     0   // 1=只测按键不打游戏，0=正式游戏

#define KEY_UP        0xFF629D   // ↑  改成你的
#define KEY_DOWN      0xFFA857   // ↓
#define KEY_LEFT      0xFF22DD   // ←
#define KEY_RIGHT     0xFFC23D   // →
#define KEY_FIRE      0xFF02FD   // OK / 开火键

// ====================== 硬件初始化 ======================

TFT_eSPI tft = TFT_eSPI();
IRrecv irrecv(IR_RECV_PIN);
decode_results irResults;

const int SCREEN_W = 240;
const int SCREEN_H = 320;
const int CELL = 12;                 // 每格像素
const int GRID_W = SCREEN_W / CELL;  // 20
const int GRID_H = SCREEN_H / CELL;  // 26

// ====================== 游戏实体 ======================

enum Dir { D_UP, D_DOWN, D_LEFT, D_RIGHT };

struct Tank {
  int x, y;          // 像素坐标
  Dir dir;
  bool alive;
  uint16_t color;    // 坦克颜色
  unsigned long lastMove;
  unsigned long lastShot;
};

struct Bullet {
  int x, y;
  Dir dir;
  bool alive;
};

// 砖墙：每格一个 bool，true=有砖
bool walls[GRID_W][GRID_H];

Tank player;
Tank enemies[3];
Bullet bullets[10];   // 子弹池（玩家+敌人共用）

int score = 0;
int lives = 3;
bool gameOver = false;
bool started = false;
unsigned long lastEnemySpawn = 0;

// ====================== 初始化地图 ======================

void initWalls() {
  memset(walls, 0, sizeof(walls));
  // 画几堵砖墙当掩体（格子坐标）
  int layout[][2] = {
    {5,8}, {6,8}, {7,8},
    {12,8}, {13,8}, {14,8},
    {3,14}, {4,14},
    {15,14}, {16,14},
    {8,18}, {9,18}, {10,18},
    {2,20}, {17,20},
  };
  for (auto& p : layout) walls[p[0]][p[1]] = true;
}

void resetGame() {
  score = 0;
  lives = 3;
  gameOver = false;
  initWalls();

  player.x = SCREEN_W/2 - CELL/2;
  player.y = SCREEN_H - CELL - 4;
  player.dir = D_UP;
  player.alive = true;
  player.lastMove = 0;
  player.lastShot = 0;

  for (auto& e : enemies) { e.alive = false; }
  for (auto& b : bullets) { b.alive = false; }

  tft.fillScreen(TFT_BLACK);
}

// ====================== 工具函数 ======================

bool wallAtPixel(int px, int py) {
  int gx = px / CELL;
  int gy = py / CELL;
  if (gx < 0 || gx >= GRID_W || gy < 0 || gy >= GRID_H) return true; // 边界当墙
  return walls[gx][gy];
}

// 两个矩形是否相交
bool rectHit(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
  return (x1 < x2+w2 && x1+w1 > x2 && y1 < y2+h2 && y1+h1 > y2);
}

void fireBullet(Tank& t) {
  for (auto& b : bullets) {
    if (!b.alive) {
      b.alive = true;
      b.dir = t.dir;
      switch (t.dir) {
        case D_UP:    b.x = t.x + CELL/2 - 1; b.y = t.y - 3; break;
        case D_DOWN:  b.x = t.x + CELL/2 - 1; b.y = t.y + CELL; break;
        case D_LEFT:  b.x = t.x - 3; b.y = t.y + CELL/2 - 1; break;
        case D_RIGHT: b.x = t.x + CELL; b.y = t.y + CELL/2 - 1; break;
      }
      return;
    }
  }
}

// ====================== 绘制 ======================

void drawExplosion(int x, int y) {
  tft.fillRect(x-2, y-2, CELL+4, CELL+4, TFT_WHITE);
  delay(50);
  tft.fillRect(x, y, CELL, CELL, TFT_YELLOW);
  delay(50);
  tft.fillRect(x, y, CELL, CELL, TFT_BLACK);
}

void drawTank(Tank& t, uint16_t color) {
  // 车身
  tft.fillRect(t.x+1, t.y+1, CELL-2, CELL-2, color);
  // 履带（左右两条深色边）
  tft.fillRect(t.x, t.y, 2, CELL, TFT_DARKGREY);
  tft.fillRect(t.x+CELL-2, t.y, 2, CELL, TFT_DARKGREY);
  // 炮管
  switch (t.dir) {
    case D_UP:    tft.fillRect(t.x+CELL/2-1, t.y-3, 2, 5, color); break;
    case D_DOWN:  tft.fillRect(t.x+CELL/2-1, t.y+CELL-2, 2, 5, color); break;
    case D_LEFT:  tft.fillRect(t.x-3, t.y+CELL/2-1, 5, 2, color); break;
    case D_RIGHT: tft.fillRect(t.x+CELL-1, t.y+CELL/2-1, 5, 2, color); break;
  }
}

void eraseTank(Tank& t) {
  tft.fillRect(t.x-3, t.y-3, CELL+6, CELL+6, TFT_BLACK);
}

void drawWalls() {
  for (int gx=0; gx<GRID_W; gx++)
    for (int gy=0; gy<GRID_H; gy++)
      if (walls[gx][gy])
        tft.fillRect(gx*CELL+1, gy*CELL+1, CELL-2, CELL-2, TFT_RED);
}

void drawHUD() {
  tft.fillRect(0, 0, SCREEN_W, 12, TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(2, 2);
  tft.printf("SCORE:%d  LIVES:%d", score, lives);
}

// ====================== 游戏逻辑 ======================

void movePlayer(Dir d) {
  eraseTank(player);
  player.dir = d;
  int nx = player.x, ny = player.y;
  switch (d) {
    case D_UP:    ny -= CELL; break;
    case D_DOWN:  ny += CELL; break;
    case D_LEFT:  nx -= CELL; break;
    case D_RIGHT: nx += CELL; break;
  }
  // 碰撞检测
  if (nx < 0 || nx > SCREEN_W-CELL || ny < 12 || ny > SCREEN_H-CELL) {
    // 越界，不动
  } else if (!wallAtPixel(nx+2, ny+2) && !wallAtPixel(nx+CELL-2, ny+CELL-2)
          && !wallAtPixel(nx+2, ny+CELL-2) && !wallAtPixel(nx+CELL-2, ny+2)) {
    player.x = nx; player.y = ny;
  }
  drawTank(player, TFT_CYAN);
}

void spawnEnemy() {
  for (auto& e : enemies) {
    if (!e.alive) {
      e.alive = true;
      e.x = random(0, SCREEN_W-CELL);
      e.y = 14;
      e.dir = (Dir)random(0,4);
      e.lastMove = 0;
      e.lastShot = 0;
      // 1/3 概率刷出品红色快速敌人
      e.color = (random(0,3)==0) ? TFT_MAGENTA : TFT_YELLOW;
      drawTank(e, e.color);
      return;
    }
  }
}

void updateEnemies() {
  unsigned long now = millis();
  for (auto& e : enemies) {
    if (!e.alive) continue;
    eraseTank(e);
    // 敌人每 500ms 动一格
    if (now - e.lastMove > 500) {
      e.lastMove = now;
      if (random(0,5) == 0) e.dir = (Dir)random(0,4);  // 偶尔转向
      int nx=e.x, ny=e.y;
      switch (e.dir) {
        case D_UP: ny -= CELL; break;
        case D_DOWN: ny += CELL; break;
        case D_LEFT: nx -= CELL; break;
        case D_RIGHT: nx += CELL; break;
      }
      if (nx<0 || nx>SCREEN_W-CELL || ny<12 || ny>SCREEN_H-CELL || wallAtPixel(nx+2,ny+2)) {
        e.dir = (Dir)random(0,4);  // 撞墙就换向
      } else {
        e.x = nx; e.y = ny;
      }
      // 敌人偶尔开火
      if (random(0,20)==0) fireBullet(e);
    }
    drawTank(e, e.color);
  }
  // 每 5 秒刷一个敌人
  if (now - lastEnemySpawn > 5000) {
    lastEnemySpawn = now;
    spawnEnemy();
  }
}

void updateBullets() {
  for (auto& b : bullets) {
    if (!b.alive) continue;
    // 擦除旧位置
    tft.fillRect(b.x, b.y, 2, 4, TFT_BLACK);
    // 移动
    switch (b.dir) {
      case D_UP: b.y -= 4; break;
      case D_DOWN: b.y += 4; break;
      case D_LEFT: b.x -= 4; break;
      case D_RIGHT: b.x += 4; break;
    }
    // 出界
    if (b.x<0||b.x>SCREEN_W||b.y<0||b.y>SCREEN_H) { b.alive=false; continue; }
    // 打砖墙
    int gx=b.x/CELL, gy=b.y/CELL;
    if (gx>=0&&gx<GRID_W&&gy>=0&&gy<GRID_H&&walls[gx][gy]) {
      walls[gx][gy]=false;
      tft.fillRect(gx*CELL+1, gy*CELL+1, CELL-2, CELL-2, TFT_BLACK);
      b.alive=false; continue;
    }
    // 打玩家
    if (rectHit(b.x,b.y,2,4, player.x,player.y,CELL,CELL)) {
      b.alive=false;
      lives--;
      // 闪红反馈
      tft.fillRect(player.x-3, player.y-3, CELL+6, CELL+6, TFT_RED);
      delay(80);
      tft.fillRect(player.x-3, player.y-3, CELL+6, CELL+6, TFT_BLACK);
      drawTank(player, TFT_CYAN);
      drawHUD();
      if (lives<=0) { gameOver=true; }
      continue;
    }
    // 打敌人
    for (auto& e : enemies) {
      if (e.alive && rectHit(b.x,b.y,2,4, e.x,e.y,CELL,CELL)) {
        e.alive=false;
        b.alive=false;
        score+=100;
        drawExplosion(e.x, e.y);
        drawHUD();
      }
    }
    // 画新子弹
    tft.fillRect(b.x, b.y, 2, 4, TFT_WHITE);
  }
}

// ====================== setup / loop ======================

void setup() {
  Serial.begin(115200);
  delay(200);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  irrecv.enableIRIn();

  if (TEST_MODE) {
    tft.setCursor(10, 10);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.println("IR Key Test");
    tft.println("See Serial");
  } else {
    // 开始界面
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(45, 40);
    tft.println("TANK");
    tft.println("BATTLE");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(15, 130);
    tft.println("Arrows: Move");
    tft.println("OK: Fire");
    tft.println("");
    tft.println("Green=You  Yellow=Enemy");
    tft.println("Red tank = fast!");
    tft.println("Shoot bricks for cover");
    tft.println("+100 per kill");
    tft.println("3 lives. Don't get hit!");
    tft.setCursor(50, 240);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Press OK to Start");
  }
}

void loop() {
  if (TEST_MODE) {
    if (irrecv.decode(&irResults)) {
      Serial.printf("Key: 0x%06llX\n", (unsigned long long)irResults.value);
      irrecv.resume();
    }
    delay(10);
    return;
  }

  if (gameOver) {
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(40, 120);
    tft.println("GAME OVER");
    tft.setTextSize(2);
    tft.setCursor(50, 160);
    tft.printf("SCORE:%d", score);
    while(1) delay(1000);
  }

  // 读红外
  if (irrecv.decode(&irResults)) {
    unsigned long key = irResults.value;
    if (!started) {
      if (key == KEY_FIRE) { resetGame(); started = true; }
      irrecv.resume();
      return;
    }
    if (key == KEY_UP)         movePlayer(D_UP);
    else if (key == KEY_DOWN)  movePlayer(D_DOWN);
    else if (key == KEY_LEFT)  movePlayer(D_LEFT);
    else if (key == KEY_RIGHT) movePlayer(D_RIGHT);
    else if (key == KEY_FIRE)  fireBullet(player);
    irrecv.resume();
  }

  if (!started) { delay(20); return; }

  updateEnemies();
  updateBullets();
  delay(20);
}
