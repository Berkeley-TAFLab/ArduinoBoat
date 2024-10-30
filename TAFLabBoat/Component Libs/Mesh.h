#ifndef Mesh_h
#define Mesh_h

#include "Arduino.h"
#include "HardwareSerial.h"
#include "XBee.h"
#include "GPSList.h"
#include "GPS.h"
#include "SDCard.h"
#include "Atmosphere.h"
#include "Magnetometer.h"
#include "Sail.h"
#include "Rudder.h"
#include "ESC.h"
#include "WindVane.h"
#include "Control.h"

class Mesh
{
    public:
    unsigned long lastSendTime;
    unsigned long sendInterval = 200;
    Mesh(int d, String ma, Atmosphere* a, WindVane* wv, Sail* s, Rudder* r, ESC* e, Magnetometer* m, GPSList* gl, GPS* gp, Control* c);
    ~Mesh();
    void start();
    void sendData(unsigned long currentTime);
    XBeeAddress64 StringToXBeeAddress64(const String addrStr);
    void xbee_SendDataFrame(double data[], int dataLength, String addr);
    void xbee_ReceiveDataFrame();
    void parseData(double receivedData[], int len);
    private:
    XBee xbee;
    ZBRxResponse rxResponse;
    int id;
    String MAC_ADR;
    String landADR;
    String sendToADR;
    double* data;
    int dataCat;
    int nDataCat;
    int dataLength;
    Atmosphere* atm;
    WindVane* windVane;
    Sail* sail;
    Rudder* rudder;
    ESC* esc;
    Magnetometer* magnetometer;
    GPSList* gpsList;
    GPS* gps;
    Control* control;
};

#endif