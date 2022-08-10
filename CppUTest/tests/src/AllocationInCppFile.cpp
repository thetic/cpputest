/* This file is for emulating allocations in a C++ file.
 * It is used simulating the use of the memory leak detector on production code in C++
 */
#include "AllocationInCppFile.hpp"

char* newAllocation()
{
    return new char;
}

char* newArrayAllocation()
{
    return new char[100];
}

char* newAllocationWithoutMacro()
{
    return new char;
}

char* newArrayAllocationWithoutMacro()
{
    return new char[100];
}

#if !CPPUTEST_NO_EXCEPTIONS

ClassThatThrowsAnExceptionInTheConstructor::ClassThatThrowsAnExceptionInTheConstructor()
{
  throw 1;
}

#endif
