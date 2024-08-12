#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    long* data;
    long top;
    long size;
    long capacity;
} Stack;

void stack_initialize(Stack* s, long capacity);
void stack_destroy(Stack* s);
void stack_push(Stack* s, long value);
long stack_pop(Stack* s);

#endif