#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include "rpn_calc.h"
#include "shunting_yard.h"
#include "tokenizer.h"

#define MAX_INPUT_SIZE 4096

static void read_input(char *s, size_t size);

int main(int argc, char *argv[])
{
	char *infix = (char *)malloc(sizeof(char) * MAX_INPUT_SIZE);
	read_input(infix, MAX_INPUT_SIZE);
	int err = false;
	char *tokens = NULL;
	if(!err && !(tokens = tokenize(infix, " "))) {
		fprintf(stderr, "error: failed to tokenize\n");
		err = true;
	}
	char *postfix = NULL;
	if (!err && !(postfix = infix_to_postfix(tokens))) {
		fprintf(stderr, "error: failed to convert to postfix\n");
		err = true;
	}
	double *rezult = NULL;
	if (!err && !(rezult = calculate(postfix))) {
		fprintf(stderr, "error: failed to calculate\n");
		err = true;
	}
	if (!err)
		printf("%g\n", *rezult);
	free(infix);
	free(tokens);
	free(postfix);
	free(rezult);
	return 0;
}

static void read_input(char *s, size_t size)
{
	s[size - 2] = '\0';
	if (fgets(s, size, stdin) == NULL) {
		printf("error: failed reading input\n");
		s[0] = '\0';
	} else if (s[size - 2] != '\0') {
		printf("error: too much input\n");
		s[0] = '\0';
	}
	char *new_line = strchr(s, '\0');
	if (new_line)
		*new_line = '\0';
}

