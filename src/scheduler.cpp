#include "scheduler.h"
#include <cstdlib>

int estimateTime(Elevator& e, Request* r) {
    int current = e.floor, target = r->from;
    if (e.stops.count == 0) return std::abs(current - target);

    int d = e.direction;
    if (d == 0) d = e.nearestDirection();
    int highest = e.topStop(), lowest = e.bottomStop();

    if (d == 1) {
        if (target >= current) return target - current;
        return (highest - current) + (highest - target);
    }
    if (target <= current) return current - target;
    return (current - lowest) + (target - lowest);
}

int calculateCost(Elevator& e, Request* r) {
    int d = e.direction;
    if (d == 0) d = e.nearestDirection();

    bool onWay = false;
    if (e.stops.count > 0) {
        if (d == 1 && r->from >= e.floor) onWay = true;
        if (d == -1 && r->from <= e.floor) onWay = true;
    }

    int score = estimateTime(e, r) + 2 * e.stops.count + 3 * e.load();
    if (onWay && r->to < r->from && d == 1)
        score += std::abs(e.topStop() - r->from) / 2;
    else if (onWay && r->to > r->from && d == -1)
        score += std::abs(r->from - e.bottomStop()) / 2;
    return score;
}

int chooseElevator(Elevator elevators[], Request* request) {
    int best = -1, bestScore = 1000000000;
    for (int i = 0; i < ELEVATORS; ++i) {
        if (!elevators[i].canTake()) continue;
        int score = calculateCost(elevators[i], request);
        if (score < bestScore) {
            bestScore = score;
            best = i;
        }
    }
    return best;
}
