/* ColorUtilsHsi
    This library contains three groups of functions:
    1. Convert HSI colors (floating point format) to RGB or RGBW colors (also in floating point)
    2. Convert RGB or RGBW colors (floating point) to integer PWM values with a specified resolution
    3. Perform various processing fucntions involving one or two HSI colors (e.g. interpolate)
*/

#include <Arduino.h>
#include "ColorUtilsHsi.h"

float globalBrightness = 1.0;   // global brightness level, changed externally using SetGlobalBrightness()

/* Hsi2Rgbw() implements the HSI to RGBW color space conversion algorithm defined in 
    https://blog.saikoled.com/post/44677718712/how-to-convert-from-hsi-to-rgb-white, with optimizations defined in
    http://www.math.tau.ac.il/~turkel/notes/hsi-to-rgb-conversion.pdf.
    It also applies gamma correction using the same gamma value for all color components, to linearize the perceived increase in
    brightness as intensity (I) is increased linearly. Finally, it applies a per-color scale factor to compensate for 
    differences in perceived brightness among the LED colors. 
    For convenience, the function call is overloaded to allow it to be called with the the gamma and scaleFactors parameters
    to be omitted, in which case the default values defined in colorUtilsHsi.h will be used. 
*/

rgbwF Hsi2Rgbw(hsiF hsi) {
  return (Hsi2Rgbw(hsi, DEFAULT_GAMMA, DEFAULT_RGBW_SCALE_FACTORS));
}

rgbwF Hsi2Rgbw(hsiF hsi, float gamma, rgbwF scaleFactors) {
  const float rad60 = 1.0472;     // 60 degrees in radians
  const float rad120 = 2.0944;    // 120 degrees in radians
  const float rad240 = 4.1888;    // 240 degrees in radians
  float hAdj;   // hue (0 - 1) converted to radians ( 0 - 2*PI) and adjusted to range 0 - rad120
  float scaleSI;  // temp scale factor
  float r, g, b, w;   // temps for computed return value

  hsi.h = constrain(hsi.h, 0, 1);   // ensure that all components of hsi are in range 0-1
  hsi.s = constrain(hsi.s, 0, 1);
  hsi.i = constrain(hsi.i, 0, 1);
  hsi.i *= globalBrightness;  // scale by global brightness level

    // Step 1: Compute RGB assuming full saturation (S == 1) and intensity (I == 1)
  hAdj = hsi.h * 2 * PI;    // convert h to radians
  if (hAdj <= rad120) {   // if h <= 120 degrees

    r = (1 + cos(hAdj)/cos(rad60 - hAdj)) / 3; 
    g = max(0, 1 - r);
    b = 0;
  }
  else if (hAdj <= rad240) {    // if (120 < h <= 240) degrees
    hAdj -= rad120;   // adjust down to range 0 - rad120
    g = (1 + cos(hAdj)/cos(rad60 - hAdj)) / 3;
    b = max(0, 1 - g);
    r = 0;
  }
  else {    // if (240 < h <= 360) degrees
    hAdj -= rad240;   // adjust down to range 0 - rad120
    b = (1 + cos(hAdj)/cos(rad60 - hAdj)) / 3;
    r = max(0, 1 - b);
    g = 0;
  }

  // Step 2: Scale RGB values by both S and I
  scaleSI = hsi.s * hsi.i;
  r *= scaleSI;
  g *= scaleSI;
  b *= scaleSI;

  // Step 3: Compute w based on amount of desaturation (1 - S) and then scale by I
  w = (1 - hsi.s) * hsi.i;

    // Step 4: Apply Gamma correction
  r = pow(r, gamma); 
  g = pow(g, gamma);   
  b = pow(b, gamma);  
  w = pow(w, gamma); 
  
    // Step 5: Apply per-color scale factors
  r *= scaleFactors.r;
  g *= scaleFactors.g;
  b *= scaleFactors.b;
  w *= scaleFactors.w;

  return {r, g, b, w};
}


/* Hsi2Rgb() implements the HSI to RGB color space conversion algorithm defined in 
    http://www.math.tau.ac.il/~turkel/notes/hsi-to-rgb-conversion.pdf.
    It also applies gamma correction using the same gamma value for all color components, to linearize the perceived increase in
    brightness as intensity (I) is increased linearly. Finally, it applies a per-color scale factor to compensate for 
    differences in perceived brightness among the LED colors. 
    For convenience, the function call is overloaded to allow it to be called with the the gamma and scalFactors parameters
    to be omitted, in which case the default values defined in colorUtilsHsi.h will be used. 
*/

rgbF Hsi2Rgb(hsiF hsi) {
  return (Hsi2Rgb(hsi, DEFAULT_GAMMA, DEFAULT_RGB_SCALE_FACTORS));
}

rgbF Hsi2Rgb(hsiF hsi, float gamma, rgbF scaleFactors) {
  const float rad60 = 1.0472;     // 60 degrees in radians
  const float rad120 = 2.0944;    // 120 degrees in radians
  const float rad240 = 4.1888;    // 240 degrees in radians
  float hAdj;   // hue (0 - 1) converted to radians ( 0 - 2*PI) and adjusted to range 0 - rad120
  float r, g, b;   // temps for computed return value

  hsi.h = constrain(hsi.h, 0, 1);   // ensure that all components of hsi are in range 0-1
  hsi.s = constrain(hsi.s, 0, 1);
  hsi.i = constrain(hsi.i, 0, 1);
  hsi.i *= globalBrightness;  // scale by global brightness level

    // Step 1: Compute RGB assuming full intensity (I == 1)
  hAdj = hsi.h * 2 * PI;    // convert h to radians
  if (hAdj <= rad120) {   // if h <= 120 degrees

    r = (1 + (hsi.s * cos(hAdj))/cos(rad60 - hAdj)) / 3;
    b = (1 - hsi.s) / 3;
    g = 1 - (r + b);
    
  }
  else if (hAdj <= rad240) {    // if (120 < h <= 240) degrees
    hAdj -= rad120;   // adjust down to range 0 - rad120
    g = (1 + (hsi.s * cos(hAdj))/cos(rad60 - hAdj)) / 3;
    r = (1 - hsi.s) / 3;
    b = 1 - (r + g);
    
  }
  else {    // if (240 < h <= 360) degrees
    hAdj -= rad240;   // adjust down to range 0 - rad120
    b = (1 + (hsi.s * cos(hAdj))/cos(rad60 - hAdj)) / 3;
    g = (1 - hsi.s) / 3;
    r = 1 - (b + g);
  }

  // Step 2: Scale RGB values by intensity I
  r *= hsi.i;
  g *= hsi.i;
  b *= hsi.i;

    // Step 3: Apply Gamma correction
  r = pow(r, gamma); 
  g = pow(g, gamma);   
  b = pow(b, gamma);  
  
    // Step 4: Apply per-color scale factors
  r *= scaleFactors.r;
  g *= scaleFactors.g;
  b *= scaleFactors.b;

  return {r, g, b};
}



/* Rgbw2Pwm() and Rgb2Pwm() convert a floating point structure of type rgbwF (or rgbF) to an equivalent integer structure of type rgbwPwm
     (or rgbPwm). The floating point values are assumed to be in the range 0 - 1, and before integer conversion they are upscaled based on 
     the maximum PWM output defined by the maxPwm parameter.
*/
rgbwPwm Rgbw2Pwm(rgbwF in, uint32_t maxPwm) {
  rgbwPwm out;

  out.r = round(in.r * maxPwm); // scale up and round to nearest integer
  out.g = round(in.g * maxPwm);
  out.b = round(in.b * maxPwm);
  out.w = round(in.w * maxPwm);
  return out;
}

rgbPwm Rgb2Pwm(rgbF in, uint32_t maxPwm) {
  rgbPwm out;

  out.r = round(in.r * maxPwm); // scale up and round to nearest integer
  out.g = round(in.g * maxPwm);
  out.b = round(in.b * maxPwm);
  return out;
}



/* HueDistance()
    Computes the signed "distance" between two hue (H) values, given that Hue is a is a circular range that wraps around from the 
    maximum value (1.0) to 0. 
    Parameters: 
      float startH: Baseline hue from which to compute the distance
      float endH: Target hue to compute the distance of (from startH)
      bool useShortestDist: If true, the distance returned will be the shortest distance (magnitude) comparing the positive and 
        negative directions, including wrap-around. If false, the positiveDir parameter determines the direction used to compute 
        the distance
      bool positiveDir: If true, the hue distance to endH will be computed in the positive direction from StartH
    Returns: 
      float: Signed distance from startH to endH, as influenced by the useShortestDist and positveDir flags
*/
float HueDistance(float startH, float endH, bool useShortestDist, bool positiveDir) {
  float nonWrapDist;    // signed distance without wrapping
  float nonWrapDistAbs; // magnitude of non-wrapped distance
  bool ascending;       // true if endH is greater than startH

  nonWrapDist = endH - startH;  // compute distance without wraparound
  if (nonWrapDist == 0)         // if start and end hues are the same
    return (0);                 // then HueDistance is zero
  ascending = (endH >= startH); // determine order of start/end hues
  nonWrapDistAbs = abs(nonWrapDist);  // compute magnitude of non-wrapped distance
  if (useShortestDist) {
    if (nonWrapDistAbs <= 0.5) {  // non-wrapped distance is shortest
      return (nonWrapDist);
    }
    else if (ascending) { // wrapped distance in negative direction is shortest
      return (nonWrapDist - 1);
    }
    else { // (!ascending), so positive wrap is shortest
      return (1 - nonWrapDistAbs);
    }
  }
  else {   // don't use shortest distance
    if (positiveDir) {  // compute distance only in positive direction (with wrapping if necessary)
      if (ascending)    // if hues are ordered, no wrapping necessary
        return (nonWrapDist);
      else  // hues not ordered, must wrap
        return (nonWrapDist + 1);
    }
    else { // compute distance only in negative direction (with wrapping if necessary)
      if (ascending)  // hues are ordered, must wrap
        return (nonWrapDist - 1);
      else  // hues not ordered, no wrapping necessary
        return (nonWrapDist);
    }
  }
}

/* HueDistance()
    Overload of HueDistance() above, omitting useShortestDist and positiveDir parameters for backward compatibility 
    Parameters: 
      float startH: Baseline hue from which to compute the distance
      float endH: Target hue to compute the distance of (from startH)
    Returns: 
      float: Minimum-magntitude (shortest) signed distance from startH to endH
*/
float HueDistance(float startH, float endH) {
  return HueDistance(startH, endH, true, false);
}


/* WrapHue() is used to wrap a computed floating point hue value back into the correct range. For example, if a hue
    value is computed as 0.1 - 0.3 = -0.2, WrapHue() will return the correct value of 1.0 - 0.2 = 0.8. 
*/
float WrapHue(float h) {
  if (h < 0) 
    h = 1 + h;
  else if (h > 1)
    h = h - 1;
  return h;
}


/* BlendHsi() returns an HSI color that is an interpolated blend of the parameters color1 and color2. The blended Hue and 
    Saturation values are computed based on the relative brightness (I) of color1 and color2, where the brightness of 
    color2 is further scaled by the parameter scaleI2. This allows the colors to be blended as if color2 were much brighter
    than the normal range of I (in HSI). Blending is performed based on the shortest distance between the hue values (wrapped
    or unwrapped), as determined by HueDistance(). The parameter scaleI2 has a minimum value of 1 (no scaling).
*/
hsiF BlendHsi(hsiF color1, hsiF color2, float scaleI2) {
  float blendRatio;
  float iRatio;
  float blendDist;
  hsiF blendColor;

  color2.i *= scaleI2;  // scale up the brightness of color2
  if (color1.i < 0.001) // if the color1.i is essentially 0
    blendRatio = 1.0;   // then color1 has no influence at all (color2 dominates)
  else {
    iRatio = color2.i / color1.i; // blends are based on ratio of scaled v2 to v1
    blendRatio = iRatio / (1 + iRatio);  // convert ratio to value between 0 and 1
  }
  blendDist = HueDistance(color1.h, color2.h) * blendRatio; // use blendRatio to compute hue distance from color1 to blended color
  blendColor.h = WrapHue(color1.h + blendDist); // add blended distance and apply wrapping if necessary
  blendColor.i = min((color1.i + color2.i), 1.0);  // brightness "blend" is just a summation with range clipping
    // saturation blend is a brightness-weighted average. Protect against div-by-0 error
  blendColor.s = ((color1.i * color1.s) + (color2.i * color2.s)) / max((color1.i + color2.i), 0.1);  
  return blendColor;
}


/* InterpHsi() returns an HSI color interpolated between color1 and color2 based on the value of ctrl (between 0 and 1).
    If one of the colors is "off" (I=0), its hue and saturation are set to the same values as the other color. This results in 
    interpolation based only on intensity (I). 
  Parameters:
    hsiF color1: color to be returned when ctrl=0
    hsiF color2: color to be returned when ctrl=1
    float ctrl: determines degree of interpolation between color1 and color2 (range 0 - 1)
    bool useShortest: determines the "direction" of interpolation in the circular hue space. When true, interpolation is performed
      in the direction (opositive or negative) of shortest distance between color1.h and color2.h, with hue wrapping as needed.
      When false, hue interpolation is always in the positive direction from color1.h to color2.h, with wrapping as needed. 
  Returns:
    hsiF: interpolated color, including hue, saturation, and intensity components
*/
hsiF InterpHsi(hsiF color1, hsiF color2, float ctrl, bool useShortest) {
  hsiF iColor;  // return value

  if (color1.i == 0) {    // color1 is "off", so interpolate only in intensity
    color1.h = color2.h;
    color1.s = color2.s;
  }
  else if (color2.i == 0) {   // color2 is "off", so interpolate only in intensity
    color2.h = color1.h;
    color2.s = color1.s;
  }
    // hue interpolation based on wrapped hue distance in specified direction
  iColor.h = WrapHue(color1.h + (HueDistance(color1.h, color2.h, useShortest, true) * ctrl));
  iColor.s = color1.s + ((color2.s - color1.s) * ctrl); // normal interpolation for saturation and intensity
  iColor.i = color1.i + ((color2.i - color1.i) * ctrl);
  return (iColor);
}


/* Overload of InterpHsi(), for backward compatibility, always using shortest hue distance 
*/
hsiF InterpHsi(hsiF color1, hsiF color2, float ctrl) {
  return (InterpHsi(color1, color2, ctrl, true));
}




void SetGlobalBrightness(float brightVal) {
  globalBrightness = constrain(brightVal, 0, 1);
}


float GetGlobalBrightness() {
  return globalBrightness;
}
