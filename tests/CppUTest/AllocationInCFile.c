#include "AllocationInCFile.h"
#include "CppUTest/StandardCLibrary.h"

/* This file is for simulating overloads of malloc */

char* mallocAllocation(void)
{
    return (char*) malloc(10UL);
}

char* strdupAllocation(void)
{
    return NULL;
}


char* strndupAllocation(void)
{
    return NULL;
}

void freeAllocation(void* memory)
{
    free(memory);
}

#undef free

void freeAllocationWithoutMacro(void* memory)
{
    free(memory);
}

