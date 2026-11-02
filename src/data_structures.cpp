#include "data_structures.h"
#include <algorithm>

IntList::IntList() : head(nullptr), tail(nullptr), count(0) {}
IntList::~IntList() { clear(); }

void IntList::pushBack(int value) {
    IntNode* node = new IntNode{value, nullptr};
    if (!head) head = node;
    else tail->next = node;
    tail = node;
    ++count;
}

bool IntList::removeFirst(int value) {
    IntNode* current = head;
    IntNode* previous = nullptr;
    while (current) {
        if (current->data == value) {
            if (previous) previous->next = current->next;
            else head = current->next;
            if (tail == current) tail = previous;
            delete current;
            --count;
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}

bool IntList::contains(int value) const {
    for (IntNode* p = head; p; p = p->next)
        if (p->data == value) return true;
    return false;
}

void IntList::clear() {
    while (head) {
        IntNode* old = head;
        head = head->next;
        delete old;
    }
    tail = nullptr;
    count = 0;
}

RequestList::RequestList() : head(nullptr), tail(nullptr), count(0) {}
RequestList::~RequestList() { clear(); }

void RequestList::pushBack(Request* value) {
    RequestNode* node = new RequestNode{value, nullptr};
    if (!head) head = node;
    else tail->next = node;
    tail = node;
    ++count;
}

bool RequestList::removeFirst(Request* value) {
    RequestNode* current = head;
    RequestNode* previous = nullptr;
    while (current) {
        if (current->data == value) {
            if (previous) previous->next = current->next;
            else head = current->next;
            if (tail == current) tail = previous;
            delete current;
            --count;
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}

void RequestList::clear() {
    while (head) {
        RequestNode* old = head;
        head = head->next;
        delete old;
    }
    tail = nullptr;
    count = 0;
}

Queue::Queue() : front(nullptr), rear(nullptr), count(0) {}
Queue::~Queue() { while (!isEmpty()) dequeue(); }

bool Queue::isEmpty() const { return front == nullptr; }

void Queue::enqueue(Request* request) {
    QueueNode* node = new QueueNode{request, nullptr};
    if (!rear) front = node;
    else rear->next = node;
    rear = node;
    ++count;
}

Request* Queue::dequeue() {
    if (isEmpty()) return nullptr;
    QueueNode* old = front;
    Request* result = old->data;
    front = front->next;
    if (!front) rear = nullptr;
    delete old;
    --count;
    return result;
}

PriorityQueue::PriorityQueue() : count(0) {}
bool PriorityQueue::isEmpty() const { return count == 0; }

bool PriorityQueue::comesBefore(const Request* a, const Request* b) const {
    if (a->prio != b->prio) return a->prio > b->prio;
    if (a->arrive != b->arrive) return a->arrive < b->arrive;
    return a->id < b->id;
}

void PriorityQueue::push(Request* request) {
    if (count >= MAX_REQUESTS) return;
    int i = count++;
    heap[i] = request;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (!comesBefore(heap[i], heap[parent])) break;
        std::swap(heap[i], heap[parent]);
        i = parent;
    }
}

Request* PriorityQueue::top() const {
    return count ? heap[0] : nullptr;
}

void PriorityQueue::pop() {
    if (!count) return;
    heap[0] = heap[count - 1];
    --count;
    int i = 0;
    while (true) {
        int left = 2 * i + 1, right = 2 * i + 2, best = i;
        if (left < count && comesBefore(heap[left], heap[best])) best = left;
        if (right < count && comesBefore(heap[right], heap[best])) best = right;
        if (best == i) break;
        std::swap(heap[i], heap[best]);
        i = best;
    }
}
