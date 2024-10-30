#ifndef WindVane_h
#define WindVane_h

#include "Arduino.h"
#include "AS5600.h"
#include "HardwareSerial.h"
#include "Wire.h"

class WindVane
{
    public:
    int currentAngle;
    WindVane(int OFF, int qone, int qtwo, int qthree);
    void start();
    void getAngle(int currentSailPos);
    int calibrateEncoder(int rawValue, int q1, int q2, int q3);
    int calculateRealAngle(double heading);
    private:
    AS5600 as5600;
    int offset;
    int q1;
    int q2;
    int q3; 
};

#endif