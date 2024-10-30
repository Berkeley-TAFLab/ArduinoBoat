#ifndef Sail_h
#define Sail_h

#include "Arduino.h"
#include "Servo.h"
#include "HardwareSerial.h"

class Sail
{
    public:
    int currentSailPos;
    unsigned long lastSailTime;
    unsigned long sailInterval;
    Sail(int pin);
    void start(int range);
    void calculateStationPos(int currentAngle);
    void calculateTargetPos(int currentAngle);
    void proportionTarget();
    void checkFullRotation(unsigned long currentTime);
    void moveSailTo();
    private:
    Servo sailServo;
    int pinNumber;
    int targetSailPos;
    int prevTargetPos;
    unsigned long switchTime;
    bool switchCheck;
};

#endif