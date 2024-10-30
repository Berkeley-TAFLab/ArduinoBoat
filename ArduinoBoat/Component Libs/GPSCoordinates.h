#ifndef GPSCoordinates_h
#define GPSCoordinates_h

#include "Arduino.h"
#include "HardwareSerial.h"

class GPSCoordinates
{
    public:
    char nodeID;
    double lat;
    double lon;
    int loop;
    int stationKeepingTime;
    GPSCoordinates(char d, double la, double lo, int lp, int skTime);
};

#endif