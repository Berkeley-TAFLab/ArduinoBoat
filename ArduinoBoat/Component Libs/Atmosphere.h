#ifndef Atmosphere_h
#define Atmosphere_h

#include "Arduino.h"
#include "Adafruit_BME280.h"
#include "Adafruit_Sensor.h"
#include "HardwareSerial.h"

class Atmosphere
{
    public:
    float temp;
    float hum;
    float press;
    Atmosphere();
    void start();
    void updateData();
    private:
    Adafruit_BME280 bme;
};

#endif