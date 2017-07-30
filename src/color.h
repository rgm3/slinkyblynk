#ifndef COLOR_H
#define COLOR_H

#define BLYNK_GREEN 0x23c48e
#define BLYNK_RED 0xd3435c
#define COLOR_SL_ORANGE 0xff3000
#define COLOR_SL_BLUE 0x0088ff
#define COLOR_LIGHT_BLUE 0x59caff
#define COLOR_RED 0xff0000
#define COLOR_GREEN 0x00ff00
#define COLOR_BLUE 0x0000ff
#define COLOR_MAGENTA 0xff00ff
#define COLOR_WHITE 0xffffff
#define COLOR_WARM_WHITE 0xff9f4c
#define COLOR_YELLOW 0xd3c400

String tmp2color(double);
uint32_t colorBetween(uint32_t color1, uint32_t color2, uint8_t percentOfColor2);
uint32_t DimColor(uint32_t);
uint8_t
  Red(uint32_t),
  Green(uint32_t),
  Blue(uint32_t);
  
uint32_t Color(uint8_t r, uint8_t g, uint8_t b);

#endif // COLOR_H
