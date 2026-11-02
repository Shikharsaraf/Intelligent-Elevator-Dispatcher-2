#pragma once
#include "data_structures.h"

struct Elevator {
    int id;
    int floor;
    int direction; // 1 = up, -1 = down, 0 = idle
    int door;
    int moved;
    int served;
    IntList stops;
    RequestList assigned;
    RequestList onboard;

    Elevator();
    bool hasStop(int target) const;
    void addStop(int target);
    int load() const;
    bool canTake() const;
    bool stopAhead(int travelDirection) const;
    int nearestDirection() const;
    int topStop() const;
    int bottomStop() const;
    void step(int time, PriorityQueue& pending);
};
