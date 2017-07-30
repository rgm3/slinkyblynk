/************************************************************************************
  Color calculation functions.
************************************************************************************/
#include "application.h"
#include "color.h"

/**
 * Map a temperature value to a color from white, through blue, green, and yellow, to red.
 * Colors are chosen to represent "comfortable" room temperature values from cold to hot.
 * 
 * \param tempF temperature in fahrenheit
 * \return HTML color value string such as "#23c48e"
 */
String tmp2color(double tempF) {
    // Define the temperature - color gradient
    const int NCOLORS = 6;
    int temps[NCOLORS] = { 30, 50, 70, 80, 90, 100 };
    uint32_t colors[NCOLORS] = { COLOR_WHITE, COLOR_LIGHT_BLUE, BLYNK_GREEN, COLOR_YELLOW, BLYNK_RED, COLOR_RED };
    
    uint32_t lowColor = colors[0];
    uint32_t highColor = colors[0];
    int lowTemp = temps[0];
    int highTemp = temps[1];
    uint32_t resultColor;
    uint8_t percent = 100;
    
    for (int i = NCOLORS - 1; i >= 0; i--) {
        if (tempF >= temps[i]) {
            if ( i == NCOLORS - 1 ) {
                // hotter than 85 degrees
                highColor = colors[i];
                highTemp = temps[i];
            } else {
                highColor = colors[i + 1];
                highTemp = temps[i + 1];
            }
    
            lowColor = colors[i];
            lowTemp = temps[i];
            
            //Serial.printf("%d: highColor %x, lowColor %x, highTemp %d, lowTemp %d\n", i, highColor, lowColor, highTemp, lowTemp);
            break;
        }
    }
    
    // How close is the temperature to the highTemp value (and therefore the hightemp color)
    if (lowTemp == highTemp) highTemp++; // avoid div/0 error on STFM platform
    percent = map((int)tempF, lowTemp, highTemp, 0, 100);
    percent = constrain(percent, 0, 100);
    resultColor = colorBetween(lowColor, highColor, percent);

    //Serial.printf("T: %.1f (%d - %d).  C: #%06x (%d%% between #%06x - #%06x) \r\n", tempF, lowTemp, highTemp, resultColor, percent, lowColor, highColor);
    return String::format("#%06x", resultColor);
}

/**
 * Linear interpolation between two color values.
 * \param c1 first color
 * \param c2 second color
 * \param percent_c2 the percentage toward the second color, from 0 - 100
 */
uint32_t colorBetween(uint32_t c1, uint32_t c2, uint8_t percent_c2) {
    uint8_t r1 = (uint8_t)(c1 >> 16);
    uint8_t g1 = (uint8_t)(c1 >> 8);
    uint8_t b1 = (uint8_t)c1;
        
    uint8_t r2 = (uint8_t)(c2 >> 16);
    uint8_t g2 = (uint8_t)(c2 >> 8);
    uint8_t b2 = (uint8_t)c2;
    
    uint8_t red   = map(percent_c2, 0, 100, r1, r2);
    uint8_t green = map(percent_c2, 0, 100, g1, g2);
    uint8_t blue  = map(percent_c2, 0, 100, b1, b2);
    
    return ((uint32_t)red << 16) | ((uint32_t)green << 8) | blue;
}

// Returns the Red component of a 32-bit color
uint8_t Red(uint32_t color) {
    return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t Green(uint32_t color) {
    return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t Blue(uint32_t color) {
    return color & 0xFF;
}

uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

// Calculate 50% dimmed version of a color (used by ScannerUpdate)
uint32_t DimColor(uint32_t color) {
    // Shift R, G and B components one bit to the right
    uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
    return dimColor;
}
