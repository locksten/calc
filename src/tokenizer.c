#include "../include/tokenizer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_IN_STR_LEN 128
#define STARTING_BUFFER_SIZE 256
#define BUFFER_INC_RATIO 2

static char *dynamic_cat(char s[], size_t *size, char in[]);
static int is_numeric(char s[]);
static int read_alpha(char *s[], char *rezult[], size_t *rezult_size,
	char delim[]);
static int read_punct(char *s[], char *rezult[], size_t *rezult_size,
	char delim[]);
static int read_numeric(char *s[], char *rezult[], size_t *rezult_size,
	char delim[]);

static char *dynamic_cat(char *s, size_t *size, char *in)
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

static int read_alpha(char *s[], char *rezult[], size_t *rezult_size,
	char delim[])
{
	int num_read;
	char tmp[MAX_IN_STR_LEN];
	sscanf(*s, "%[a-zA-Z] %n", tmp, &num_read);
	*s += num_read;

	char *test;
	if (!(test = dynamic_cat(*rezult, rezult_size, tmp)))
		return -1;
	else
		*rezult = test;
	if (!(test = dynamic_cat(*rezult, rezult_size, delim)))
		return -1;
	else
		*rezult = test;
	return 0;
}

static int read_punct(char *s[], char *rezult[], size_t *rezult_size,
	char delim[])
{
	char tmp[2];
	sscanf(*s, "%1s", tmp);
	++*s;

	char *test;
	if (!(test = dynamic_cat(*rezult, rezult_size, tmp)))
		return -1;
	else
		*rezult = test;
	if (!(test = dynamic_cat(*rezult, rezult_size, delim)))
		return -1;
	else
		*rezult = test;
	return 0;
}

static int read_numeric(char *s[], char *rezult[], size_t *rezult_size,
	char delim[])
{
	int read_dot = 0;
	int num_read;
	char tmp[MAX_IN_STR_LEN * 2];
	if (**s == '-' && *(*s + 1) == '.') {
		read_dot = 1;
		tmp[0] = '-';
		tmp[1] = '.';
		*s += 2;
		sscanf(*s, "%[0-9] %n", &tmp[2], &num_read);
	} else if (**s == '-') {
		tmp[0] = '-';
		++*s;
		sscanf(*s, "%[0-9] %n", &tmp[1], &num_read);
	} else if (**s == '.') {
		read_dot = 1;
		tmp[0] = '.';
		++*s;
		sscanf(*s, "%[0-9] %n", &tmp[1], &num_read);
	} else {
		sscanf(*s, "%[0-9] %n", tmp, &num_read);
	}
	*s += num_read;

	if (!read_dot && **s == '.' && isdigit(*(*s + 1))) {
		*s += 1;
		char postdot[MAX_IN_STR_LEN];
		sscanf(*s, "%[0-9] %n", postdot, &num_read);
		*s += num_read;
		strcat(tmp, ".");
		strcat(tmp, postdot);
	}


	char *test;
	if (!(test = dynamic_cat(*rezult, rezult_size, tmp)))
		return -1;
	else
		*rezult = test;
	if (!(test = dynamic_cat(*rezult, rezult_size, delim)))
		return -1;
	else
		*rezult = test;
	return 0;
}

char *tokenize(char s[], char delim[])
{
	size_t rezult_size = STARTING_BUFFER_SIZE;
	char *rezult = (char *)malloc(sizeof(rezult) * rezult_size);
	rezult[0] = '\0';

	int err = 0;
	while (*s != '\0') {
		if (isalpha(*s))
			err = read_alpha(&s, &rezult, &rezult_size, delim);
		else if (is_numeric(s))
			err = read_numeric(&s, &rezult, &rezult_size, delim);
		else if (ispunct(*s))
			err = read_punct(&s, &rezult, &rezult_size, delim);
		else
			++s;
		if (err) {
			free(rezult);
			return NULL;
		}
	}
	return rezult;
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

