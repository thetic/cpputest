#ifndef ALLOCATIONINCPPFILE_H
#define ALLOCATIONINCPPFILE_H

char* newAllocation();
char* newArrayAllocation();

char* newAllocationWithoutMacro();
char* newArrayAllocationWithoutMacro();

#if !CPPUTEST_NO_EXCEPTIONS

class ClassThatThrowsAnExceptionInTheConstructor
{
public:
    [[noreturn]] ClassThatThrowsAnExceptionInTheConstructor();
};

#endif

#endif
