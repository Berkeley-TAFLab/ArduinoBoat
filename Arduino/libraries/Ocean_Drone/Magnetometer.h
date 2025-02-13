#ifndef Magnetometer_h
#define Magnetometer_h

#include "Arduino.h"
#include "Adafruit_LIS3MDL.h"
#include "Adafruit_Sensor.h"
#include "HardwareSerial.h"

class Magnetometer
{
    public:
    double heading;
    Magnetometer(float hard[3], float soft[3][3]);
    void start();
    void updateHeading();
    private:
    Adafruit_LIS3MDL lis3mdl;
    float mag_decl = -1.233;
    float hard_iron[3];
    float soft_iron[3][3];
};

#endif