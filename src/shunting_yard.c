#include "shunting_yard.h"
#include "stack.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_IN_STR_LEN 128
#define STARTING_QUEUE_SIZE 256
#define BUFFER_INC_RATIO 2
#define DELIMITER " "

typedef struct oper_info {
	char *str;
	int precedence;
	int association;
} oper_info;

enum { LEFT, RIGHT };
enum { NUMBER, FUNC, OPER, SEPARATOR, L_PARENTH, R_PARENTH };

int operator_compare(oper_info *oper_info, size_t oper_info_size,
	char o1[], char o2[]);
int pop_to_q_until_l_parenth (stack *operators, char *queue[],
	size_t *q_size);
int handle_r_parenth(char token[], stack *operators, char *queue[],
	size_t *q_size);
int handle_operator(char token[], stack *operators, char *queue[],
	size_t *q_size, oper_info *oper_info, size_t oper_info_size);
char *opers_top(stack *operators);
static char *dynamic_cat(char s[], size_t *size, char in[]);
static void enqueue(char *queue[], size_t *size, char token[]);
static int is_token_type(char token[], int type);
static int is_numeric(char s[]);
static void push_oper(stack *operators, char s[]);
static char *pop_oper(stack *operators);
static int association(oper_info *op_info, size_t size, char s[]);
static int precedence(oper_info *op_info, size_t size, char s[]);
static char *get_token(char *s[]);
static oper_info *init_oper_info(size_t *size);

char *infix_to_postfix(char input[])
{
	stack *operators = stack_new();
	size_t q_size = STARTING_QUEUE_SIZE;
	char *queue = (char *)malloc(sizeof(char) * q_size);
	queue[0] = '\0';
	size_t oper_info_size;
	oper_info *oper_info = init_oper_info(&oper_info_size);

	int error = false;
	char *token;
	while (!error && (token = get_token(&input))) {
		if (is_token_type(token, NUMBER)) {
			enqueue(&queue, &q_size, token);
			free(token);
		} else if (is_token_type(token, FUNC)) {
			push_oper(operators, token);
			free(token);
		} else if (is_token_type(token, SEPARATOR)) {
			if (pop_to_q_until_l_parenth(operators,
					&queue, &q_size)) {
				fprintf(stderr,
					"error: mismatched parentheses\n");
				error = true;
			}
			free(token);
		} else if (is_token_type(token, OPER)) {
			if (handle_operator(token, operators, &queue, &q_size,
					oper_info, oper_info_size)) {
				fprintf(stderr,
					"error: unrecognized operator\n");
				error = true;
			}
		} else if (is_token_type(token, L_PARENTH)) {
			push_oper(operators, token);
			free(token);
		} else if (is_token_type(token, R_PARENTH)) {
			if (handle_r_parenth(token, operators,
					&queue, &q_size)) {
				fprintf(stderr,
					"error: mismatched parentheses\n");
				error = true;
			}
		} else {
			free(token);
			fprintf(stderr, "error: unrecognized token\n");
			error = true;
		}
	}
	while (!error && is_token_type(opers_top(operators), OPER)) {
		char *tmp = pop_oper(operators);
		enqueue(&queue, &q_size, tmp);
		free(tmp);
	}
	if (!error && (is_token_type(opers_top(operators), L_PARENTH)
		|| is_token_type(opers_top(operators), R_PARENTH))) {
		fprintf(stderr, "error: mismatched parentheses\n");
		error = true;
	}
	if (!error && (operators->size != 0)) {
		fprintf(stderr, "error: mismatched parentheses"
			" or number of functions\n");
		error = true;
	}

	for (size_t i = 0; i < oper_info_size; ++i) {
		free((oper_info + i)->str);
	}
	free(oper_info);
	stack_free(operators);
	if (error) {
		free(queue);
		return NULL;
	}
	return queue;
}

static char *dynamic_cat(char s[], size_t *size, char in[])
{
	while (&s[*size - 1] - strchr(s, '\0') < strlen(in)) {
		char *tmp = (char *)realloc(s, *size *= BUFFER_INC_RATIO);
		if (!tmp)
			return NULL;
		else
			s = tmp;
	}
	strcat(s, in);
	return s;
}

static void enqueue(char *queue[], size_t *size, char token[])
{
	*queue = dynamic_cat(*queue, size, token);
	if (!*queue) {
		fprintf(stderr, "%s:%d: error: "
			"failed to allocate memory\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	*queue = dynamic_cat(*queue, size, DELIMITER);
	if (!*queue) {
		fprintf(stderr, "%s:%d: error: "
			"failed to allocate memory\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
}

static char *get_token(char *s[])
{
	char *tmp = (char *)malloc(sizeof(char) * MAX_IN_STR_LEN);
	if (!tmp) {
		fprintf(stderr, "%s:%d: error: "
			"failed to allocate memory\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	int n_moved;
	if (sscanf(*s, "%s%n", tmp, &n_moved) == EOF) {
		free(tmp);
		return NULL;
	}
	*s += n_moved;
	return tmp;
}

static void push_oper(stack *operators, char s[])
{
	char *tmp = (char *)malloc(sizeof(char) * (strlen(s) + 1));
	if (!tmp) {
		fprintf(stderr, "%s:%d: error: "
			"failed to allocate memory\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	strcpy(tmp, s);
	stack_push(operators, tmp);
}

static char *pop_oper(stack *operators)
{
	return (char *)stack_pop(operators);
}

static oper_info *init_oper_info(size_t *size)
{
	*size = 6;
	char operators_str[] = {"^ 1 4\n% 0 3\n* 0 3\n/ 0 3\n+ 0 2\n- 0 2\n"};

	oper_info *operators = (oper_info *)malloc(sizeof(oper_info) * *size);
	char *p = operators_str;
	for (size_t i = 0; i < *size; ++i) {
		oper_info tmp;
		tmp.str = (char *)malloc(sizeof(char) * MAX_IN_STR_LEN);
		if (!tmp.str) {
			fprintf(stderr, "%s:%d: error: "
				"failed to allocate memory\n",
					__FILE__, __LINE__);
			exit(EXIT_FAILURE);
		}
		int n_moved = 0;
		sscanf(p, "%s %d %d%n", tmp.str, &tmp.association,
			&tmp.precedence, &n_moved);
		p += n_moved;
		operators[i] = tmp;
	}
	return operators;
}

static int is_token_type(char token[], int type)
{
	if (token == NULL) return false;
	switch(type) {
		case NUMBER:
			return is_numeric(token);
			break;
		case FUNC:
			return isalpha(token[0]);
			break;
		case SEPARATOR:
			return !strcmp(token, ",");
			break;
		case L_PARENTH:
			return !strcmp(token, "(");
			break;
		case R_PARENTH:
			return !strcmp(token, ")");
			break;
		case OPER:
			return ispunct(token[0])
				&& !is_token_type(token, NUMBER)
				&& !is_token_type(token, SEPARATOR)
				&& !is_token_type(token, L_PARENTH)
				&& !is_token_type(token, R_PARENTH);
			break;
		default:
			return false;
	}
	return false;
}

static int association(oper_info *op_info, size_t size, char s[])
{
	for (size_t i = 0; i < size; ++i)
		if (strcmp(op_info[i].str, s) == 0)
			return op_info[i].association;
	return -1;
}

static int precedence(oper_info *op_info, size_t size, char s[])
{
	for (size_t i = 0; i < size; ++i)
		if (strcmp(op_info[i].str, s) == 0)
			return op_info[i].precedence;
	return -1;
}

int handle_operator(char token[], stack *operators, char *queue[],
	size_t *q_size, oper_info *oper_info, size_t oper_info_size)
{
	int op_cmp_ret = 0;
	while (is_token_type(opers_top(operators), OPER)
		&& (op_cmp_ret = operator_compare(oper_info, oper_info_size,
			token, opers_top(operators)))) {
		char *tmp = pop_oper(operators);
		enqueue(queue, q_size, tmp);
		free(tmp);
	}
	if (op_cmp_ret == -1) {
		free(token);
		return -1;
	}
	push_oper(operators, token);
	free(token);
	return 0;
}

int handle_r_parenth(char token[], stack *operators, char *queue[],
	size_t *q_size)
{
	free(token);
	if (pop_to_q_until_l_parenth(operators, queue, q_size))
		return -1;
	char *tmp = pop_oper(operators);
	free(tmp);
	if (is_token_type(opers_top(operators), FUNC)) {
		char *tmp = pop_oper(operators);
		enqueue(queue, q_size, tmp);
		free(tmp);
	}
	return 0;
}

int pop_to_q_until_l_parenth(stack *operators, char *queue[],
	size_t *q_size)
{
	while (is_token_type(opers_top(operators), OPER)) {
		char *tmp = pop_oper(operators);
		enqueue(queue, q_size, tmp);
		free(tmp);
	}
	if (!is_token_type(opers_top(operators), L_PARENTH))
		return -1;
	return 0;
}

int operator_compare(oper_info *oper_info, size_t oper_info_size,
	char o1[], char o2[])
{
	if (!o1 || !o2) return 0;
	int o1_asso = association(oper_info, oper_info_size, o1);
	int o1_prec = precedence(oper_info, oper_info_size, o1);
	int o2_prec = precedence(oper_info, oper_info_size, o2);
	if (o1_asso == -1 || o1_prec == -1 || o2_prec == -1)
		return -1;
	return (o1_asso == LEFT && o1_prec <= o2_prec) ||
		(o1_asso == RIGHT && o1_prec < o2_prec);
}

char *opers_top(stack *operators)
{
	if (operators->size == 0)
		return NULL;
	return (char *)operators->head->data;
}

static int is_numeric(char s[])
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

