#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    long* data;
    long front;
    long end;
    long size;
    long capacity;
} Queue;

void queue_initialize(Queue* q, long capacity);
void queue_destroy(Queue* q);
void queue_enqueue(Queue* q, long value);
long queue_dequeue(Queue* q);

#endif