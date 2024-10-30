#include "Arduino.h"
#include "HardwareSerial.h"
#include "WindVane.h"
#include "AS5600.h"
#include "Wire.h"

#define AS5600_ADDRESS 0x36

WindVane::WindVane(int OFF, int qone, int qtwo, int qthree)
{
    currentAngle = 0;
    offset = OFF;
    q1 = qone;
    q2 = qtwo;
    q3 = qthree;
}

void WindVane::start() {
    Wire.beginTransmission(AS5600_ADDRESS);
    if (Wire.endTransmission() != 0) {
      Serial.println("AS5600 (Wind Vane) not detected. Please check your wiring.");
      while (1);
    }

    Serial.println("Wind Vane Found!");
}

void WindVane::getAngle(int currentSailPos)
{
    int rawValue = as5600.readAngle() + offset;
    if (rawValue > 4095) {
      rawValue -= 4095;
    } else if (rawValue < 0) {
      rawValue += 4095;
    }
    Serial.println(rawValue);
    int windVaneAngle = calibrateEncoder(rawValue, q1, q2, q3);

    currentAngle = windVaneAngle + (currentSailPos - 90);

    if (currentAngle < 0) {
      currentAngle += 360;
    } else if (currentAngle > 360) {
      currentAngle -= 360;
    }
    // Serial.println(currentAngle);
}

int WindVane::calibrateEncoder(int rawValue, int q1, int q2, int q3)
{
    if (rawValue >= q3) {
        return map(rawValue, q3 , 4095, 270, 360);
    } else if (rawValue >= q2) {
        return map(rawValue, q2 , q3, 180, 270);
    } else if (rawValue >= q1) {
        return map(rawValue, q1 , q2, 90, 180);
    } else {
        return map(rawValue, 0 , q1, 0, 90);
    }
}

int WindVane::calculateRealAngle(double heading)
{
    int currentWindAngle = heading + map(currentAngle, 0, 360, 360, 0);
    if (currentWindAngle < 0) {
      currentWindAngle += 360;
    } else if (currentWindAngle > 360) {
      currentWindAngle -= 360;
    }
    currentWindAngle += 180;
    if (currentWindAngle > 360) {
      currentWindAngle -= 360;
    }
    return currentWindAngle;
}