#include "led.h"
#include "key.h"

void setup() {
  led_init();
  key_init();
}

void loop() {
  if (KEY == 0)
  {
    delay(10);
    if (KEY == 0)
    {
      LED(0);
    }
    else
    {
      LED(1);
    }
  }

}
