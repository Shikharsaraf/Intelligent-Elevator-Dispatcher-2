#pragma once
#include "elevator.h"

int estimateTime(Elevator& elevator, Request* request);
int calculateCost(Elevator& elevator, Request* request);
int chooseElevator(Elevator elevators[], Request* request);
