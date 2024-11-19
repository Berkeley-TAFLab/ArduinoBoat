#include "Arduino.h"
#include "Sail.h"
#include "Servo.h"
#include "HardwareSerial.h"

Sail::Sail(int pin)
{
    pinNumber = pin;
    currentSailPos = 0;
    targetSailPos = 0;
    lastSailTime = 0;
    switchCheck = false;
    sailInterval = 30;
}

void Sail::start(int range)
{
    sailServo.attach(pinNumber, 544 + (2400 - 544) * (1 - (float) (180 / range)), 2400);
    // sailServo.attach(pinNumber);
}

void Sail::calculateStationPos(int currentAngle)
{
    prevTargetPos = targetSailPos;
    targetSailPos = currentAngle + 90;
    while (targetSailPos < 0) {
      targetSailPos += 180;
    } 
    while (targetSailPos > 180) {
      targetSailPos -= 180;
    }
    
    proportionTarget();
}

void Sail::calculateTargetPos(int currentAngle)
{
    prevTargetPos = targetSailPos;
    if (currentAngle < 90 && currentAngle > 15) {
      targetSailPos = currentAngle - 15;
    } else if (currentAngle > 270 && currentAngle < 345) {
      targetSailPos = currentAngle + 15 - 180;
    } else if (currentAngle >= 90 || currentAngle <= 270) {
      targetSailPos = currentAngle / 2;
    } else {
      targetSailPos = 0;
    }

    targetSailPos -= 90;
    if (targetSailPos < 0) {
      targetSailPos += 180;
    }

    proportionTarget();
}

void Sail::proportionTarget()
{
    if (abs(targetSailPos - currentSailPos) > 90) {
      sailInterval = 30;
    } else {
      sailInterval = 30 + 70 * pow((double) (90 - abs(targetSailPos - currentSailPos)) / 90, 2);
    }
}

void Sail::checkFullRotation(unsigned long currentTime)
{
    if (abs(targetSailPos - prevTargetPos) > 160) {
      if (!switchCheck) {
        switchCheck = true;
        switchTime = millis();
        targetSailPos = prevTargetPos;
        return;
      }
      if (currentTime - switchTime < 3000) {
        targetSailPos = prevTargetPos;
      } else {
        switchCheck = false;
      }
    } else {
      switchCheck = false;
    }
}

void Sail::moveSailTo()
{
    if (abs(currentSailPos - targetSailPos) > 5) {
      if (currentSailPos < targetSailPos) {
        currentSailPos++;
      } else {
        currentSailPos--;
      }
      sailServo.write(currentSailPos);
    }
}