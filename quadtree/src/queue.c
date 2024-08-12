#include "queue.h"

void queue_initialize(Queue* q, long capacity) {
    q->data = (long*)malloc(capacity * sizeof(long));
    q->front = 0;
    q->end = 0;
    q->size = 0;
    q->capacity = capacity;
}

void queue_destroy(Queue* q) {
    free(q->data);
}

void queue_enqueue(Queue* q, long value) {
    if (q->size == q->capacity) {
        fprintf(stderr, "queue_enqueue: queue is full\n");
        exit(1);
    }
    q->data[q->end] = value;
    q->end = (q->end + 1) % q->capacity;
    q->size++;
}

long queue_dequeue(Queue* q) {
    if (q->size == 0) {
        fprintf(stderr, "queue_dequeue: queue is empty\n");
        exit(1);
    }
    long value = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return value;
}