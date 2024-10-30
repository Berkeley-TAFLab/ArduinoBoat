#include "Arduino.h"
#include "HardwareSerial.h"
#include "Atmosphere.h"
#include "Adafruit_BME280.h"
#include "Adafruit_Sensor.h"

Atmosphere::Atmosphere()
{
    temp = 0;
    hum = 0;
    press = 0;
}

void Atmosphere::start() 
{
    if (!bme.begin(0x77)) {
      Serial.println("Could not find BME280 sensor!");
      while (1);
    }

    Serial.println("Found atmosphere sensor");
}

void Atmosphere::updateData()
{
    temp = bme.readTemperature();
    hum = bme.readHumidity();
    press = bme.readPressure() / 100.0F;
}