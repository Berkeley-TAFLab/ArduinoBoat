#include "Arduino.h"
#include "ESC.h"
#include "Servo.h"
#include "HardwareSerial.h"

ESC::ESC(int pin)
{
    pinNumber = pin;
}

void ESC::start()
{
    escMotor.attach(pinNumber);
    escMotor.writeMicroseconds(1000);
}

void ESC::setThrottle(int microseconds) 
{
    escMotor.writeMicroseconds(microseconds);
}