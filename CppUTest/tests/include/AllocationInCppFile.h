#ifndef ALLOCATIONINCPPFILE_H
#define ALLOCATIONINCPPFILE_H

#include "CppUTest/CppUTestConfig.h"

char* newAllocation();
char* newArrayAllocation();

char* newAllocationWithoutMacro();
char* newArrayAllocationWithoutMacro();

#if CPPUTEST_USE_STD_CPP_LIB

class ClassThatThrowsAnExceptionInTheConstructor
{
public:
    CPPUTEST_NORETURN ClassThatThrowsAnExceptionInTheConstructor();
};

#endif

#endif
