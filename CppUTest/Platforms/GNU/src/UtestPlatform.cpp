/*
 * Copyright (c) 2007, Michael Feathers, James Grenning and Bas Vodde
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE EARLIER MENTIONED AUTHORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CppUTest/PlatformSpecificFunctions.hpp"
#include "CppUTest/SimpleString.hpp"
#include "CppUTest/TestFailure.hpp"
#include "CppUTest/TestOutput.hpp"
#include "CppUTest/TestResult.hpp"
#include "CppUTest/Utest.hpp"

#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef CPPUTEST_HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif

#if defined(CPPUTEST_HAVE_FORK) && defined(CPPUTEST_HAVE_WAITPID)
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#ifdef CPPUTEST_HAVE_PTHREAD_MUTEX_LOCK
#include <pthread.h>
#endif

// There is a possibility that a compiler provides fork but not waitpid.
#if !defined(CPPUTEST_HAVE_FORK) || !defined(CPPUTEST_HAVE_WAITPID)

static void GccPlatformSpecificRunTestInASeperateProcess(UtestShell* shell, TestPlugin*, TestResult* result)
{
    result->addFailure(TestFailure(shell, "-p doesn't work on this platform, as it is lacking fork.\b"));
}

static int PlatformSpecificForkImplementation(void)
{
    return 0;
}

static int PlatformSpecificWaitPidImplementation(int, int*, int)
{
    return 0;
}

#else

static void SetTestFailureByStatusCode(UtestShell* shell, TestResult* result, int status)
{
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        result->addFailure(TestFailure(shell, "Failed in separate process"));
    } else if (WIFSIGNALED(status)) {
        std::string message("Failed in separate process - killed by signal ");
        message += StringFrom(WTERMSIG(status));
        result->addFailure(TestFailure(shell, message));
    } else if (WIFSTOPPED(status)) {
        result->addFailure(TestFailure(shell, "Stopped in separate process - continuing"));
    }
}

static void GccPlatformSpecificRunTestInASeperateProcess(UtestShell* shell, TestPlugin* plugin, TestResult* result)
{
    const pid_t syscallError = -1;
    pid_t cpid;
    pid_t w;
    int status = 0;

    cpid = PlatformSpecificFork();

    if (cpid == syscallError) {
        result->addFailure(TestFailure(shell, "Call to fork() failed"));
        return;
    }

    if (cpid == 0) { /* Code executed by child */
        const size_t initialFailureCount = result->getFailureCount(); // LCOV_EXCL_LINE
        shell->runOneTestInCurrentProcess(plugin, *result); // LCOV_EXCL_LINE
        _exit(initialFailureCount < result->getFailureCount()); // LCOV_EXCL_LINE
    } else { /* Code executed by parent */
        size_t amountOfRetries = 0;
        do {
            w = PlatformSpecificWaitPid(cpid, &status, WUNTRACED);
            if (w == syscallError) {
                // OS X debugger causes EINTR
                if (EINTR == errno) {
                    if (amountOfRetries > 30) {
                        result->addFailure(TestFailure(shell, "Call to waitpid() failed with EINTR. Tried 30 times and giving up! Sometimes happens in debugger"));
                        return;
                    }
                    amountOfRetries++;
                } else {
                    result->addFailure(TestFailure(shell, "Call to waitpid() failed"));
                    return;
                }
            } else {
                SetTestFailureByStatusCode(shell, result, status);
                if (WIFSTOPPED(status))
                    kill(w, SIGCONT);
            }
        } while ((w == syscallError) || (!WIFEXITED(status) && !WIFSIGNALED(status)));
    }
}

static pid_t PlatformSpecificForkImplementation(void)
{
    return fork();
}

static pid_t PlatformSpecificWaitPidImplementation(int pid, int* status, int options)
{
    return waitpid(pid, status, options);
}

#endif

WorkingEnvironment PlatformSpecificGetWorkingEnvironment()
{
    return WorkingEnvironment::eclipse;
}

void (*PlatformSpecificRunTestInASeperateProcess)(UtestShell* shell, TestPlugin* plugin, TestResult* result) = GccPlatformSpecificRunTestInASeperateProcess;
int (*PlatformSpecificFork)(void) = PlatformSpecificForkImplementation;
int (*PlatformSpecificWaitPid)(int, int*, int) = PlatformSpecificWaitPidImplementation;

///////////// Time in millis

static long TimeInMillisImplementation()
{
#ifdef CPPUTEST_HAVE_GETTIMEOFDAY
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return (tv.tv_sec * 1000) + (long)((double)tv.tv_usec * 0.001);
#else
    return 0;
#endif
}

static const char* TimeStringImplementation()
{
    time_t theTime = time(nullptr);
    static char dateTime[80];
#if defined(_WIN32) && defined(MINGW_HAS_SECURE_API)
    static struct tm lastlocaltime;
    localtime_s(&lastlocaltime, &theTime);
    struct tm* tmp = &lastlocaltime;
#else
    struct tm* tmp = localtime(&theTime);
#endif
    strftime(dateTime, 80, "%Y-%m-%dT%H:%M:%S", tmp);
    return dateTime;
}

long (*GetPlatformSpecificTimeInMillis)() = TimeInMillisImplementation;
const char* (*GetPlatformSpecificTimeString)() = TimeStringImplementation;

/* Wish we could add an attribute to the format for discovering mis-use... but the __attribute__(format) seems to not work on va_list */
#ifdef __clang__
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wused-but-marked-unused"
#endif

static PlatformSpecificFile PlatformSpecificFOpenImplementation(const char* filename, const char* flag)
{
#if defined(_WIN32) && defined(MINGW_HAS_SECURE_API)
    FILE* file;
    fopen_s(&file, filename, flag);
    return file;
#else
    return fopen(filename, flag);
#endif
}

static void PlatformSpecificFPutsImplementation(const char* str, PlatformSpecificFile file)
{
    fputs(str, (FILE*)file);
}

static void PlatformSpecificFCloseImplementation(PlatformSpecificFile file)
{
    fclose((FILE*)file);
}

static void PlatformSpecificFlushImplementation()
{
    fflush(stdout);
}

PlatformSpecificFile (*PlatformSpecificFOpen)(const char*, const char*) = PlatformSpecificFOpenImplementation;
void (*PlatformSpecificFPuts)(const char*, PlatformSpecificFile) = PlatformSpecificFPutsImplementation;
void (*PlatformSpecificFClose)(PlatformSpecificFile) = PlatformSpecificFCloseImplementation;

int (*PlatformSpecificPutchar)(int) = putchar;
void (*PlatformSpecificFlush)() = PlatformSpecificFlushImplementation;

void* (*PlatformSpecificMalloc)(size_t size) = malloc;
void* (*PlatformSpecificRealloc)(void*, size_t) = realloc;
void (*PlatformSpecificFree)(void* memory) = free;
void* (*PlatformSpecificMemCpy)(void*, const void*, size_t) = memcpy;
void* (*PlatformSpecificMemset)(void*, int, size_t) = memset;

void (*PlatformSpecificSrand)(unsigned int) = srand;
int (*PlatformSpecificRand)(void) = rand;
int (*PlatformSpecificAtExit)(void (*func)(void)) = atexit; /// this was undefined before

static PlatformSpecificMutex PThreadMutexCreate(void)
{
#ifdef CPPUTEST_HAVE_PTHREAD_MUTEX_LOCK
    pthread_mutex_t* mutex = new pthread_mutex_t;

    pthread_mutex_init(mutex, nullptr);
    return (PlatformSpecificMutex)mutex;
#else
    return nullptr;
#endif
}

static void PThreadMutexLock(PlatformSpecificMutex mtx)
{
#ifdef CPPUTEST_HAVE_PTHREAD_MUTEX_LOCK
    pthread_mutex_lock((pthread_mutex_t*)mtx);
#else
    (void)mtx;
#endif
}

static void PThreadMutexUnlock(PlatformSpecificMutex mtx)
{
#ifdef CPPUTEST_HAVE_PTHREAD_MUTEX_LOCK
    pthread_mutex_unlock((pthread_mutex_t*)mtx);
#else
    (void)mtx;
#endif
}

static void PThreadMutexDestroy(PlatformSpecificMutex mtx)
{
#ifdef CPPUTEST_HAVE_PTHREAD_MUTEX_LOCK
    pthread_mutex_t* mutex = (pthread_mutex_t*)mtx;
    pthread_mutex_destroy(mutex);
    delete mutex;
#else
    (void)mtx;
#endif
}

PlatformSpecificMutex (*PlatformSpecificMutexCreate)(void) = PThreadMutexCreate;
void (*PlatformSpecificMutexLock)(PlatformSpecificMutex) = PThreadMutexLock;
void (*PlatformSpecificMutexUnlock)(PlatformSpecificMutex) = PThreadMutexUnlock;
void (*PlatformSpecificMutexDestroy)(PlatformSpecificMutex) = PThreadMutexDestroy;
