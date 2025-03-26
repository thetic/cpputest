#include "AllocationInCFile.h"
#include "CppUTest/StandardCLibrary.h"

/* This file is for simulating overloads of malloc */

char* mallocAllocation(void)
{
    return (char*) malloc(10UL);
}

char* strdupAllocation(void)
{
#ifdef CPPUTEST_USE_STRDUP_MACROS
    return strdup("0123456789");
#else
    return NULL;
#endif
}


char* strndupAllocation(void)
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

