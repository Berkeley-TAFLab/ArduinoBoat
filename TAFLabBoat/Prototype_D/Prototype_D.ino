#include <SDCard.h>
#include <Atmosphere.h>
#include <WindVane.h>
#include <Sail.h>
#include <Rudder.h>
#include <ESC.h>
#include <Magnetometer.h>
#include <GPSCoordinates.h>
#include <GPSList.h>
#include <GPS.h>
#include <Control.h>
#include <Mesh.h>

unsigned long currentTime = 0;
float hard_iron[3] = {-17.1, -2.95, -18.41};
float soft_iron[3][3] = {
    {0.988, 0.042, -0.014},
    {0.042, 1.018, -0.006},
    {-0.014, -0.006, 0.996}
};

SDCard sd(53);
Atmosphere atm;
WindVane windVane(-2150, 1100, 2100, 3000);
Sail sail(10);
Rudder rudder(8);
ESC esc(9);
Magnetometer magnetometer(hard_iron, soft_iron);
GPSList gpsList;
GPS gps;
Control control(&windVane, &rudder, &esc, &magnetometer, &gpsList, &gps);
Mesh mesh(0, "0013A20041BEEF1E", &atm, &windVane, &sail, &rudder, &esc, &magnetometer, &gpsList, &gps, &control);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sd.start();
  atm.start();
  windVane.start();
  sail.start();
  rudder.start();
  esc.start();
  gps.start();
  mesh.start();
  // magnetometer.start();
}

void loop() {
  currentTime = millis();
  mesh.xbee_ReceiveDataFrame();
  gps.readGPS();

  windVane.getAngle(sail.currentSailPos);
  if (control.stationKeeping) {
    sail.calculateStationPos(windVane.currentAngle);
  } else {
    sail.calculateTargetPos(windVane.currentAngle);
  }
  sail.checkFullRotation(currentTime);
  if (currentTime - sail.lastSailTime > sail.sailInterval) {
    sail.moveSailTo();
    sail.lastSailTime = millis();
  }

  if (currentTime - rudder.lastRudderTime > rudder.rudderInterval) {
    rudder.moveRudderTo();
    rudder.lastRudderTime = millis();
  }

  // magnetometer.updateHeading();
  atm.updateData();

  control.autonomousControl(currentTime);

  if (currentTime - mesh.lastSendTime > mesh.sendInterval) {
    mesh.sendData(currentTime);
    sd.writeData(currentTime, gps.currentLat, gps.currentLon, windVane.calculateRealAngle(magnetometer.heading), atm.temp, atm.hum, atm.press, windVane.currentAngle, sail.currentSailPos, rudder.currentRudderPos, magnetometer.heading, control.bearingToDestination, gpsList.getSize());
    mesh.lastSendTime = millis();
  }
}
