#include "Arduino.h"
#include "SDCard.h"
#include "HardwareSerial.h"

SDCard::SDCard(int cs)
{
    CS = cs;
}

void SDCard::start() 
{
    if (!SD.begin(CS)) {
      Serial.println("Card failed, or not present");
      while (1);  // Stop here if no SD card
    }
    Serial.println("SD card initialized.");

    int i = 0;
    dataFileName = "data" + String(i) + ".csv";
    while (SD.exists(dataFileName)) {
      i++;
      dataFileName = "data" + String(i) + ".csv";
    }

    dataFile = SD.open(dataFileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println("Time,Wind Angle,Temperature,Humidity,Pressure");
      dataFile.flush();
      dataFile.close();
    } else {
      Serial.println("Error opening file");
    }
}

void SDCard::writeData(unsigned long currentTime, double lat, double lon, int windAngle, float temp, float hum, float press, int wV, int sail, int rudder, double heading, double target, int nW) 
{   
    dataFile = SD.open(dataFileName, FILE_WRITE);
    if (dataFile) {
      dataFile.print(currentTime);
      dataFile.print(",");
      dataFile.print(String(lat, 6));
      dataFile.print(",");
      dataFile.print(String(lon, 6));
      dataFile.print(",");
      dataFile.print(windAngle);
      dataFile.print(",");
      dataFile.print(temp);
      dataFile.print(",");
      dataFile.print(hum);
      dataFile.print(",");
      dataFile.print(press);
      dataFile.print(",");
      dataFile.print(wV);
      dataFile.print(",");
      dataFile.print(sail);
      dataFile.print(",");
      dataFile.print(rudder);
      dataFile.print(",");
      dataFile.print(heading);
      dataFile.print(",");
      dataFile.print(target);
      dataFile.print(",");
      dataFile.println(nW);
      dataFile.flush();
      dataFile.close();
    } else {
      Serial.println("Error opening file");
    }
}

void SDCard::writeData(unsigned long currentTime, char id, double lat, double lon, int windAngle, float temp, float hum, float press) 
{
  dataFile = SD.open(dataFileName, FILE_WRITE);
  if (dataFile) {
    dataFile.print(currentTime);
    dataFile.print(",");
    dataFile.print(String(lat, 6));
    dataFile.print(",");
    dataFile.print(String(lon, 6));
    dataFile.print(",");
    dataFile.print(windAngle);
    dataFile.print(",");
    dataFile.print(temp);
    dataFile.print(",");
    dataFile.print(hum);
    dataFile.print(",");
    dataFile.println(press);
    dataFile.flush();
    dataFile.close();
  } else {
    Serial.println("Error opening file");
  }
}

int SDCard::fileSize()
{
  dataFile = SD.open(dataFileName, FILE_WRITE);
  if (dataFile) {
    return dataFile.size();
  } else {
    return -1;
  }
}