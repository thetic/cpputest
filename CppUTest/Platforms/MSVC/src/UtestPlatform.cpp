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

///////////// Time in millis

static long VisualCppTimeInMillis()
{
    static LARGE_INTEGER s_frequency;
    static const BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (long)((now.QuadPart * 1000) / s_frequency.QuadPart);
    } else {
#ifdef TIMERR_NOERROR
        return (long)timeGetTime();
#else
#if !defined(_WIN32_WINNT) || !defined(_WIN32_WINNT_VISTA) || (_WIN32_WINNT < _WIN32_WINNT_VISTA)
        return (long)GetTickCount();
#else
        return (long)GetTickCount64();
#endif
#endif
    }
}

long (*GetPlatformSpecificTimeInMillis)() = VisualCppTimeInMillis;

///////////// Time in String

static const char* VisualCppTimeString()
{
    time_t the_time = time(NULL);
    struct tm the_local_time;
    static char dateTime[80];
    memcpy(&the_local_time, localtime(&the_time), sizeof(the_local_time));
    strftime(dateTime, 80, "%Y-%m-%dT%H:%M:%S", &the_local_time);
    return dateTime;
}

const char* (*GetPlatformSpecificTimeString)() = VisualCppTimeString;
