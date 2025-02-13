#ifndef SDCard_h
#define SDCard_h

#include "Arduino.h"
#include "SD.h"
#include "HardwareSerial.h"

class SDCard
{
    public:
    SDCard(int cs);
    void start();
    void writeData(unsigned long currentTime, double lat, double lon, int windAngle, float temp, float hum, float press, int wV, int sail, int rudder, double heading, double target, int nW);
    void writeData(unsigned long currentTime, char id, double lat, double lon, int windAngle, float temp, float hum, float press);
    int fileSize();
    private:
    int CS;
    File dataFile;
    String dataFileName;
};

#endif