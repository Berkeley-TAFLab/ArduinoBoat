#ifndef Rudder_h
#define Rudder_h

#include "Arduino.h"
#include "Servo.h"
#include "HardwareSerial.h"

class Rudder
{
    public:
    int currentRudderPos;
    int targetRudderPos;
    unsigned long rudderInterval;
    unsigned long lastRudderTime;
    Rudder(int pin);
    void start();
    void turnTo(double bearing, double directionBearing);
    void moveRudderTo();
    private:
    Servo tailServo;
    int pinNumber;
    int straight;
    int range;
};

#endif