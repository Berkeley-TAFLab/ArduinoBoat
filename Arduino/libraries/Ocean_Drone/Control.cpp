#include "Arduino.h"
#include "HardwareSerial.h"
#include "GPSList.h"
#include "GPS.h"
#include "Magnetometer.h"
#include "Rudder.h"
#include "ESC.h"
#include "WindVane.h"
#include "Control.h"

Control::Control(WindVane* wv, Rudder* r, ESC* e, Magnetometer* mag, GPSList* gl, GPS* gp)
{
    rudder = r;
    gpsList = gl;
    gps = gp;
    magnetometer = mag;
    esc = e;
    windVane = wv;
    stationKeeping = false;
    movingUpwind = false;
    completed = false;
}

void Control::autonomousControl(unsigned long currentTime) 
{    
    if (gpsList->getSize() > 0) {
      if (completed) {
        if (gpsList->getFirst()->loop) {
          gpsList->moveFirstToLast();
        } else {
          gpsList->deleteFirst();
        }
        completed = false;
      }

      targetWaypointLat = gpsList->getFirst()->lat;
      targetWaypointLon = gpsList->getFirst()->lon;

      distanceToDestination = calculateDistance(gps->currentLat, gps->currentLon, targetWaypointLat, targetWaypointLon);
      bearingToDestination = calculateBearing(gps->currentLat, gps->currentLon, targetWaypointLat, targetWaypointLon);

      if (!stationKeeping) {
        navigateToDestination(distanceToDestination, bearingToDestination, magnetometer->heading);
      } else {
        stationKeep(distanceToDestination, currentTime);
      }
    }
}

void Control::navigateToDestination(double distance, double bearing, double directionBearing) 
{
    if (distance > 5) {
      // if (facingUpwind(windVane->currentAngle)) {
      //   esc->setThrottle(1400);
      // } else {
      //   esc->setThrottle(1000);
      // }
      rudder->turnTo(bearing, directionBearing);
    } else {
      // esc->setThrottle(1000);
      stationKeepingTime = gpsList->getFirst()->stationKeepingTime;
      stationKeeping = true;
      arrivalTime = millis();
    }
}

void Control::stationKeep(double distance, unsigned long currentTime) 
{
    if (distance > 5) {
      stationKeeping = false;
      stationKeepingTime -= currentTime - arrivalTime;
    } else if (currentTime - arrivalTime > stationKeepingTime) {
      stationKeeping = false;
      completed = true;
    }
}

double Control::calculateDistance(double lat1, double lon1, double lat2, double lon2) 
{
    const double R = 6371000; // Radius of the Earth in meters
    double lat1Rad = radians(lat1);
    double lon1Rad = radians(lon1);
    double lat2Rad = radians(lat2);
    double lon2Rad = radians(lon2);

    double dlat = lat2Rad - lat1Rad;
    double dlon = lon2Rad - lon1Rad;

    double a = sin(dlat / 2) * sin(dlat / 2) +
              cos(lat1Rad) * cos(lat2Rad) *
              sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = R * c;

    return distance;
}

double Control::calculateBearing(double lat1, double lon1, double lat2, double lon2) 
{
    double lat1Rad = radians(lat1);
    double lon1Rad = radians(lon1);
    double lat2Rad = radians(lat2);
    double lon2Rad = radians(lon2);

    double y = sin(lon2Rad - lon1Rad) * cos(lat2Rad);
    double x = cos(lat1Rad) * sin(lat2Rad) -
              sin(lat1Rad) * cos(lat2Rad) * cos(lon2Rad - lon1Rad);
    double bearing = degrees(atan2(y, x));

    if (bearing < 0) {
      bearing += 360;
    }

    return bearing; // Ensure the bearing is in the range [0, 360]
}

bool Control::facingUpwind(double windVaneAngle) 
{
    return windVaneAngle < upwindThreshold || windVaneAngle > 360 - upwindThreshold;
}