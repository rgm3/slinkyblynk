#ifndef DOT_STAR_PATTERN_H
#define DOT_STAR_PATTERN_H

#include "application.h"
#include <dotstar/dotstar.h>

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, SCANNER2, FADE, RANDOM_FILL, SPACED_LIGHTS, XMAS, SOLID };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class DotStarPattern : public Adafruit_DotStar
{
public:

    // Member Variables:  
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    
    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern
    int On = 1, Spacing = 5, Running = 1, Width = 1;
    
    void shuffle(uint8_t*, int);
    //uint8_t *numbers;
    uint8_t numbers[120];
    
    void (*OnComplete)();  // Callback on completion of pattern
    
    // Constructor - calls base-class constructor to initialize strip
    DotStarPattern(uint16_t pixels, uint8_t type, void (*callback)());
    
    DotStarPattern(uint16_t pixels, uint8_t type)
    :Adafruit_DotStar(pixels, type)
    {
    }
    
    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color) {
        for (int i = 0; i < numPixels(); i++) {
            setPixelColor(i, color);
        }
        show();
    }
    
    // shared state modifiers
    void
      Update(),
      Increment(),
      Reverse(),
      on(),
      off(),
      setSpeed(uint8_t);
      
    void
      RainbowCycle(uint8_t interval, direction dir = FORWARD),
      RainbowCycleUpdate(),
      RandomFill(uint32_t color1, uint8_t interval),
      RandomFillUpdate(),
      TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD),
      TheaterChaseUpdate(),
      ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD),
      ColorWipeUpdate(),
      Scanner(uint32_t color1, uint8_t interval),
      ScannerUpdate(),
      Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD),
      FadeUpdate(),
      Solid(uint32_t color),
      SolidUpdate(),
      SpacedLights(uint32_t color1, uint32_t color2, uint8_t skip = 5),
      SpacedLights(uint32_t color, uint8_t skip = 5),
      SpacedLightsUpdate(),
      ChristmasColors(uint8_t skip = 5),
      ChristmasUpdate(),
      TwoScanner(uint32_t c1, uint32_t c2, uint8_t interval = 30),
      TwoScannerUpdate();
      
    uint8_t
      getSpeed();
      
    uint32_t
      Wheel(byte WheelPos);
    
private:
    uint8_t speed;
};

#endif // DOT_STAR_PATTERN_H
