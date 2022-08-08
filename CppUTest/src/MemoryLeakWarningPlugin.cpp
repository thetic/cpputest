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

#include "CppUTest/TestHarness.hpp"
#include "CppUTest/MemoryLeakWarningPlugin.hpp"
#include "CppUTest/MemoryLeakDetector.hpp"
#include "CppUTest/TestMemoryAllocator.hpp"
#include "CppUTest/PlatformSpecificFunctions.hpp"
#include "CppUTest/SimpleMutex.hpp"

/********** Enabling and disabling for C also *********/

static void* normal_malloc(size_t size, const char*, size_t)
{
    return PlatformSpecificMalloc(size);
}

static void* normal_realloc(void* memory, size_t size, const char*, size_t)
{
    return PlatformSpecificRealloc(memory, size);
}

static void normal_free(void* buffer, const char*, size_t)
{
    PlatformSpecificFree(buffer);
}

static void *(*malloc_fptr)(size_t size, const char* file, size_t line) = normal_malloc;
static void (*free_fptr)(void* mem, const char* file, size_t line) = normal_free;
static void*(*realloc_fptr)(void* memory, size_t size, const char* file, size_t line) = normal_realloc;

void* cpputest_malloc_location_with_leak_detection(size_t size, const char* file, size_t line)
{
    return malloc_fptr(size, file, line);
}

void* cpputest_realloc_location_with_leak_detection(void* memory, size_t size, const char* file, size_t line)
{
    return realloc_fptr(memory, size, file, line);
}

void cpputest_free_location_with_leak_detection(void* buffer, const char* file, size_t line)
{
    free_fptr(buffer, file, line);
}

/********** C++ *************/

void MemoryLeakWarningPlugin::turnOffNewDeleteOverloads()
{
}

void MemoryLeakWarningPlugin::turnOnDefaultNotThreadSafeNewDeleteOverloads()
{
}

void MemoryLeakWarningPlugin::turnOnThreadSafeNewDeleteOverloads()
{
}

bool MemoryLeakWarningPlugin::areNewDeleteOverloaded()
{
    return false;
}

void MemoryLeakWarningPlugin::saveAndDisableNewDeleteOverloads()
{
}

void MemoryLeakWarningPlugin::restoreNewDeleteOverloads()
{
}

void crash_on_allocation_number(unsigned alloc_number)
{
    static CrashOnAllocationAllocator crashAllocator;
    crashAllocator.setNumberToCrashOn(alloc_number);
    setCurrentMallocAllocator(&crashAllocator);
    setCurrentNewAllocator(&crashAllocator);
    setCurrentNewArrayAllocator(&crashAllocator);
}

class MemoryLeakWarningReporter: public MemoryLeakFailure
{
public:
    ~MemoryLeakWarningReporter() override
    {
    }

    void fail(char* fail_string) override
    {
        UtestShell* currentTest = UtestShell::getCurrent();
        currentTest->failWith(FailFailure(currentTest, currentTest->getName().asCharString(), currentTest->getLineNumber(), fail_string), TestTerminatorWithoutExceptions());
    } // LCOV_EXCL_LINE
};

static MemoryLeakFailure* globalReporter = nullptr;
static MemoryLeakDetector* globalDetector = nullptr;

MemoryLeakDetector* MemoryLeakWarningPlugin::getGlobalDetector()
{
    if (globalDetector == nullptr) {
        saveAndDisableNewDeleteOverloads();

        globalReporter = new MemoryLeakWarningReporter;
        globalDetector = new MemoryLeakDetector(globalReporter);

        restoreNewDeleteOverloads();
    }
    return globalDetector;
}

MemoryLeakFailure* MemoryLeakWarningPlugin::getGlobalFailureReporter()
{
    return globalReporter;
}

void MemoryLeakWarningPlugin::destroyGlobalDetectorAndTurnOffMemoryLeakDetectionInDestructor(bool des)
{
    destroyGlobalDetectorAndTurnOfMemoryLeakDetectionInDestructor_ = des;
}

void MemoryLeakWarningPlugin::setGlobalDetector(MemoryLeakDetector* detector, MemoryLeakFailure* reporter)
{
    globalDetector = detector;
    globalReporter = reporter;
}

void MemoryLeakWarningPlugin::destroyGlobalDetector()
{
    turnOffNewDeleteOverloads();
    delete globalDetector;
    delete globalReporter;
    globalDetector = nullptr;
}


MemoryLeakWarningPlugin* MemoryLeakWarningPlugin::firstPlugin_ = nullptr;

MemoryLeakWarningPlugin* MemoryLeakWarningPlugin::getFirstPlugin()
{
    return firstPlugin_;
}

MemoryLeakDetector* MemoryLeakWarningPlugin::getMemoryLeakDetector()
{
    return memLeakDetector_;
}

void MemoryLeakWarningPlugin::ignoreAllLeaksInTest()
{
    ignoreAllWarnings_ = true;
}

void MemoryLeakWarningPlugin::expectLeaksInTest(size_t n)
{
    expectedLeaks_ = n;
}

MemoryLeakWarningPlugin::MemoryLeakWarningPlugin(const SimpleString& name, MemoryLeakDetector* localDetector) :
    TestPlugin(name), ignoreAllWarnings_(false), destroyGlobalDetectorAndTurnOfMemoryLeakDetectionInDestructor_(false), expectedLeaks_(0)
{
    if (firstPlugin_ == nullptr) firstPlugin_ = this;

    if (localDetector) memLeakDetector_ = localDetector;
    else memLeakDetector_ = getGlobalDetector();

    memLeakDetector_->enable();
}

MemoryLeakWarningPlugin::~MemoryLeakWarningPlugin()
{
    if (destroyGlobalDetectorAndTurnOfMemoryLeakDetectionInDestructor_) {
        MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
        MemoryLeakWarningPlugin::destroyGlobalDetector();
    }
}

void MemoryLeakWarningPlugin::preTestAction(UtestShell& /*test*/, TestResult& result)
{
    memLeakDetector_->startChecking();
    failureCount_ = result.getFailureCount();
}

void MemoryLeakWarningPlugin::postTestAction(UtestShell& test, TestResult& result)
{
    memLeakDetector_->stopChecking();
    size_t leaks = memLeakDetector_->totalMemoryLeaks(mem_leak_period_checking);

    if (!ignoreAllWarnings_ && expectedLeaks_ != leaks && failureCount_ == result.getFailureCount()) {
        if(MemoryLeakWarningPlugin::areNewDeleteOverloaded()) {
            TestFailure f(&test, memLeakDetector_->report(mem_leak_period_checking));
            result.addFailure(f);
        } else if(expectedLeaks_ > 0) {
            result.print(StringFromFormat("Warning: Expected %d leak(s), but leak detection was disabled", (int) expectedLeaks_).asCharString());
        }
    }
    memLeakDetector_->markCheckingPeriodLeaksAsNonCheckingPeriod();
    ignoreAllWarnings_ = false;
    expectedLeaks_ = 0;
}

const char* MemoryLeakWarningPlugin::FinalReport(size_t toBeDeletedLeaks)
{
    size_t leaks = memLeakDetector_->totalMemoryLeaks(mem_leak_period_enabled);
    if (leaks != toBeDeletedLeaks) return memLeakDetector_->report(mem_leak_period_enabled);
    return "";
}


