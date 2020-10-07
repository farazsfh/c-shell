#include <stdio.h>
#include <string.h>
#include "parse.h"

#define MAXARGC 100

char **parse(char *s)
{
    static char *argv[MAXARGC + 1];
    char **p = argv;
    char *this;
    for (this = strtok(s, " \t\r\n");
	    this && p - argv < MAXARGC;
	    this = strtok(NULL, " \t\r\n"))
	*p++ = this;
    *p = NULL;
    return((p > argv) ? argv : NULL);
}
