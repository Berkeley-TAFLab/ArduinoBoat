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
float hard_iron[3] = {-18.95, -2.88, -39.35};
float soft_iron[3][3] = {
  {0.984, 0.03, -0.005},
  {0.03, 0.996, 0.002},
  {-0.005, 0.002, 1.021}
};

SDCard sd(53);
Atmosphere atm;
WindVane windVane(-900, 900, 1900, 2900, true);
Sail sail(8);
Rudder rudder(10);
ESC esc(9);
Magnetometer magnetometer(hard_iron, soft_iron);
GPSList gpsList;
GPS gps;
Control control(&windVane, &rudder, &esc, &magnetometer, &gpsList, &gps);
Mesh mesh(0, "0013A2004233185B", &atm, &windVane, &sail, &rudder, &esc, &magnetometer, &gpsList, &gps, &control);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  atm.start();
  windVane.start();
  sail.start(180);
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
      mesh.lastSendTime = millis();
  }
}
