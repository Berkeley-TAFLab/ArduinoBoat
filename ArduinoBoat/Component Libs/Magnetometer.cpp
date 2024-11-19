#include "Arduino.h"
#include "Adafruit_LIS3MDL.h"
#include "Adafruit_Sensor.h"
#include "HardwareSerial.h"
#include "Magnetometer.h"

Magnetometer::Magnetometer(float hard[3], float soft[3][3])
{
    heading = 0;
    for (int i = 0; i < 3; i++) {
      hard_iron[i] = hard[i];
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            soft_iron[i][j] = soft[i][j];
        }
    }
}

void Magnetometer::start()
{
    if (!lis3mdl.begin_I2C()) {
      Serial.println("Failed to find LIS3MDL chip");
      while (1); // Stop everything if no magnetometer
    }
    Serial.println("Magnetometer Found");
    lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
}

void Magnetometer::updateHeading()
{
    sensors_event_t event;
    lis3mdl.getEvent(&event);
    double raw[3] = {event.magnetic.x, event.magnetic.y, event.magnetic.z};
    double corrected[3];
    for (int i = 0; i < 3; i++) {
      corrected[i] = raw[i] - hard_iron[i];
    }
    double final[3] = {0, 0, 0};
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        final[i] += soft_iron[i][j] * corrected[j];
      }
    }

    heading = atan2(final[1], final[0]) * 180 / PI;
    heading += mag_decl;
    heading += 180;
    if (heading < 0) heading += 360;
    if (heading > 360) heading -= 360;
    // Serial.println(heading);
}