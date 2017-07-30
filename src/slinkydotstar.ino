#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "color.h"
#include "math.h"
#include "OneWire.h"
#include "spark-dallas-temperature.h"
#include "blynk/blynk.h"
#include "DotStarPattern.h"

#include "Tmp36.h"
/*
Slinky light show and temperature measurement.
*/
#define NUMPIXELS 120 // Number of LEDs in strip
#define ONBUTTON V4
#define WARNING_SLIDER V2
#define WARNING_LED V5
#define TMPPIN A1
#define DS18PIN D0
#define PULSE V6
#define BRIGHTNESS V7
#define ZEBRA1 V8
#define ZEBRA2 V9
#define GAUGE1 V0
#define GAUGE2 V3
#define SPEED V11
#define PATTERN V10
#define SLINKCOLORS V12
#define RUNNING V13
#define SPACING_SLIDER V14
#define WIDTH_SLIDER V15

uint32_t g_color1 = COLOR_SL_ORANGE;
uint32_t g_color2 = COLOR_SL_BLUE;
uint8_t buffer[NUMPIXELS];
int g_count = 0;
char blynk_auth[] = "SECRET_CODE_HERE"; // TODO: put this in 'blynk-auth.h' and add that to .gitignore
int g_warningLevel = 80;
float g_outsideTemp = 0;
double g_insideTemp = 0; // Particle.variable doesn't support float
bool isFirstConnect = false;
WidgetLED warningLed(WARNING_LED);

unsigned long lastInsideCheck = 0;
uint16_t insideCheckInterval = 1000; // The DS18B20 takes a few hundred milliseconds, depending on resolution.
uint16_t blynkPublishInterval = 4000;
unsigned long g_lastBlynkPublish;
pattern lastPattern;
int g_lastPattern = 1;
int g_blynkPattern = 1;

// Hardware SPI is a little faster, but must be wired to specific pins
// (Core/Photon/P1/Electron = pin A5 for data, A3 for clock)
// This dotstar strip has GREEN = DATA (to A5),  YELLOW = CLK (to A3)

void cb1();
void cb2();
void cb3();
void fadecb();
void breathingcb();
void printAddress(DeviceAddress);
void checkOutsideTemp();
void checkInsideTemp();
void outsideTempHandler(const char *, const char *);
void updateBlynk();
void blynkPublish();
void setPattern(int);
void setZebra(int, uint32_t);
void updateWarningSlider();
void updateWarningLed();
void setIntervalFromSpeed();
void tempWarningPatternCheck();

DotStarPattern Strip(NUMPIXELS, DOTSTAR_BGR); // , &cb1
Timer weatherTimer(97 * 1000, checkOutsideTemp);
DallasTemperature sensors(new OneWire(DS18PIN));
DeviceAddress insideThermometer = { 0x10, 0xEC, 0xAE, 0x2C, 0x03, 0x08, 0x00, 0xCE };


void setup() {
  Particle.variable("temp", g_insideTemp);
  Particle.subscribe("hook-response/outsidetemp", outsideTempHandler, MY_DEVICES);
  Blynk.begin(blynk_auth);
    
  Serial.begin(9600);
  delay(1000);
  Serial.println("booted");
  
  pinMode(DS18PIN, INPUT);
  sensors.begin();
  //if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
  //printAddress(insideThermometer);


  Strip.begin(); // Initialize pins for output
  Strip.show();  // Turn all LEDs off ASAP
  
  //blynkTimer.start();
  Time.zone(-6);
  weatherTimer.start();
  checkInsideTemp();
  checkOutsideTemp();
}

void printAddress(DeviceAddress da)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (da[i] < 16) Serial.print("0");
    Serial.print(da[i], HEX);
  }
}

void loop() {
  Strip.Update();
  Blynk.run();
  checkInsideTemp();

  updateBlynk();
}

void checkInsideTemp() {
    float reading;
    unsigned long now = millis();
    
    if (now - lastInsideCheck > insideCheckInterval) {
        lastInsideCheck = now;
        sensors.requestTemperaturesByAddress(insideThermometer);
        reading = sensors.getTempF(insideThermometer);

        if (reading > -50.0 && reading < 180.0)
            g_insideTemp = reading;
        else
          Serial.printf("got garbage %f\n", reading);
    }
    tempWarningPatternCheck();
}

void tempWarningPatternCheck() {
    if (g_insideTemp > g_warningLevel) {
        if (g_blynkPattern != 0) {
            g_lastPattern = g_blynkPattern;
            setPattern(0);
        }
    } else {
        if (g_blynkPattern == 0) {
            setPattern(g_lastPattern);
            Blynk.syncVirtual(SPEED);
        }
    }
}

void updateBlynk() {
  if( millis() - g_lastBlynkPublish > blynkPublishInterval ){
    g_lastBlynkPublish = millis();
    //Serial.println("publishing to blynk");
    blynkPublish();
  }
}

BLYNK_WRITE(ONBUTTON) {
    if(param.asInt()) {
        Strip.on();
    } else {
        Strip.off();
    }
}

BLYNK_WRITE(PULSE) {
    Strip.On ? Strip.off() : Strip.on();
}

BLYNK_CONNECTED() {
  if (isFirstConnect) {
    // Request Blynk server to re-send latest values for all pins
    //Blynk.syncAll();

    // You can also update an individual Virtual pin like this:
    //Blynk.syncVirtual(V0);

    isFirstConnect = false;
  }
  Blynk.syncVirtual(WARNING_SLIDER);

  Blynk.virtualWrite(ZEBRA1, Red(g_color1), Green(g_color1), Blue(g_color1));
  Blynk.virtualWrite(ZEBRA2, Red(g_color2), Green(g_color2), Blue(g_color2));
  //Blynk.virtualWrite(BRIGHTNESS, Strip.getBrightness());
  //Blynk.virtualWrite(PATTERN, g_blynkPattern);
  Blynk.syncVirtual(PATTERN);
  Blynk.syncVirtual(BRIGHTNESS);
  Blynk.syncVirtual(SPEED);
  
  Blynk.virtualWrite(ONBUTTON, Strip.On);
  Blynk.virtualWrite(RUNNING, Strip.Running);
  //Blynk.virtualWrite(WARNING_SLIDER, g_warningLevel);
  //updateWarningSlider();
}
BLYNK_WRITE(WARNING_SLIDER) {
    g_warningLevel = param.asInt();
    updateWarningSlider();
    tempWarningPatternCheck();
}

BLYNK_WRITE(ZEBRA1) {
    g_color1 = Strip.Color(param[0].asInt(), param[1].asInt(), param[2].asInt());
    Strip.Color1 = g_color1;
}
BLYNK_WRITE(ZEBRA2) {
    g_color2 = Strip.Color(param[0].asInt(), param[1].asInt(), param[2].asInt());
    Strip.Color2 = g_color2;
}
BLYNK_WRITE(BRIGHTNESS) {
    Strip.setBrightness(param.asInt());
}
BLYNK_WRITE(SPACING_SLIDER) {
    int slider = param.asInt();
    Strip.Spacing = constrain(slider, 1, NUMPIXELS - 1);
}
BLYNK_WRITE(WIDTH_SLIDER) {
    int slider = param.asInt();
    Strip.Width = constrain(slider, 1, 10);
}
BLYNK_WRITE(SPEED) {
    Strip.setSpeed(param.asInt());
    setIntervalFromSpeed();
}
BLYNK_WRITE(RUNNING) {
    Strip.Running = param.asInt();
}
void setIntervalFromSpeed() {
    uint8_t s = Strip.getSpeed();
    int newInterval = 200.0 * exp(-0.012 * s) - 8; // exponential
    //int newInterval = map(Strip.getSpeed(), 0, 255, maxInterval, minInterval); // linear
    Strip.Interval = constrain(newInterval, 1, 200);
}

BLYNK_WRITE(SLINKCOLORS) {
    g_color1 = COLOR_SL_ORANGE;
    g_color2 = COLOR_SL_BLUE;
    
    Strip.Color1 = g_color1;
    Strip.Color2 = g_color2;
    
    setZebra(ZEBRA1, g_color1);
    setZebra(ZEBRA2, g_color2);
}

void setZebra(int vPin, uint32_t color) {
    Blynk.virtualWrite(vPin, Red(color), Green(color), Blue(color));
}

BLYNK_WRITE(PATTERN) {
    Strip.OnComplete = NULL;
    setPattern(param.asInt());
}

void setPattern(int pat) {
    g_blynkPattern = pat;
    Strip.OnComplete = NULL;
    switch (g_blynkPattern) {
        case 0: // Warning flashes
            Strip.Fade(COLOR_RED, COLOR_WHITE, 6, 10);
            Strip.setSpeed(100);
            Strip.TotalSteps = 6;
            //Strip.OnComplete = &fadecb;
            break;
        case 1: // Auto
            break;
        case 2: // Fill Up/Down (colorwipe)
            Strip.ColorWipe(g_color1, 20);
            Strip.OnComplete = &cb2;
            break;
        case 3: // Random Fill
            for(int i = 0;i < 120;i++) Strip.numbers[i] = i;
            Strip.shuffle(Strip.numbers, NUMPIXELS);;
            Strip.RandomFill(g_color1, 20);
            Strip.OnComplete = &cb1;
            break;
        case 4: // Larson Scanner (single color)
            Strip.Scanner(g_color1, 20);
            break;
        case 5: // Larson Scanner (random colors)
            Strip.Scanner(g_color1, 20);
            Strip.OnComplete = &cb3;
            break;
        case 6: // Rainbow
            Strip.RainbowCycle(10);
            Strip.Width = 1;
            break;
        case 7: // Cross Fade
            Strip.Fade(g_color1, g_color2, 200, 30);
            Strip.OnComplete = &fadecb;
            break;
        case 8: // Breathing (Fade in color 1, Fade out, Fade in color 2, Fade out)
            Strip.Fade(g_color1, 0, 64, 30);
            Strip.OnComplete = &breathingcb;
            break;
        case 9: // Solid
            Strip.Solid(g_color1);
            break;
        case 10: // Spaced out: single color
            g_color1 = COLOR_WARM_WHITE;
            g_color2 = g_color1;
            Strip.SpacedLights(g_color1);
            setZebra(ZEBRA1, g_color1);
            setZebra(ZEBRA2, g_color2);
            break;
        case 11: // Spaced out: multicolor (christmas lights)
            Strip.ChristmasColors();
            break;
        case 12: // Theater Chase
            Strip.Spacing = 6;
            Strip.TheaterChase(g_color1, g_color2, 30);
            //Strip.Spacing = 8;
            break;
        case 13:  // dual scanner
            Strip.Spacing = 30;
            Strip.TwoScanner(g_color1, g_color2);
            break;
        case 14: // Rainbow Fade
            Strip.RainbowCycle(10);
            Strip.Width = 10;
            break;
        default:
            break;
    }
    setIntervalFromSpeed();
}

void blynkPublish() {
    // Server room temp
    Blynk.virtualWrite(GAUGE1, String::format("%.1f", g_insideTemp));
    Blynk.setProperty(GAUGE1, "color", tmp2color(g_insideTemp));

    // Outside temp
    Blynk.virtualWrite(GAUGE2, String::format("%.1f", g_outsideTemp));
    Blynk.setProperty(GAUGE2, "color", tmp2color(g_outsideTemp));

    updateWarningLed();
}

void updateWarningSlider() {
    Blynk.setProperty(WARNING_SLIDER, "color", tmp2color(g_warningLevel));
    updateWarningLed();
}

void updateWarningLed() {
    if (g_insideTemp > g_warningLevel) {
        if (!warningLed.getValue()) {
          warningLed.on();
          String timestr = Time.format(Time.now(), "%I:%M %p");
          Blynk.notify(String::format("%s: Temperature %.1f exceeds %d °F", timestr.c_str(), g_insideTemp, g_warningLevel));
        }
    } else {
        warningLed.off();
    }
}


void cb1() // random fill
{
    if(Strip.Color1 == g_color1) {
        Strip.Color1 = g_color2;
    } else {
        Strip.Color1 = g_color1;
    }
    Strip.shuffle(Strip.numbers, NUMPIXELS);
}

void cb2() // color wipe
{
    if(Strip.Color1 == g_color1) {
        Strip.Color1 = g_color2;
    } else {
        Strip.Color1 = g_color1;
    }
    Strip.Reverse();
}

void cb3() // scanner
{
    Strip.Color1 = Strip.Wheel(random(255));
    Strip.Reverse();
}
void fadecb()
{
    Strip.Reverse();
}
void breathingcb() {
    if (Strip.Index == 0) {
        if (Strip.Color1 == g_color1) {
            Strip.Color1 = g_color2;
        } else {
            Strip.Color1 = g_color1;
        }
    }
    Strip.Color2 = 0;
    Strip.Reverse();
}

// Temperature ##############################################################
void checkOutsideTemp() {
    Particle.publish("outsidetemp");
    
    // Slow down requests at night to save API calls
    if (Time.hour() > 22 || Time.hour() < 5) {
        weatherTimer.changePeriod(10 * 60 * 1000);
    } else {
        // 1000 free requests per day is one every 86.4 seconds.
        weatherTimer.changePeriod(97 * 1000);
    }
}

void outsideTempHandler(const char *event, const char *data) {
  // Handle the integration response
  // event = "hook-response/outsidetemp/0"
  // data = "74.09~-6"  // temp and TZ offset
  int tz;
  char sTemp[8];
  char sTz[4];
  float ot;

  if (sscanf(data, "%[^'~']~%s", sTemp, sTz) == 2) {
    ot = atof(sTemp);
    tz = atoi(sTz);
    Time.zone(tz);
    g_outsideTemp = ot;
  }
  
  //String tt = Time.timeStr();
  //Serial.println(String::format("%s: Got outside temp %f", tt.c_str(), ot));
}

