#include "Arduino.h"
#include "Rudder.h"
#include "Servo.h"
#include "HardwareSerial.h"

Rudder::Rudder(int pin)
{
    pinNumber = pin;
    currentRudderPos = 0;
    targetRudderPos = 0;
    lastRudderTime = 0;
    straight = 90;
    range = 45;
}

void Rudder::start()
{
    tailServo.attach(pinNumber);
}

void Rudder::turnTo(double bearing, double directionBearing)
{
    if (bearing - directionBearing > 0) {
      if (bearing - directionBearing > 180) {
        targetRudderPos = straight + pow(((360 - (bearing - directionBearing))/180), 0.5) * range;
      } else {
        targetRudderPos = straight - pow(((bearing - directionBearing)/180), 0.5) * range;
      }
    }
    if (bearing - directionBearing < 0) {
      if (bearing - directionBearing < -180) {
        targetRudderPos = straight - pow(((360 + (bearing - directionBearing))/180), 0.5) * range;
      } else {
        targetRudderPos = straight + pow((-(bearing - directionBearing)/180), 0.5) * range;
      }
    }
    if (bearing - directionBearing == 0) {
      targetRudderPos = straight;
    }
}

void Rudder::moveRudderTo()
{
    if (abs(targetRudderPos - currentRudderPos) > 1) {
      if (currentRudderPos < targetRudderPos) {
        currentRudderPos++;
      } else {
        currentRudderPos--;
      }
      tailServo.write(currentRudderPos);
    }
}