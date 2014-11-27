#ifndef STACK_H
#define STACK_H

#include <stddef.h>

typedef struct node {
	void *data;
	struct node *next;
} node;

typedef struct stack {
	node *head;
	size_t size;
} stack;

stack *stack_new();
void stack_free(stack *self);
void* stack_pop(stack *self);
void stack_push(stack *self, void *data);

#endif
