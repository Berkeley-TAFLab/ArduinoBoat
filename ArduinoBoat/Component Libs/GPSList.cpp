#include "Arduino.h"
#include "GPSCoordinates.h"
#include "HardwareSerial.h"
#include "GPSList.h"

GPSList::GPSList()
{
    GPSCoordinates* newCoordinate = new GPSCoordinates(0, 0, 0, 0, 0);
    sentinel = new Node(newCoordinate);  // Create the sentinel node with no data
    sentinel->next = sentinel;  // Sentinel points to itself (empty list)
    size = 0;  // Initialize size to 0
}

GPSList::Node::Node(GPSCoordinates* coordinates = nullptr) : data(coordinates), next(nullptr) {}

void GPSList::addLast(int id, double lat, double lon, int looping, int skTime) 
{
    char nodeID = (char)id;  // Convert int ID to char
    GPSCoordinates* newCoordinate = new GPSCoordinates(nodeID, lat, lon, looping, skTime);
    Node* newNode = new Node(newCoordinate);

    // Find the last node (the node before the sentinel)
    Node* last = sentinel;
    while (last->next != sentinel) {
        last = last->next;
    }

    // Insert the new node at the end, right before the sentinel
    last->next = newNode;
    newNode->next = sentinel;
    size++;  // Increment the size counter
}

void GPSList::deleteFirst() 
{
    if (sentinel->next == sentinel) {
        return;  // List is empty (sentinel points to itself)
    }
    Node* toDelete = sentinel->next;
    sentinel->next = toDelete->next;

    delete toDelete->data;  // Free the GPSCoordinates object
    delete toDelete;  // Free the node itself

    size--;  // Decrement the size counter
}

void GPSList::moveFirstToLast() 
{
    if (sentinel->next == sentinel || sentinel->next->next == sentinel) {
        return;  // List is empty or has only one element, no need to move
    }

    Node* firstNode = sentinel->next;
    sentinel->next = firstNode->next;  // Remove the first node from the front

    // Find the last node (the node before the sentinel)
    Node* last = sentinel;
    while (last->next != sentinel) {
        last = last->next;
    }

    last->next = firstNode;  // Attach the first node to the last node
    firstNode->next = sentinel;  // Update the first node's next to point to sentinel (end of list)
}

GPSCoordinates* GPSList::getFirst() 
{
    if (sentinel->next == sentinel) {
        return nullptr;  // List is empty
    }

    return sentinel->next->data;  // Return the first GPSCoordinates object
}

int GPSList::getSize() 
{
    return size;  // Return the current size of the list
}

void GPSList::clearList() 
{
    while (sentinel->next != sentinel) {
        deleteFirst();  // Repeatedly delete the first node
    }
    size = 0;  // Reset the size
}

GPSList::~GPSList() 
{
    while (sentinel->next != sentinel) {
        deleteFirst();  // Keep deleting the first node until the list is empty
    }
    delete sentinel;  // Delete the sentinel node
}