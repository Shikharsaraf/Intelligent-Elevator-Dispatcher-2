#include "simulation.h"
#include "config.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <string>

Simulation::Simulation() : passengerCount(0), globalTime(0) {
    for (int i = 0; i < ELEVATORS; ++i) {
        elevators[i].id = i;
        elevators[i].floor = (i * (FLOORS - 1)) / ELEVATORS;
    }
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

int Simulation::readInt(const std::string& prompt, int low, int high) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= low && value <= high) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter a number from " << low << " to " << high << ".\n";
    }
}

void Simulation::makeScenario(int type, int n, int seed, Request output[], int arrivalBase) {
    std::srand(seed);
    for (int i = 0; i < n; ++i) {
        Request r;
        r.id = i + 1;
        r.arrive = arrivalBase + std::rand() % (3 * n / 2 + 1);
        int p = std::rand() % 100 + 1;

        if (type == 1) {
            if (p <= 65) { r.from = 0; r.to = std::rand() % 9 + 1; }
            else if (p <= 85) { r.from = std::rand() % 9 + 1; r.to = std::rand() % 10; }
            else { r.from = std::rand() % 9 + 1; r.to = 0; }
        } else if (type == 2) {
            if (p <= 65) { r.from = std::rand() % 9 + 1; r.to = 0; }
            else if (p <= 85) { r.from = std::rand() % 10; r.to = std::rand() % 10; }
            else { r.from = 0; r.to = std::rand() % 9 + 1; }
        } else {
            r.from = std::rand() % 10;
            r.to = std::rand() % 10;
        }

        if (r.from == r.to) r.to = (r.from + 1 + std::rand() % 9) % 10;
        r.prio = (std::rand() % 100 < 10) ? 2 : 1;
        r.pickup = r.drop = -1;
        output[i] = r;
    }
}

void Simulation::draw(int time, PriorityQueue& pending, int notArrived) {
    std::cout << "\n=== t = " << time << "s | not yet arrived: "
              << notArrived << " | waiting for dispatch: " << pending.count << " ===\n";
    for (int f = FLOORS - 1; f >= 0; --f) {
        int waiting = 0;
        for (int i = 0; i < pending.count; ++i)
            if (pending.heap[i]->from == f) ++waiting;
        for (int i = 0; i < ELEVATORS; ++i)
            for (RequestNode* p = elevators[i].assigned.head; p; p = p->next)
                if (p->data->from == f) ++waiting;

        std::cout << "F" << f << " ";
        for (int i = 0; i < ELEVATORS; ++i) {
            if (elevators[i].floor == f) {
                std::string symbol = elevators[i].door ? "*" :
                    (elevators[i].direction == 1 ? "^" :
                    (elevators[i].direction == -1 ? "v" : "-"));
                std::string label = "[E" + std::to_string(i + 1) + symbol +
                                    std::to_string(elevators[i].onboard.count) + "]";
                std::cout << std::setw(7) << label;
            } else std::cout << "   |   ";
        }
        std::cout << "  " << std::string(waiting, 'o') << "\n";
    }
    std::cout << "   (^ up, v down, * doors open, - idle; number = onboard; o = waiting)\n";
}

Result Simulation::simulate(int startIndex, int n, bool animate) {
    for (int i = startIndex + 1; i < startIndex + n; ++i) {
        Request key = history[i];
        int j = i - 1;
        while (j >= startIndex && history[j].arrive > key.arrive) {
            history[j + 1] = history[j];
            --j;
        }
        history[j + 1] = key;
    }

    Queue incoming;
    for (int i = startIndex; i < startIndex + n; ++i)
        incoming.enqueue(&history[i]);

    PriorityQueue pending;
    int movedBefore = 0;
    for (auto& elevator : elevators) movedBefore += elevator.moved;

    for (; globalTime < 20000; ++globalTime) {
        while (!incoming.isEmpty() && incoming.front->data->arrive <= globalTime)
            pending.push(incoming.dequeue());

        while (!pending.isEmpty()) {
            Request* request = pending.top();
            int selected = chooseElevator(elevators, request);
            if (selected == -1) break;
            pending.pop();
            elevators[selected].assigned.pushBack(request);
            elevators[selected].addStop(request->from);
        }

        for (auto& elevator : elevators) elevator.step(globalTime, pending);

        if (animate) {
            draw(globalTime, pending, incoming.count);
            std::string input;
            std::getline(std::cin, input);
            if (input == "q") break;
        }

        int completed = 0;
        for (int i = startIndex; i < startIndex + n; ++i)
            if (history[i].drop >= 0) ++completed;
        if (completed == n) break;
    }

    Result result;
    result.makespan = globalTime;
    int completed = 0;
    for (int i = startIndex; i < startIndex + n; ++i) {
        Request& r = history[i];
        if (r.drop >= 0) {
            int wait = r.pickup - r.arrive;
            result.avgWait += wait;
            result.avgTrip += r.drop - r.arrive;
            if (wait > result.maxWait) result.maxWait = wait;
            ++completed;
        }
    }
    if (completed) {
        result.avgWait /= completed;
        result.avgTrip /= completed;
    }

    int minServed = std::numeric_limits<int>::max(), maxServed = 0, movedNow = 0;
    for (auto& elevator : elevators) {
        movedNow += elevator.moved;
        if (elevator.served < minServed) minServed = elevator.served;
        if (elevator.served > maxServed) maxServed = elevator.served;
    }
    result.moved = movedNow - movedBefore;
    result.imbalance = maxServed - minServed;
    return result;
}

void Simulation::showStatus() const {
    std::cout << "\nCurrent simulation time: " << globalTime << " seconds\n";
    for (int i = 0; i < ELEVATORS; ++i) {
        const Elevator& e = elevators[i];
        std::cout << "Elevator " << i + 1 << " | floor: " << e.floor
                  << " | direction: " << e.direction << " | onboard: "
                  << e.onboard.count << " | assigned: " << e.assigned.count
                  << " | stops: ";
        if (!e.stops.head) std::cout << "none";
        for (IntNode* p = e.stops.head; p; p = p->next) std::cout << p->data << " ";
        std::cout << "\n";
    }
    std::cout << "\nPassenger history (" << passengerCount << " requests):\n";
    for (int i = 0; i < passengerCount; ++i) {
        const Request& r = history[i];
        std::cout << "ID " << r.id << ": F" << r.from << " -> F" << r.to
                  << " | priority " << r.prio << " | arrival " << r.arrive
                  << " | pickup " << r.pickup << " | drop " << r.drop << "\n";
    }
}

void Simulation::runMenu() {
    std::cout << "INTELLIGENT ELEVATOR DISPATCHER (" << FLOORS
              << " floors, " << ELEVATORS << " elevators)\n";
    while (true) {
        std::cout << "\n1. Live simulation with random passengers\n"
                  << "2. Live simulation with ONE manual + random passengers\n"
                  << "3. Display remembered elevator and passenger status\n"
                  << "4. Data structures used\n0. Exit\n";
        int choice = readInt("Choose: ", 0, 4);
        if (choice == 0) break;
        if (choice == 4) {
            std::cout << "\nQueue: incoming requests wait in arrival order (FIFO).\n"
                      << "Priority Queue: urgent requests are selected first (binary max-heap).\n"
                      << "Linked Lists: elevator stops, assigned and onboard passengers.\n"
                      << "Insertion Sort: orders each new batch by arrival time.\n"
                      << "Scheduling: Smart Cost strategy.\n";
            continue;
        }
        if (choice == 3) { showStatus(); continue; }

        int n = readInt("Total passengers for this run (2-50): ", 2, 50);
        if (passengerCount + n > MAX_REQUESTS) {
            std::cout << "Passenger history is full. Maximum is " << MAX_REQUESTS << ".\n";
            continue;
        }

        int startIndex = passengerCount;
        int manualCount = (choice == 2) ? 1 : 0;
        if (manualCount) {
            Request r;
            r.id = passengerCount + 1;
            r.arrive = globalTime;
            r.from = readInt("Passenger starting floor (0-9): ", 0, FLOORS - 1);
            r.to = readInt("Passenger destination floor (0-9): ", 0, FLOORS - 1);
            while (r.to == r.from) {
                std::cout << "Destination must differ from starting floor.\n";
                r.to = readInt("Passenger destination floor (0-9): ", 0, FLOORS - 1);
            }
            r.prio = readInt("Priority (1 = normal, 2 = high): ", 1, 2);
            history[passengerCount++] = r;
        }

        int randomCount = n - manualCount;
        if (randomCount > 0) {
            Request generated[MAX_REQUESTS];
            int scenario = readInt("Random passengers scenario (1 morning, 2 evening, 3 random): ", 1, 3);
            makeScenario(scenario, randomCount, 42 + passengerCount, generated, globalTime);
            for (int i = 0; i < randomCount; ++i) {
                generated[i].id = passengerCount + 1;
                history[passengerCount++] = generated[i];
            }
        }

        std::cout << "\nRunning Smart Cost scheduling...\n";
        Result result = simulate(startIndex, n, true);
        std::cout << "\nRun completed.\n"
                  << "Average wait: " << std::fixed << std::setprecision(1) << result.avgWait << " s\n"
                  << "Average trip: " << result.avgTrip << " s\n"
                  << "Maximum wait: " << result.maxWait << " s\n"
                  << "Total floors travelled in this run: " << result.moved << "\n"
                  << "Passenger records and elevator positions are retained.\n";
    }
}
