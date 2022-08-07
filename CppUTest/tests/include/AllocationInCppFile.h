#ifndef ALLOCATIONINCPPFILE_H
#define ALLOCATIONINCPPFILE_H

#include "CppUTest/CppUTestConfig.h"

char* newAllocation();
char* newArrayAllocation();

char* newAllocationWithoutMacro();
char* newArrayAllocationWithoutMacro();

#if !CPPUTEST_NO_EXCEPTIONS

class ClassThatThrowsAnExceptionInTheConstructor
{
public:
    CPPUTEST_NORETURN ClassThatThrowsAnExceptionInTheConstructor();
};

#endif

#endif
