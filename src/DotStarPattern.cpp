/*
Based on https://learn.adafruit.com/multi-tasking-the-arduino-part-3
Thanks, Bill Earl.
*/
//#include <dotstar/dotstar.h>
#include "DotStarPattern.h"
#include "color.h"

DotStarPattern::DotStarPattern(uint16_t pixels, uint8_t type, void (*callback)())
:Adafruit_DotStar(pixels, type)
{
   /* if(numbers) free(numbers);
    if((numbers = (uint8_t *)malloc(pixels))) {
        for(uint8_t i = 0; i < pixels; i++)
          numbers[i] = i;
      shuffle(numbers, pixels);   
    }*/
    for(uint8_t i = 0; i < pixels; i++)
        numbers[i] = i;
    shuffle(numbers, pixels);
    numbers[1]=1;
    OnComplete = callback;
}

    
// Update the pattern
void DotStarPattern::Update()
{
    if (!On) {
        return;
    }
    if((millis() - lastUpdate) > Interval) // time to update
    {
        lastUpdate = millis();
        switch(ActivePattern)
        {
            case RAINBOW_CYCLE:
                RainbowCycleUpdate();
                break;
            case THEATER_CHASE:
                TheaterChaseUpdate();
                break;
            case COLOR_WIPE:
                ColorWipeUpdate();
                break;
            case SCANNER:
                ScannerUpdate();
                break;
            case FADE:
                FadeUpdate();
                break;
            case RANDOM_FILL:
                RandomFillUpdate();
                break;
            case SPACED_LIGHTS:
                SpacedLightsUpdate();
                break;
            case XMAS:
                ChristmasUpdate();
                break;
            case SOLID:
                SolidUpdate();
                break;
            case SCANNER2:
                TwoScannerUpdate();
                break;
            default:
                break;
        }
    }
}

void DotStarPattern::Solid(uint32_t color)
{
    ActivePattern = SOLID;
    Color1 = color;
    Index = 0;
    ColorSet(Color1);
}

void DotStarPattern::SolidUpdate()
{
    ColorSet(Color1);
}

// Increment the Index and reset at the end
void DotStarPattern::Increment()
{
    if (!Running)
        return;

    if (Direction == FORWARD)
    {
       Index++;
       if (Index >= TotalSteps)
        {
            Index = 0;
            if (OnComplete != NULL)
            {
                OnComplete(); // call the comlpetion callback
            }
        }
    }
    else // Direction == REVERSE
    {
        --Index;
        if (Index <= 0)
        {
            Index = TotalSteps-1;
            if (OnComplete != NULL)
            {
                OnComplete(); // call the comlpetion callback
            }
        }
    }
}

// Reverse pattern direction
void DotStarPattern::Reverse()
{
    if (Direction == FORWARD)
    {
        Direction = REVERSE;
        Index = TotalSteps-1;
    }
    else
    {
        Direction = FORWARD;
        Index = 0;
    }
}

// Initialize for a RainbowCycle
void DotStarPattern::RainbowCycle(uint8_t interval, direction dir)
{
    ActivePattern = RAINBOW_CYCLE;
    Interval = interval;
    TotalSteps = 255;
    Index = 0;
    Direction = dir;
}

// Update the Rainbow Cycle Pattern
void DotStarPattern::RainbowCycleUpdate()
{
    int wheelSteps = map(Width, 10, 1, 1, 512);
    for(int i=0; i< numPixels(); i++)
    {
        setPixelColor(i, Wheel(((i * wheelSteps / numPixels()) + Index) & 255));
    }
    show();
    Increment();
}

// Initialize for Warm Lights
void DotStarPattern::SpacedLights(uint32_t color, uint8_t skip) {
    SpacedLights(color, color, skip);
}
void DotStarPattern::SpacedLights(uint32_t color1, uint32_t color2, uint8_t skip)
{
    ActivePattern = SPACED_LIGHTS;
    Color1 = color1;
    Color2 = color2;
    Spacing = skip;
    clear();
    SpacedLightsUpdate();
}

void DotStarPattern::SpacedLightsUpdate()
{
    clear();
    for(int i=0, c=0; i < numPixels(); i++, c++)
    {
        if( i % Spacing <= (Width - 1))
          setPixelColor(i, (c % 2 == 0) ? Color1 : Color2);
    }
    show();
}

void DotStarPattern::ChristmasColors(uint8_t skip)
{
    ActivePattern = XMAS;
    Spacing = skip;
    clear();
    ChristmasUpdate();
}

void DotStarPattern::ChristmasUpdate()
{
    const int nColors = 5;
    int colorIdx = 0;
    //int mycolors[] = { COLOR_RED, COLOR_YELLOW, COLOR_BLUE, COLOR_GREEN, COLOR_MAGENTA };
    int mycolors[] = { 0xdd0000, 0xdda400, 0x1188ff, 0x00ee22, 0xaa0044 };
    clear();
    for(int i=0; i < numPixels(); i++)
    {
        if (i % Spacing <= (Width - 1)) {
            setPixelColor(i, mycolors[colorIdx] );
            
            if(i % Spacing == Width - 1) {
              colorIdx++;
              if (colorIdx >= nColors) colorIdx = 0;
            }
        }
    }
    show();
}

// Initialize for a Theater Chase
void DotStarPattern::TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir)
{
    ActivePattern = THEATER_CHASE;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color1;
    Color2 = color2;
    Index = 0;
    Direction = dir;
}

// Update the Theater Chase Pattern
void DotStarPattern::TheaterChaseUpdate()
{
    TotalSteps = numPixels() - (numPixels() % Spacing);
    for(int i=0; i< numPixels(); i++)
    {
        if ((i + Index) % Spacing <= (Width - 1))
        {
            setPixelColor(i, Color1);
        }
        else
        {
            setPixelColor(i, Color2);
        }
    }
    show();
    Increment();
}

// Initialize for a ColorWipe
void DotStarPattern::ColorWipe(uint32_t color, uint8_t interval, direction dir)
{
    ActivePattern = COLOR_WIPE;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color;
    Index = 0;
    Direction = dir;
}

// Update the Color Wipe Pattern
void DotStarPattern::ColorWipeUpdate()
{
    setPixelColor(Index, Color1);
    show();
    Increment();
}

void DotStarPattern::RandomFill(uint32_t color, uint8_t interval) {
    ActivePattern = RANDOM_FILL;
    Direction = FORWARD;
    Interval = interval;
    TotalSteps = numPixels();
    Color1 = color;
    Index = 0;
}
void DotStarPattern::RandomFillUpdate()
{
    //if ( Index >= 0 && Index <= numPixels())
    //  if(numbers[Index] >= 0 && numbers[Index] <= numPixels())
        setPixelColor(numbers[Index], Color1);

    show();
    Increment();
}


// Initialize for a SCANNNER
void DotStarPattern::Scanner(uint32_t color1, uint8_t interval)
{
    ActivePattern = SCANNER;
    Interval = interval;
    TotalSteps = (numPixels() - 1) * 2;
    Color1 = color1;
    Index = 0;
}
// Update the Scanner Pattern
void DotStarPattern::ScannerUpdate()
{ 
    for (int i = 0; i < numPixels(); i++)
    {
        if (i == Index)  // Scan Pixel to the right
        {
             setPixelColor(i, Color1);
        }
        else if (i == TotalSteps - Index) // Scan Pixel to the left
        {
             setPixelColor(i, Color1);
        }
        else // Fading tail
        {
             setPixelColor(i, DimColor(getPixelColor(i)));
        }
    }
    show();
    Increment();
}

// Initialize for a SCANNNER
void DotStarPattern::TwoScanner(uint32_t color1, uint32_t color2, uint8_t interval)
{
    ActivePattern = SCANNER2;
    Interval = interval;
    TotalSteps = (numPixels() + Spacing - 1) * 2;
    Color1 = color1;
    Color2 = color2;
    Index = 0;
}
// Update the Scanner Pattern
void DotStarPattern::TwoScannerUpdate()
{ 
    for (int i = 0; i < numPixels() + Spacing; i++) {
        if (i == Index) {  // Scan Pixel to the right
            if (i < numPixels())
                setPixelColor(i, Color1);
            if (i > Spacing) {
                setPixelColor(i - Spacing, Color2);
            }
        }
        else if (i == TotalSteps + Spacing - Index) {  // Scan Pixel to the left
            if (i < numPixels())
                setPixelColor(i, Color2);
            if(i > Spacing) {
                setPixelColor(i - Spacing, Color1);
            }
        }
        else {  // Fading tail
             setPixelColor(i, DimColor(getPixelColor(i)));
        }
    }
    show();
    Increment();
}

// Initialize for a Fade
void DotStarPattern::Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir)
{
    ActivePattern = FADE;
    Interval = interval;
    TotalSteps = steps;
    Color1 = color1;
    Color2 = color2;
    Index = 0;
    Direction = dir;
}

// Update the Fade Pattern
void DotStarPattern::FadeUpdate()
{
    // Calculate linear interpolation between Color1 and Color2
    // Optimise order of operations to minimize truncation error
    uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
    uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
    uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
    
    ColorSet(Color(red, green, blue));
    show();
    Increment();
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t DotStarPattern::Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85)
    {
        return Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else if(WheelPos < 170)
    {
        WheelPos -= 85;
        return Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}

void DotStarPattern::shuffle(uint8_t *array, int n)
{
    if (n > 1) 
    {
        int i;
        for (i = 0; i < n - 1; i++) 
        {
          int j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void DotStarPattern::setSpeed(uint8_t s) {
    speed = s;
}
uint8_t DotStarPattern::getSpeed() {
    return speed;
}
void DotStarPattern::on() {
    On = 1;
}
void DotStarPattern::off() {
    On = 0;
    clear();
    show();
}
