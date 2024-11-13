#include "Arduino.h"
#include "HardwareSerial.h"
#include "XBee.h"
#include "GPSList.h"
#include "GPS.h"
#include "Atmosphere.h"
#include "Magnetometer.h"
#include "Sail.h"
#include "Rudder.h"
#include "ESC.h"
#include "WindVane.h"
#include "Control.h"
#include "Mesh.h"

#define xbeeSerial Serial2

Mesh::Mesh(int d, String ma, Atmosphere* a, WindVane* wv, Sail* s, Rudder* r, ESC* e, Magnetometer* m, GPSList* gl, GPS* gp, Control* c) : xbee(XBee()), rxResponse(ZBRxResponse()), MAC_ADR(ma), landADR("0013A2004218639F"), sendToADR("0013A2004218639F"), dataCat(0), nDataCat(3), lastSendTime(0)
{
    id = d;
    atm = a;
    windVane = wv;
    sail = s;
    rudder = r;
    esc = e;
    magnetometer = m;
    gpsList = gl;
    gps = gp;
    control = c;
}

Mesh::~Mesh() 
{
      delete[] data;  // Free allocated memory for data array
}

void Mesh::start() 
{
    xbeeSerial.begin(115200);
    xbee.begin(xbeeSerial);
}

void Mesh::sendData(unsigned long currentTime) 
{
    int currentWindAngle = windVane->calculateRealAngle(magnetometer->heading);
    delete data;
    if (dataCat == 0) {
      dataLength = 9;
      data = new double[dataLength]{dataCat, id, id, gps->currentLat, gps->currentLon, windVane->currentAngle, sail->currentSailPos, rudder->currentRudderPos, magnetometer->heading};
    } else if (dataCat == 1) {
      dataLength = 7;
      data = new double[dataLength]{dataCat, id, id, currentWindAngle, atm->press, atm->hum, atm->temp};
    } else if (dataCat == 2) {
      dataLength = 9;
      data = new double[dataLength]{dataCat, id, id, gpsList->getSize(), control->stationKeeping, magnetometer->heading, control->bearingToDestination, control->distanceToDestination, gpsList->getFirst()->stationKeepingTime / 1000};
    }
    xbee_SendDataFrame(data, dataLength, sendToADR);
    dataCat = (dataCat + 1) % nDataCat;
}

XBeeAddress64 Mesh::StringToXBeeAddress64(const String addrStr) 
{
      // Ensure the string is the correct length
      if (addrStr.length() != 16) {
          // Return a default address if the string is invalid
          return XBeeAddress64();
      }
      
      // Split the string into high and low parts
      String highStr = addrStr.substring(0, 8);
      String lowStr = addrStr.substring(8, 16);
      
      // Convert the strings to 32-bit integers
      uint32_t high = strtoul(highStr.c_str(), NULL, 16);
      uint32_t low = strtoul(lowStr.c_str(), NULL, 16);
      
      // Create and return an XBeeAddress64 object
      return XBeeAddress64(high, low);
}

void Mesh::xbee_SendDataFrame(double data[], int dataLength, String addr) 
{
    uint8_t payload[dataLength * sizeof(double)];
    memcpy(payload, data, dataLength * sizeof(double));
    XBeeAddress64 addr64 = StringToXBeeAddress64(addr);
    ZBTxRequest txRequest = ZBTxRequest(addr64, payload, sizeof(payload));
    
    // Send the message
    xbee.send(txRequest);

    // Check the status of the transmission
    // if (xbee.readPacket(500)) {
    //   if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
    //     ZBTxStatusResponse txStatus;
    //     xbee.getResponse().getZBTxStatusResponse(txStatus);
    //     if (txStatus.getDeliveryStatus() == SUCCESS) {
          // Serial.println("Success");
    //     } else {
          // Serial.println("Failure");
    //     }
    //   }
    // } else {
      // Serial.println("Transmission timeout");
    // }
}

void Mesh::xbee_ReceiveDataFrame() 
{
    xbee.readPacket();

    if (xbee.getResponse().isAvailable()) {
      // Check if we received a ZB RX packet
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        xbee.getResponse().getZBRxResponse(rxResponse);

        // Get the byte data
        int byteDataLength = rxResponse.getDataLength();
        uint8_t* byteData = rxResponse.getData();

        // Convert the byte array back into a double array
        int numDoubles = byteDataLength / sizeof(double);
        double receivedData[numDoubles];
        memcpy(receivedData, byteData, byteDataLength);

        // Print the received double array
        // for (int i = 0; i < numDoubles; i++) {
        //   Serial.print("Received double: ");
        //   Serial.println(receivedData[i], 6); // Print with 6 decimal places
        // }
        parseData(receivedData, numDoubles);
      }
    } else if (xbee.getResponse().isError()) {
      // Serial.print("Error reading packet. Error code: ");
      // Serial.println(xbee.getResponse().getErrorCode());
    }
}

void Mesh::parseData(double receivedData[], int len) 
{
    if (receivedData[0] == -1) {
      if (receivedData[1] == -1) {
        gpsList->clearList();
      } else {
        if (gpsList->getSize() == 0) {
          rudder->targetRudderPos = (int) receivedData[1];
        }
        esc->setThrottle(map((int) receivedData[2], 90, 180, 1000, 2000));
      }
    } else if (receivedData[0] == -2) {
      if (receivedData[1] == 1) {
        gpsList->deleteFirst();
      } else if (receivedData[1] == 0) {
        gpsList->addLast((int) receivedData[2], receivedData[3], receivedData[4], (int) receivedData[5], (int) receivedData[6] * 1000);
      }
    }
}