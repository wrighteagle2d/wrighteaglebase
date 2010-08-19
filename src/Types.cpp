#include <cstdio>
#include <cmath>
#include "Types.h"

int abort_with_SIGSEGV(const char *expr, const char *file, int line, const char *function)
{
	fprintf(stderr, "%s in %s:%d aborted at %s\n", function, file, line, expr);
	int *p = 0;
	*p = 1; //segfault here
	return *p;
}

