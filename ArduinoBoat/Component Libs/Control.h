#ifndef Control_h
#define Control_h

#include "Arduino.h"
#include "HardwareSerial.h"
#include "GPSList.h"
#include "GPS.h"
#include "Magnetometer.h"
#include "Rudder.h"
#include "ESC.h"
#include "WindVane.h"

class Control
{
    public:
    bool completed;
    bool stationKeeping;
    double distanceToDestination;
    double bearingToDestination;
    Control(WindVane* wv, Rudder* r, ESC* e, Magnetometer* mag, GPSList* gl, GPS* gp);
    void autonomousControl(unsigned long currentTime);
    void navigateToDestination(double distance, double bearing, double directionBearing);
    void stationKeep(double distance, unsigned long currentTime);
    double calculateDistance(double lat1, double lon1, double lat2, double lon2);
    double calculateBearing(double lat1, double lon1, double lat2, double lon2);
    bool facingUpwind(double windVaneAngle);
    private:
    WindVane* windVane;
    Rudder* rudder;
    ESC* esc;
    Magnetometer* magnetometer;
    GPSList* gpsList;
    GPS* gps;
    bool movingUpwind;
    double targetWaypointLat;
    double targetWaypointLon;
    int stationKeepingTime;
    unsigned long arrivalTime;
    int upwindThreshold = 90;
};

#endif