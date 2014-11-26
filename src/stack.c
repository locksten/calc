#include "../include/stack.h"
#include <stdlib.h>
#include <stdio.h>

stack *stack_new()
{
	stack *new_stack = (stack *)malloc(sizeof(stack));
	if (!new_stack) {
		fprintf(stderr, "%s:%d: error: "
			"failed to allocate memory\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	new_stack->head = NULL;
	new_stack->size = 0;
	return new_stack;
}

void stack_free(stack *self)
{
	node *p = self->head;
	node *last = self->head;
	while (p != NULL) {
		last = self->head;
		self->head = self->head->next;
		p = p->next;
		free(last->data);
		free(last);
	}
	free(self);
}

void* stack_pop(stack *self)
{
	if (self->head == NULL) return NULL;
	--self->size;
	void *tmp_data = self->head->data;
	node *tmp_node = self->head;
	self->head = self->head->next;
	free(tmp_node);
	return tmp_data;
}

void stack_push(stack *self, void *data)
{
	node *new_node = (node *)malloc(sizeof(node));
	if (!new_node) {
		fprintf(stderr, "%s:%d: error: "
			"failed to allocate memory\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	++self->size;
	if (self->head == NULL) {
		new_node->next = NULL;
		new_node->data = data;
		self->head = new_node;
	} else {
		new_node->next = self->head;
		self->head = new_node;
		new_node->data = data;
	}
}
