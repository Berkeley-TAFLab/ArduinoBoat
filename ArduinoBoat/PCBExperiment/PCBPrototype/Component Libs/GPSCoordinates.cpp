#include "Arduino.h"
#include "HardwareSerial.h"
#include "GPSCoordinates.h"

GPSCoordinates::GPSCoordinates(char d, double la, double lo, int lp, int skTime) 
{
    nodeID = d;
    lat = la;
    lon = lo;
    loop = lp;
    stationKeepingTime = skTime;
}