#include <Arduino.h>
#include "ColorUtilsHsi.h"

void setup() {
  Serial.begin(115200);
  delay(3000);

  char msg[80];
  hsiF hsi1;
  rgbF rgb1;
  rgbwF rgbw1;
  rgbPwm rgbPwm1;
  rgbwPwm rgbwPwm1;

  hsi1 = {0.5, 0.5, 0.5};
  rgb1 = Hsi2Rgb(hsi1);
  rgbw1 = Hsi2Rgbw(hsi1);
  rgbPwm1 = Rgb2Pwm(rgb1, 255);
  rgbwPwm1 = Rgbw2Pwm(rgbw1, 255);

  sprintf(msg, "rgb1 = %3.2f %3.2f %3.2f", rgb1.r, rgb1.g, rgb1.b);
  Serial.println(msg);
  sprintf(msg, "rgbw1 = %3.2f %3.2f %3.2f %3.2f", rgbw1.r, rgbw1.g, rgbw1.b, rgbw1.w);
  Serial.println(msg);
  sprintf(msg, "rgbPwm1 = %lu %lu %lu", rgbPwm1.r, rgbPwm1.g, rgbPwm1.b);
  Serial.println(msg);
  sprintf(msg, "rgbwPwm1 = %lu %lu %lu %lu", rgbwPwm1.r, rgbwPwm1.g, rgbwPwm1.b, rgbwPwm1.w);
  Serial.println(msg);

}

void loop() {
}