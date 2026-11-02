#pragma once
#include "models.h"
#include "config.h"

struct IntNode {
    int data;
    IntNode* next;
};

struct IntList {
    IntNode* head;
    IntNode* tail;
    int count;
    IntList();
    ~IntList();
    IntList(const IntList&) = delete;
    IntList& operator=(const IntList&) = delete;
    void pushBack(int value);
    bool removeFirst(int value);
    bool contains(int value) const;
    void clear();
};

struct RequestNode {
    Request* data;
    RequestNode* next;
};

struct RequestList {
    RequestNode* head;
    RequestNode* tail;
    int count;
    RequestList();
    ~RequestList();
    RequestList(const RequestList&) = delete;
    RequestList& operator=(const RequestList&) = delete;
    void pushBack(Request* value);
    bool removeFirst(Request* value);
    void clear();
};

struct QueueNode {
    Request* data;
    QueueNode* next;
};

struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int count;
    Queue();
    ~Queue();
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
    bool isEmpty() const;
    void enqueue(Request* request);
    Request* dequeue();
};

struct PriorityQueue {
    Request* heap[MAX_REQUESTS];
    int count;
    PriorityQueue();
    bool isEmpty() const;
    bool comesBefore(const Request* a, const Request* b) const;
    void push(Request* request);
    Request* top() const;
    void pop();
};
