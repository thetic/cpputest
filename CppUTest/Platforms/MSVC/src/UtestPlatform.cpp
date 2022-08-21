#include <windows.h>

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

static PlatformSpecificFile VisualCppFOpen(const char* filename, const char* flag)
{
    return fopen(filename, flag);
}

static void VisualCppFPuts(const char* str, PlatformSpecificFile file)
{
    fputs(str, (FILE*)file);
}

static void VisualCppFClose(PlatformSpecificFile file)
{
    fclose((FILE*)file);
}

PlatformSpecificFile (*PlatformSpecificFOpen)(const char* filename, const char* flag) = VisualCppFOpen;
void (*PlatformSpecificFPuts)(const char* str, PlatformSpecificFile file) = VisualCppFPuts;
void (*PlatformSpecificFClose)(PlatformSpecificFile file) = VisualCppFClose;

static void VisualCppFlush()
{
    fflush(stdout);
}

int (*PlatformSpecificPutchar)(int c) = putchar;
void (*PlatformSpecificFlush)(void) = VisualCppFlush;

static void* VisualCppMalloc(size_t size)
{
    return malloc(size);
}

static void* VisualCppReAlloc(void* memory, size_t size)
{
    return realloc(memory, size);
}

static void VisualCppFree(void* memory)
{
    free(memory);
}

void (*PlatformSpecificSrand)(unsigned int) = srand;
int (*PlatformSpecificRand)(void) = rand;
void* (*PlatformSpecificMalloc)(size_t size) = VisualCppMalloc;
void* (*PlatformSpecificRealloc)(void* memory, size_t size) = VisualCppReAlloc;
void (*PlatformSpecificFree)(void* memory) = VisualCppFree;
void* (*PlatformSpecificMemCpy)(void* s1, const void* s2, size_t size) = memcpy;
void* (*PlatformSpecificMemset)(void* mem, int c, size_t size) = memset;

static int IsInfImplementation(double d)
{
    return !_finite(d);
}

double (*PlatformSpecificFabs)(double d) = fabs;
int (*PlatformSpecificIsNan)(double) = _isnan;
int (*PlatformSpecificIsInf)(double) = IsInfImplementation;
int (*PlatformSpecificAtExit)(void (*func)(void)) = atexit;

static PlatformSpecificMutex VisualCppMutexCreate(void)
{
    CRITICAL_SECTION* critical_section = new CRITICAL_SECTION;
    InitializeCriticalSection(critical_section);
    return (PlatformSpecificMutex)critical_section;
}

static void VisualCppMutexLock(PlatformSpecificMutex mutex)
{
    EnterCriticalSection((CRITICAL_SECTION*)mutex);
}

static void VisualCppMutexUnlock(PlatformSpecificMutex mutex)
{
    LeaveCriticalSection((CRITICAL_SECTION*)mutex);
}

static void VisualCppMutexDestroy(PlatformSpecificMutex mutex)
{
    CRITICAL_SECTION* critical_section = (CRITICAL_SECTION*)mutex;
    DeleteCriticalSection(critical_section);
    delete critical_section;
}

PlatformSpecificMutex (*PlatformSpecificMutexCreate)(void) = VisualCppMutexCreate;
void (*PlatformSpecificMutexLock)(PlatformSpecificMutex) = VisualCppMutexLock;
void (*PlatformSpecificMutexUnlock)(PlatformSpecificMutex) = VisualCppMutexUnlock;
void (*PlatformSpecificMutexDestroy)(PlatformSpecificMutex) = VisualCppMutexDestroy;
