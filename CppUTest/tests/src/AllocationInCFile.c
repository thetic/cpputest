#include "AllocationInCFile.h"

#include <stdlib.h>

/* This file is for simulating overloads of malloc */

char* mallocAllocation()
{
    return (char*) malloc(10UL);
}

char* strdupAllocation()
{
#ifdef CPPUTEST_USE_STRDUP_MACROS
    return strdup("0123456789");
#else
    return NULL;
#endif
}


char* strndupAllocation()
{
#ifdef CPPUTEST_USE_STRDUP_MACROS
    return strndup("0123456789", 10);
#else
    return NULL;
#endif
}

void freeAllocation(void* memory)
{
    free(memory);
}

void freeAllocationWithoutMacro(void* memory)
{
    free(memory);
}

