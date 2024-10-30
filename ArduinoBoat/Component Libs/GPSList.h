#ifndef GPSList_h
#define GPSList_h

#include "Arduino.h"
#include "GPSCoordinates.h"
#include "HardwareSerial.h"

class GPSList
{
    public:
    GPSList();
    void addLast(int id, double lat, double lon, int looping, int skTime);
    void deleteFirst();
    void moveFirstToLast();
    GPSCoordinates* getFirst();
    int getSize();
    void clearList();
    ~GPSList();
    private:
    class Node
    {
        public:
        GPSCoordinates* data;
        Node* next;
        Node(GPSCoordinates* coordinates = nullptr);
    };
    Node* sentinel;
    int size;
};

#endif