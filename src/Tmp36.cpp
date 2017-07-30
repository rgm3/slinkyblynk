#include "Tmp36.h"

// Constructor
Tmp36::Tmp36(int pin, int sampleInterval) {
    this->pin = pin;
    this->sampleInterval = sampleInterval;

    // No need to set pin mode for analogRead on Photon, apparently.
    // But I don't want it floating or to use an external pull-down.
    //pinMode(pin, INPUT_PULLDOWN);
}

void Tmp36::update()
{
    if((millis() - lastUpdate) > sampleInterval) // time to update
    {
        lastUpdate = millis();
        
        read();
        smoothUpdate();
        
        tempC = (float)calcTempC();
        tempF = (float)calcTempF();
        
        //Serial.printf("%4d   %4d   %f\r\n", lastReading, smoothReading, tempF);
    }
}

void Tmp36::read() {
    //lastReading = analogRead(pin);
    lastReading = 930;
}

// Low-Pass filter the data.
// Thanks https://kiritchatterjee.wordpress.com/2014/11/10/a-simple-digital-low-pass-filter-in-c/
void Tmp36::smoothUpdate() {
    
    //smoothReading = lastReading;
    //return;
    
    if( smoothReading == 0) {
        smoothReading = lastReading;
        return;
    }
    const uint8_t LPF_BETA = 4;
    smoothReading = (smoothReading << LPF_BETA) - smoothReading; 
    smoothReading += lastReading;
    smoothReading >>= LPF_BETA;
}

float Tmp36::calcTempC() {
    float voltage = smoothReading * V_AREF / (float)MAX_ADC;
    return (voltage - 500) / 10;
}

float Tmp36::calcTempF() {
    float tc = calcTempC();
    return ((9 * tc) / 5.0) + 32;
}

float Tmp36::voltage() {
    return smoothReading * V_AREF / (float)MAX_ADC;
}
