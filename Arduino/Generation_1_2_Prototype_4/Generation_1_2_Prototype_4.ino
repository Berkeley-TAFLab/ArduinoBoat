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
float hard_iron[3] = {3.3, -4.98, -46.28};
float soft_iron[3][3] = {
  {0.986, 0.024, -0.018},
  {0.024, 0.996, -0.008},
  {-0.018, -0.008, 1.018}
};

SDCard sd(53);
Atmosphere atm;
WindVane windVane(-1200, 900, 1900, 2900, true);
Sail sail(8);
Rudder rudder(10);
ESC esc(9);
Magnetometer magnetometer(hard_iron, soft_iron);
GPSList gpsList;
GPS gps;
Control control(&windVane, &rudder, &esc, &magnetometer, &gpsList, &gps);
Mesh mesh(3, "0013A200423319C0", &atm, &windVane, &sail, &rudder, &esc, &magnetometer, &gpsList, &gps, &control);

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
