/*
 * ST7789 + 喇叭 音画同步 Demo
 * 播放简单旋律，屏幕同步跳动
 *
 * 接线:
 *   TFT:  DC=GPIO7, SCK=GPIO18, SDA=GPIO16, CS=GPIO10
 *   音频: MCLK=GPIO3, BCLK=GPIO46, LRCK=GPIO9, DOUT=GPIO14 (板载NS4168功放)
 */

#include <TFT_eSPI.h>
#include <driver/i2s.h>

// ===== TFT =====
TFT_eSPI tft = TFT_eSPI();

// ===== I2S =====
#define I2S_PORT      I2S_NUM_0
#define I2S_BCLK      46
#define I2S_LRCK      9
#define I2S_DOUT      14
#define I2S_MCLK      3
#define SAMPLE_RATE   16000

// ===== 音符频率 =====
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

// 旋律: (频率, 时长ms)
struct Note { int freq; int dur; };
Note melody[] = {
  {NOTE_C4, 200}, {NOTE_D4, 200}, {NOTE_E4, 200}, {NOTE_F4, 200},
  {NOTE_G4, 200}, {NOTE_A4, 200}, {NOTE_B4, 200}, {NOTE_C5, 400},
  {0, 200},
  {NOTE_C5, 200}, {NOTE_B4, 200}, {NOTE_A4, 200}, {NOTE_G4, 200},
  {NOTE_F4, 200}, {NOTE_E4, 200}, {NOTE_D4, 200}, {NOTE_C4, 400},
  {0, 500},
};
int melodyLen = sizeof(melody) / sizeof(melody[0]);

// ===== 波形生成 =====
int samplePos = 0;

void initI2S() {
  i2s_config_t i2s_config;
  i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  i2s_config.sample_rate = SAMPLE_RATE;
  i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  i2s_config.dma_buf_count = 8;
  i2s_config.dma_buf_len = 256;
  i2s_config.use_apll = false;
  i2s_config.tx_desc_auto_clear = true;
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  i2s_pin_config_t pins;
  pins.bck_io_num = I2S_BCLK;
  pins.ws_io_num = I2S_LRCK;
  pins.data_out_num = I2S_DOUT;
  pins.data_in_num = I2S_PIN_NO_CHANGE;
  pins.mck_io_num = I2S_MCLK;
  i2s_set_pin(I2S_PORT, &pins);
}

// 播放一个音符
void playNote(int freq, int durMs) {
  int totalSamples = SAMPLE_RATE * durMs / 1000;
  int phase = 0;

  for (int i = 0; i < totalSamples; i++) {
    float t = (float)i / totalSamples;
    float ampEnv = sin(t * PI);

    int16_t sample = 0;
    if (freq > 0) {
      phase += freq * 256 / SAMPLE_RATE;
      float s = sin((phase >> 8) * 2 * PI / 256.0);
      sample = (int16_t)(s * ampEnv * 8000);
    }

    size_t bytesWritten;
    i2s_write(I2S_PORT, &sample, 2, &bytesWritten, portMAX_DELAY);

    // 每200个样本更新屏幕振幅
    if (i % 200 == 0) {
      int barH = (int)(ampEnv * 180);
      tft.fillRect(0, 230, 240, 90, TFT_BLACK);
      tft.fillRect(100, 230 - barH, 40, barH, TFT_RED);
      tft.drawRect(100, 230 - barH, 40, barH, TFT_WHITE);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Audio + TFT Demo");

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  initI2S();

  // 标题
  tft.fillRect(0, 0, 240, 30, TFT_NAVY);
  tft.setTextColor(TFT_WHITE, TFT_NAVY);
  tft.setTextSize(2);
  tft.setCursor(8, 6);
  tft.print("MUSIC BOX");
}

void loop() {
  for (int n = 0; n < melodyLen; n++) {
    int freq = melody[n].freq;
    int dur = melody[n].dur;

    // 显示音符频率
    tft.fillRect(0, 35, 240, 180, TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(60, 60);
    if (freq > 0) {
      tft.printf("%dHz", freq);
    } else {
      tft.print("---");
    }

    playNote(freq, dur);
  }

  delay(500);
}
