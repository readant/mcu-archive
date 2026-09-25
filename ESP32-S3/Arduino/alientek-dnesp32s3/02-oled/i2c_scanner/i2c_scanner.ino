/*
 * I2C 双总线扫描工具(正点原子 DNESP32S3 专用)
 * 依次扫描底板上两组 I2C:
 *   外部排针 SCL/SDA = GPIO38/39  -> 你外接 OLED/传感器的排针
 *   板载主 I2C       = GPIO41/42  -> 24C02/XL9555/AP3216C
 * 串口监视器 115200, 每 3 秒扫描一轮, 边插线边看即可。
 */

#include <Wire.h>

struct Bus { const char* name; int sda, scl; };
Bus buses[] = {
  {"外部排针 SCL=38/SDA=39", 39, 38},
  {"板载主I2C SCL=42/SDA=41", 41, 42},
};

byte scanBus(int sda, int scl) {
  Wire.end();
  delay(20);
  Wire.begin(sda, scl);
  delay(20);
  byte n = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("    0x%02X\n", addr);
      n++;
    }
  }
  return n;
}

void setup() {
  Serial.begin(115200);
  delay(800);
}

void loop() {
  Serial.println("\n======== I2C 双总线扫描 ========");
  for (byte i = 0; i < sizeof(buses) / sizeof(buses[0]); i++) {
    Serial.printf("[%s]\n", buses[i].name);
    byte n = scanBus(buses[i].sda, buses[i].scl);
    Serial.printf("    -> %d 个设备%s\n", n, n ? "" : " (空)");
  }
  delay(3000);
}
