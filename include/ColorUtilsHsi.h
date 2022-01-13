#include <Arduino.h>


#ifndef _COLOR_TYPES  // prevent duplicate type definitions when this file is included in multiple places
#define _COLOR_TYPES

#define DEFAULT_GAMMA 1.01
#define DEFAULT_RGBW_SCALE_FACTORS {1.0, 1.0, 1.0, 1.0}
#define DEFAULT_RGB_SCALE_FACTORS {1.0, 1.0, 1.0}

/* Struct used to represent HSI colors. Each component value ranges from 0 - 1.
*/
struct hsiF {  
  float h;
  float s;
  float i;
};

/* Struct used to represent RGBW colors after being converted from HSI (hsiF). Each component value ranges from 0 - 1. 
*/
struct rgbwF {  
  float r;
  float g;
  float b;
  float w;
};

/* Struct used to represent RGB colors after being converted from HSI (hsiF). Each component value ranges from 0 - 1. 
*/
struct rgbF {  
  float r;
  float g;
  float b;
};


/* Structs used to represent RGB and RGBW colors as per-channel PWM values in unsigned integer format. The maximum PWM value is
    determined by the maxPwm parameter supplied to the Rgb2Pwm() and Rgbw2Pwm() conversion functions. Note that the max value of 
    a "16-bit" PWM value can be 65536, which requires 17 bits to represent.
*/
struct rgbwPwm {
  uint32_t r;
  uint32_t g;
  uint32_t b;
  uint32_t w;
};

struct rgbPwm {
  uint32_t r;
  uint32_t g;
  uint32_t b;
};


#endif // _COLOR_TYPES

rgbwF Hsi2Rgbw(hsiF hsi, float gamma, rgbwF scaleFactors);
rgbwF Hsi2Rgbw(hsiF hsi);
rgbF Hsi2Rgb(hsiF hsi, float gamma, rgbF scaleFactors);
rgbF Hsi2Rgb(hsiF hsi);
rgbwPwm Rgbw2Pwm(rgbwF in, uint32_t maxPwm);
rgbPwm Rgb2Pwm(rgbF in, uint32_t maxPwm);
float HueDistance(float startH, float endH);
float WrapHue(float h);
hsiF BlendHsi(hsiF color1, hsiF color2, float scaleI2);
hsiF InterpHsi(hsiF color1, hsiF color2, float ctrl);
void SetGlobalBrightness(float brightVal);
float GetGlobalBrightness();
