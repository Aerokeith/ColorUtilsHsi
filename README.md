# ColorUtilsHsi
This library contains three groups of functions:
    1. Convert HSI colors (floating point format) to RGB or RGBW colors (also in floating point)
    2. Convert RGB or RGBW colors (floating point) to integer PWM values with a specified resolution
    3. Perform various processing fucntions involving one or two HSI colors (e.g. interpolate)
The HSI to RGBW color space conversion algorithm defined in 
    https://blog.saikoled.com/post/44677718712/how-to-convert-from-hsi-to-rgb-white, with optimizations defined in
    http://www.math.tau.ac.il/~turkel/notes/hsi-to-rgb-conversion.pdf.
