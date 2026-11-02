#pragma once
#include "scheduler.h"
#include <string>

class Simulation {
public:
    Simulation();
    void runMenu();
private:
    Request history[MAX_REQUESTS];
    int passengerCount;
    int globalTime;
    Elevator elevators[ELEVATORS];

    int readInt(const std::string& prompt, int low, int high);
    void makeScenario(int type, int count, int seed, Request output[], int arrivalBase);
    void draw(int time, PriorityQueue& pending, int notArrived);
    Result simulate(int startIndex, int count, bool animate);
    void showStatus() const;
};
