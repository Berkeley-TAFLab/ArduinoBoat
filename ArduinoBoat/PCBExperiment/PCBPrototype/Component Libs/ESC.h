#ifndef ESC_h
#define ESC_h

#include "Arduino.h"
#include "Servo.h"
#include "HardwareSerial.h"

class ESC
{
    public:
    ESC(int pin);
    void start();
    void setThrottle(int microseconds);
    private:
    Servo escMotor;
    int pinNumber;
};

#endif