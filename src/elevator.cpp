#include "elevator.h"
#include "config.h"
#include <cstdlib>

Elevator::Elevator()
    : id(0), floor(0), direction(0), door(0), moved(0), served(0) {}

bool Elevator::hasStop(int target) const { return stops.contains(target); }

void Elevator::addStop(int target) {
    if (!hasStop(target)) stops.pushBack(target);
}

int Elevator::load() const { return assigned.count + onboard.count; }

bool Elevator::canTake() const {
    return assigned.count < MAX_ASSIGNED && load() < CAPACITY;
}

bool Elevator::stopAhead(int d) const {
    for (IntNode* p = stops.head; p; p = p->next)
        if ((p->data - floor) * d > 0) return true;
    return false;
}

int Elevator::nearestDirection() const {
    int bestDistance = 1000, bestDirection = 0;
    for (IntNode* p = stops.head; p; p = p->next) {
        int distance = std::abs(p->data - floor);
        if (distance < bestDistance) {
            bestDistance = distance;
            bestDirection = (p->data > floor) ? 1 : (p->data < floor ? -1 : 0);
        }
    }
    return bestDirection;
}

int Elevator::topStop() const {
    int highest = floor;
    for (IntNode* p = stops.head; p; p = p->next)
        if (p->data > highest) highest = p->data;
    return highest;
}

int Elevator::bottomStop() const {
    int lowest = floor;
    for (IntNode* p = stops.head; p; p = p->next)
        if (p->data < lowest) lowest = p->data;
    return lowest;
}

void Elevator::step(int time, PriorityQueue& pending) {
    if (door > 0) { --door; return; }
    if (stops.count == 0) { direction = 0; return; }

    if (hasStop(floor)) {
        stops.removeFirst(floor);

        RequestNode* p = onboard.head;
        while (p) {
            RequestNode* next = p->next;
            Request* r = p->data;
            if (r->to == floor) {
                r->drop = time;
                ++served;
                onboard.removeFirst(r);
            }
            p = next;
        }

        p = assigned.head;
        while (p) {
            RequestNode* next = p->next;
            Request* r = p->data;
            if (r->from == floor && onboard.count < CAPACITY) {
                r->pickup = time;
                onboard.pushBack(r);
                addStop(r->to);
                assigned.removeFirst(r);
            }
            p = next;
        }

        p = assigned.head;
        while (p) {
            RequestNode* next = p->next;
            Request* r = p->data;
            if (r->from == floor) {
                pending.push(r);
                assigned.removeFirst(r);
            }
            p = next;
        }
        door = DOOR_TICKS;
        return;
    }

    if (direction == 0) direction = nearestDirection();
    else if (!stopAhead(direction)) direction = -direction;

    floor += direction;
    ++moved;
}
