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
float hard_iron[3] = {-10.09, -8.15, -26.11};
float soft_iron[3][3] = {
    {1.000, 0.034, -0.002},
    {0.034, 1.012, -0.004},
    {-0.002, -0.004, 0.989}
};


// SDCard sd(53);
Atmosphere atm;
WindVane windVane(-4000, 1000, 2000, 2900);
Sail sail(8);
Rudder rudder(10);
ESC esc(9);
Magnetometer magnetometer(hard_iron, soft_iron);
GPSList gpsList;
GPS gps;
Control control(&windVane, &rudder, &esc, &magnetometer, &gpsList, &gps);
Mesh mesh(3, "0013A20041BEEF1E", &atm, &windVane, &sail, &rudder, &esc, &magnetometer, &gpsList, &gps, &control);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  // sd.start();
  atm.start();
  windVane.start();
  sail.start(270);
  rudder.start();
  esc.start();
  magnetometer.start();
  gps.start();
  mesh.start();
}

void loop() {
  currentTime = millis();
  mesh.xbee_ReceiveDataFrame();
  gps.readGPS();

  magnetometer.updateHeading();
  atm.updateData();
  windVane.getAngle(sail.currentSailPos);
  windVane.currentAngle = map(windVane.currentAngle, 0, 360, 360, 0);
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

  control.autonomousControl(currentTime);

  if (currentTime - mesh.lastSendTime > mesh.sendInterval) {
    mesh.sendData(currentTime);
    // sd.writeData(currentTime, gps.currentLat, gps.currentLon, windVane.calculateRealAngle(magnetometer.heading), atm.temp, atm.hum, atm.press, windVane.currentAngle, sail.currentSailPos, rudder.currentRudderPos, magnetometer.heading, control.bearingToDestination, gpsList.getSize());
    mesh.lastSendTime = millis();
  }
}
