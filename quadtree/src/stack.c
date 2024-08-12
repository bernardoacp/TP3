#include "stack.h"

void stack_initialize(Stack* s, long capacity) {
    s->data = (long*) malloc(capacity * sizeof(long));
    s->top = -1;
    s->size = 0;
    s->capacity = capacity;
}

void stack_destroy(Stack* s) {
    free(s->data);
}

void stack_push(Stack* s, long value) {
    if (s->size == s->capacity) {
        fprintf(stderr, "stack_push: stack is full\n");
        exit(1);
    }
    s->data[++s->top] = value;
    s->size++;
}

long stack_pop(Stack* s) {
    if (s->size == 0) {
        fprintf(stderr, "stack_pop: stack is empty\n");
        exit(1);
    }
    s->size--;
    return s->data[s->top--];
}
