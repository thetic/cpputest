#ifndef ALLOCATIONINCPPFILE_H
#define ALLOCATIONINCPPFILE_H

#include "CppUTest/CppUTestConfig.h"

char* newAllocation();
char* newArrayAllocation();

char* newAllocationWithoutMacro();
char* newArrayAllocationWithoutMacro();

#if CPPUTEST_HAVE_EXCEPTIONS

class ClassThatThrowsAnExceptionInTheConstructor
{
public:
    [[noreturn]] ClassThatThrowsAnExceptionInTheConstructor();
};

#endif

#endif
