#pragma once

struct Request {
    int id = 0;
    int arrive = 0;
    int from = 0;
    int to = 0;
    int prio = 1;
    int pickup = -1;
    int drop = -1;
};

struct Result {
    double avgWait = 0.0;
    double avgTrip = 0.0;
    int maxWait = 0;
    int makespan = 0;
    int moved = 0;
    int imbalance = 0;
};
