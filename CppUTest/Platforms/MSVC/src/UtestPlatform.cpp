#include <windows.h>

#include "CppUTest/PlatformSpecificFunctions.h"
#include "CppUTest/PlatformSpecificFunctions.hpp"

#include "CppUTest/TestFailure.hpp"
#include "CppUTest/TestOutput.hpp"
#include "CppUTest/TestResult.hpp"

#include <mmsystem.h>

#include <cfloat>
#include <cmath>
#include <csetjmp>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

static void VisualCppRunTestInASeperateProcess(UtestShell* shell, TestPlugin* plugin, TestResult* result)
{
    result->addFailure(TestFailure(shell, "-p doesn't work on this platform, as it is lacking fork.\b"));
}

void (*PlatformSpecificRunTestInASeperateProcess)(UtestShell* shell, TestPlugin* plugin, TestResult* result) = VisualCppRunTestInASeperateProcess;

WorkingEnvironment PlatformSpecificGetWorkingEnvironment()
{
    return WorkingEnvironment::visualStudio;
}
