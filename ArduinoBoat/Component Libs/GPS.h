#ifndef GPS_h
#define GPS_h

#include "Arduino.h"
#include "HardwareSerial.h"

class GPS
{
    public:
    double currentLat;
    double currentLon;
    GPS();
    void start();
    void readGPS();
    void parseGPGLL(String nmea);
    double convertToDecimalDegrees(String rawDegrees, String direction, bool isLatitude);
    private:
    String nmeaSentence;
};

#endif