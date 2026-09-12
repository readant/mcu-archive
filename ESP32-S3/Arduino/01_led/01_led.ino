#include "led.h"


void setup() {
  led_init();     // LED 初始化

}

void loop() {
  LED(0);         // 
  delay(500);
  LED(1);
  delay(500);
}
