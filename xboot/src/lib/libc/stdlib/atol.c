/*
 * libc/stdlib/atol.c
 */

#include <xboot/module.h>
#include <stddef.h>
#include <stdlib.h>

long atol(const char * nptr)
{
	return (long)strtol(nptr, NULL, 10);
}
EXPORT_SYMBOL(atol);
