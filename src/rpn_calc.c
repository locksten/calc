#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "stack.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

static int exec_operator(stack *_stack, char _operator[]);
static int exec_constant(stack *_stack, char _operator[]);
static int exec_1_oper(stack *_stack, char _operator[]);
static int exec_2_oper(stack *_stack, char _operator[]);
static int is_number(char s[]);

static int exec_operator(stack *_stack, char _operator[])
{
	if (!exec_constant(_stack, _operator))
		return 0;

	if (_stack->size < 1) {
		fprintf(stderr, "error: too few arguments\n");
		return -1;
	}
	if (!exec_1_oper(_stack, _operator))
		return 0;

	if (_stack->size < 2) {
		fprintf(stderr, "error: too few arguments\n");
		return -1;
	}
	if (!exec_2_oper(_stack, _operator))
		return 0;

	fprintf(stderr, "error: unrecognized operator\n");
	return -1;
}

double *calculate(char *input)
{
	stack *values;
	values = stack_new();
	char *inputp = input;
	int num_read = 0;
	while (*inputp != '\0') {
		if (isspace(*inputp)) {
			++inputp;
		} else if (is_number(inputp)) {
			double *operand = (double *)malloc(sizeof(double));
			sscanf(inputp, "%lf%n", operand, &num_read);
			inputp += num_read;
			stack_push(values, operand);
		} else if (ispunct(*inputp)) {
			char _operator[2];
			_operator[0]= *inputp;
			_operator[1]= '\0';
			if (exec_operator(values, _operator)) {
				stack_free(values);
				return NULL;
			}
			++inputp;
		} else if (isalpha(*inputp)) {
			char *_operator = (char *)malloc(sizeof(char) * 128);
			sscanf(inputp, "%[a-z] %n", _operator, &num_read);
			inputp += num_read;
			if (exec_operator(values, _operator)) {
				stack_free(values);
				free(_operator);
				return NULL;
			}
			free(_operator);
		} else {
			++inputp;
		}
	}
	if (values->size != 1) {
		fprintf(stderr, "error: incorrect number of operands\n");
		stack_free(values);
		return NULL;
	}
	double *rezult = (double *)stack_pop(values);
	stack_free(values);
	return rezult;
}

static int exec_constant(stack *_stack, char _operator[]) {
	double *answer = (double *)malloc(sizeof(double));
	if (strcmp(_operator, "pi") == 0)
		*answer = M_PI;
	else if (strcmp(_operator, "tau") == 0)
		*answer = 2 * M_PI;
	else if (strcmp(_operator, "e") == 0)
		*answer = M_E;
	else {
		free(answer);
		return -1;
	}
	stack_push(_stack, answer);
	return 0;
}

static int exec_1_oper(stack *_stack, char _operator[]) {
	double *op = (double *)stack_pop(_stack);
	double *answer = (double *)malloc(sizeof(double));
	if (strcmp(_operator, "sqrt") == 0)
		*answer = sqrt(*op);
	else if (strcmp(_operator, "ln") == 0)
		*answer = log(*op);
	else if (strcmp(_operator, "lb") == 0)
		*answer = log2(*op);
	else if (strcmp(_operator, "lg") == 0)
		*answer = log10(*op);
	else if (strcmp(_operator, "cos") == 0)
		*answer = cos(*op);
	else if (strcmp(_operator, "sin") == 0)
		*answer = sin(*op);
	else if (strcmp(_operator, "tan") == 0)
		*answer = tan(*op);
	else if (strcmp(_operator, "acos") == 0)
		*answer = acos(*op);
	else if (strcmp(_operator, "asin") == 0)
		*answer = asin(*op);
	else if (strcmp(_operator, "atan") == 0)
		*answer = atan(*op);
	else if (strcmp(_operator, "abs") == 0)
		*answer = fabs(*op);
	else if (strcmp(_operator, "floor") == 0)
		*answer = floor(*op);
	else if (strcmp(_operator, "ceil") == 0)
		*answer = ceil(*op);
	else {
		stack_push(_stack, op);
		free(answer);
		return -1;
	}
	free(op);
	stack_push(_stack, answer);
	return 0;
}

static int exec_2_oper(stack *_stack, char _operator[]) {
	double *op1 = (double *)stack_pop(_stack);
	double *op2 = (double *)stack_pop(_stack);
	double *answer = (double *)malloc(sizeof(double));
	if (strcmp(_operator, "*") == 0)
		*answer = *op2 * *op1;
	else if (strcmp(_operator, "/") == 0)
		*answer = *op2 / *op1;
	else if (strcmp(_operator, "+") == 0)
		*answer = *op2 + *op1;
	else if (strcmp(_operator, "-") == 0)
		*answer = *op2 - *op1;
	else if (strcmp(_operator, "^") == 0)
		*answer = pow(*op2, *op1);
	else if (strcmp(_operator, "%") == 0)
		*answer = fmod(*op2, *op1);
	else {
		stack_push(_stack, op2);
		stack_push(_stack, op1);
		free(answer);
		return -1;
	}
	free(op1);
	free(op2);
	stack_push(_stack, answer);
	return 0;
}

static int is_number(char s[])
{
	if (isdigit(s[0]))
		return 1;
	if (s[0] == '-' && isdigit(s[1]))
		return 1;
	if (s[0] == '.' && isdigit(s[1]))
		return 1;
	if (strlen(s) > 1 && s[0] == '-' && s[1] == '.' && isdigit(s[2]))
		return 1;
	return 0;
}

