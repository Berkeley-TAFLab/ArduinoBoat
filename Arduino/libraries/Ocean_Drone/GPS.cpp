#include "Arduino.h"
#include "HardwareSerial.h"
#include "GPS.h"

#define gpsSerial Serial1

GPS::GPS() : currentLat(0.0), currentLon(0.0), nmeaSentence("") {}

void GPS::start()
{
    gpsSerial.begin(9600);
}

void GPS::readGPS() 
{
    while (gpsSerial.available()) {
      char c = gpsSerial.read();
      nmeaSentence += c;
            
      // Check if we received a complete NMEA sentence
      if (c == '\n') {
        // Process the NMEA sentence
        if (nmeaSentence.startsWith("$GPGLL")) {
          parseGPGLL(nmeaSentence);
        }
              
        // Clear the sentence after processing
        nmeaSentence = "";
      }
    }
}  

void GPS::parseGPGLL(String nmea) 
{
    int commaIndex = nmea.indexOf(',');
    int fieldIndex = 0;
    String fields[10];
    
    // Split the sentence into fields
    while (commaIndex > 0) {
      fields[fieldIndex++] = nmea.substring(0, commaIndex);
      nmea = nmea.substring(commaIndex + 1);
      commaIndex = nmea.indexOf(',');
    }
    fields[fieldIndex] = nmea; // Last field after the last comma

    // For GPGLL sentence, fields are:
    // fields[1] = latitude, fields[2] = N/S, fields[3] = longitude, fields[4] = E/W
    if (fields[0] == "$GPGLL") {
      String latitude = fields[1];
      String latitudeDir = fields[2];
      String longitude = fields[3];
      String longitudeDir = fields[4];
      
      currentLat = convertToDecimalDegrees(latitude, latitudeDir, true);
      currentLon = convertToDecimalDegrees(longitude, longitudeDir, false);
    }
}

double GPS::convertToDecimalDegrees(String rawDegrees, String direction, bool isLatitude) 
{
    int degreesLength = isLatitude ? 2 : 3;
    double degrees = rawDegrees.substring(0, degreesLength).toDouble();
    double minutes = rawDegrees.substring(degreesLength).toDouble();
    double decimalDegrees = degrees + (minutes / 60.0);
    if (direction == "S" || direction == "W") {
      decimalDegrees *= -1;
    }
    return decimalDegrees;
}