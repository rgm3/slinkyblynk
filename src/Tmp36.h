#ifndef TMP36_H
#define TMP36_H

#include "application.h"

class Tmp36 {
  private:
    const int MAX_ADC = 4095;
    const int V_AREF = 3300;    // mV from 3v3 pin, AREF
    int pin;                    // like A0
    unsigned long lastUpdate;
    unsigned long sampleInterval;
    
    
    uint16_t
      lastReading = 0;         // 0-4095
      
    uint32_t
      smoothReading = 0;
      
    void smoothUpdate();

    
  public:
    Tmp36(int pin, int sampleMicroseconds);

    double
      tempC = 0,
      tempF = 0;
    
    void update();
    void read();
    float calcTempC();
    float calcTempF();
    float voltage();
};

#endif // TMP36_H
