#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "install-log.h"

/* sprintf wrapper that can allocate memory for itself */
char* safe_sprintf(char** to, int* to_cap, char* fmt, ...)
{
	char* string_buf = NULL;
	int string_cap = 0;
	va_list ap;
	int len;
	
	/* Allocate a string if necessary */
	if (to == NULL) to = &string_buf;
	if (to_cap == NULL) to_cap = &string_cap;
	if (*to == NULL) {
		*to_cap = strlen(fmt) * 2;
		*to = malloc(strlen(fmt) * 2);
	}

	while (true) {
		/* Generate the formatted string */
		va_start(ap, fmt);
		len = vsnprintf(*to, *to_cap, fmt, ap);
		va_end(ap);

		/* Resize and repeat if necessary */
		if (len > -1 && len < *to_cap) return *to;
		if (len > -1)
			*to_cap = len + 1;
		else
			*to_cap *= 2;
		free(*to);
		*to = malloc(*to_cap);
	}
}

/* Replace from with to in *context; *context_cap is the capacity */
void replace(char** context, int* context_cap, char* from, char* to)
{
	int from_len = strlen(from);
	int to_len = strlen(to);
	char* context_p;

	/* Easiest case (whew!) */
	if (from_len == to_len) {
		while ((context_p = strstr(context_p, from)) != NULL) {
			memcpy(context_p, to, to_len);
			context_p += to_len;
		}
		return;
	}

	/* Hardest case (aaugh): allocate enough memory */
	if (to_len > from_len) {
		int diff = to_len - from_len;
		int count = 0;
		int len = 1;

		/* Calculate length of final string */
		context_p = *context;
		while (strstr(context_p, from) != NULL) {
			len += diff;
			context_p += from_len;
		}

		/* Ensure there is enough room */
		if (len > *context_cap) {
			*context_cap = len;
			*context = realloc(*context, *context_cap);
		}
	}
	
	/* Replace from with to */
	context_p = *context;
	while ((context_p = strstr(context_p, from)) != NULL) {
		memmove(context_p + to_len, context_p + from_len,
				strlen(context_p + from_len) + 1);
		memcpy(context_p, to, to_len);
		context_p += to_len;
	}
}

/* Remove repetitions of c in string */
void collapse(char* string, char c)
{
	char rep[3];
	char* string_p = string;

	/* Build search string */
	rep[0] = rep[1] = c;
	rep[2] = 0;

	/* Replace search string with c */
	while ((string_p = strstr(string_p, rep)) != NULL)
		memmove(string_p, string_p + 1,	strlen(string_p + 1) + 1);
}

/* Output a printf-style message */
void alert(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	fprintf(stderr, "%s: ", program_name);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

/* Output a printf-style message and exit */
void fail(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	fprintf(stderr, "%s: ", program_name);
	vfprintf(stderr, format, ap);
	va_end(ap);
	
	exit(EXIT_FAILURE);
}

/* Depending on verbosity level, output a printf-style message */
void report(int level, const char* format, ...)
{
	va_list ap;
	
	if (level > verbosity) return;

	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}
