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
#ifdef _MSC_VER
#include <windows.h>

#include <mmsystem.h>
#endif

#include "CppUTest/TestOutput.hpp"

#include "CppUTest/StringFrom.hpp"
#include "CppUTest/TestFailure.hpp"
#include "CppUTest/TestResult.hpp"
#include "CppUTest/Utest.hpp"

#ifdef CPPUTEST_HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif

namespace cpputest {

namespace {
    long time_in_millis_impl()
    {
#ifdef _MSC_VER
        static LARGE_INTEGER s_frequency;
        static const BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
        if (s_use_qpc) {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            return (long)((now.QuadPart * 1000) / s_frequency.QuadPart);
        } else {
#ifdef TIMERR_NOERROR
            return (long)timeGetTime();
#elif !defined(_WIN32_WINNT) || !defined(_WIN32_WINNT_VISTA) || (_WIN32_WINNT < _WIN32_WINNT_VISTA)
            return (long)GetTickCount();
#else
            return (long)GetTickCount64();
#endif
        }
#elif defined(CPPUTEST_HAVE_GETTIMEOFDAY)
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);
        return (tv.tv_sec * 1000) + (long)((double)tv.tv_usec * 0.001);
#else
        return 0;
#endif
    }
}

WorkingEnvironment TestOutput::workingEnvironment_ = WorkingEnvironment::detectEnvironment;

std::FILE* (*TestOutput::fopen)(const char* filename, const char* mode) = std::fopen;
int (*TestOutput::fputs)(const char* str, std::FILE* stream) = std::fputs;
int (*TestOutput::fclose)(std::FILE* stream) = std::fclose;
int (*TestOutput::putchar)(int c) = std::putchar;
int (*TestOutput::fflush)(std::FILE* stream) = std::fflush;
long (*TestOutput::time_in_millis)(void) = time_in_millis_impl;

void TestOutput::setWorkingEnvironment(WorkingEnvironment workEnvironment)
{
    workingEnvironment_ = workEnvironment;
}

WorkingEnvironment TestOutput::getWorkingEnvironment()
{
    if (workingEnvironment_ == WorkingEnvironment::detectEnvironment) {
#ifdef _MSC_VER
        return WorkingEnvironment::visualStudio;
#else
        return WorkingEnvironment::eclipse;
#endif
    }
    return workingEnvironment_;
}

TestOutput::TestOutput()
    : dotCount_(0)
    , verbose_(level_quiet)
    , color_(false)
    , progressIndication_(".")
{
}

TestOutput::~TestOutput()
{
}

void TestOutput::verbose(VerbosityLevel level)
{
    verbose_ = level;
}

void TestOutput::color()
{
    color_ = true;
}

void TestOutput::print(const char* str)
{
    printBuffer(str);
}

void TestOutput::print(long n)
{
    print(StringFrom(n).c_str());
}

void TestOutput::print(size_t n)
{
    print(StringFrom(n).c_str());
}

void TestOutput::printDouble(double d)
{
    print(StringFrom(d).c_str());
}

TestOutput& operator<<(TestOutput& p, const char* s)
{
    p.print(s);
    return p;
}

TestOutput& operator<<(TestOutput& p, long int i)
{
    p.print(i);
    return p;
}

void TestOutput::printCurrentTestStarted(const UtestShell& test)
{
    if (verbose_ > level_quiet)
        print(test.getFormattedName().c_str());

    if (test.willRun()) {
        setProgressIndicator(".");
    } else {
        setProgressIndicator("!");
    }
}

void TestOutput::printCurrentTestEnded(const TestResult& res)
{
    if (verbose_ > level_quiet) {
        print(" - ");
        print(res.getCurrentTestTotalExecutionTime());
        print(" ms\n");
    } else {
        printProgressIndicator();
    }
}

void TestOutput::printProgressIndicator()
{
    print(progressIndication_);
    if (++dotCount_ % 50 == 0)
        print("\n");
}

void TestOutput::setProgressIndicator(const char* indicator)
{
    progressIndication_ = indicator;
}

void TestOutput::printTestsStarted()
{
}

void TestOutput::printCurrentGroupStarted(const UtestShell& /*test*/)
{
}

void TestOutput::printCurrentGroupEnded(const TestResult& /*res*/)
{
}

void TestOutput::printTestsEnded(const TestResult& result)
{
    print("\n");
    const bool isFailure = result.isFailure();
    const size_t failureCount = result.getFailureCount();
    if (isFailure) {
        if (color_) {
            print("\033[31;1m");
        }
        print("Errors (");
        if (failureCount > 0) {
            print(failureCount);
            print(" failures, ");
        } else {
            print("ran nothing, ");
        }
    } else {
        if (color_) {
            print("\033[32;1m");
        }
        print("OK (");
    }
    print(result.getTestCount());
    print(" tests, ");
    print(result.getRunCount());
    print(" ran, ");
    print(result.getCheckCount());
    print(" checks, ");
    print(result.getIgnoredCount());
    print(" ignored, ");
    print(result.getFilteredOutCount());
    print(" filtered out, ");
    print(result.getTotalExecutionTime());
    print(" ms)");
    if (color_) {
        print("\033[m");
    }
    if (isFailure && failureCount == 0) {
        print("\nNote: test run failed because no tests were run or ignored. Assuming something went wrong. "
              "This often happens because of linking errors or typos in test filter.");
    }
    print("\n\n");

    dotCount_ = 0;
}

void TestOutput::printTestRun(size_t number, size_t total)
{
    if (total > 1) {
        print("Test run ");
        print(number);
        print(" of ");
        print(total);
        print("\n");
    }
}

void TestOutput::printFailure(const TestFailure& failure)
{
    if (failure.isOutsideTestFile() || failure.isInHelperFunction())
        printFileAndLineForTestAndFailure(failure);
    else
        printFileAndLineForFailure(failure);

    printFailureMessage(failure.getMessage());
}

void TestOutput::printFileAndLineForTestAndFailure(const TestFailure& failure)
{
    printErrorInFileOnLineFormattedForWorkingEnvironment(failure.getTestFileName(), failure.getTestLineNumber());
    printFailureInTest(failure.getTestName());
    printErrorInFileOnLineFormattedForWorkingEnvironment(failure.getFileName(), failure.getFailureLineNumber());
}

void TestOutput::printFileAndLineForFailure(const TestFailure& failure)
{
    printErrorInFileOnLineFormattedForWorkingEnvironment(failure.getFileName(), failure.getFailureLineNumber());
    printFailureInTest(failure.getTestName());
}

void TestOutput::printFailureInTest(std::string testName)
{
    print(" Failure in ");
    print(testName.c_str());
}

void TestOutput::printFailureMessage(std::string reason)
{
    print("\n");
    print("\t");
    print(reason.c_str());
    print("\n\n");
}

void TestOutput::printErrorInFileOnLineFormattedForWorkingEnvironment(std::string file, size_t lineNumber)
{
    if (TestOutput::getWorkingEnvironment() == WorkingEnvironment::visualStudio)
        printVisualStudioErrorInFileOnLine(file, lineNumber);
    else
        printEclipseErrorInFileOnLine(file, lineNumber);
}

void TestOutput::printEclipseErrorInFileOnLine(std::string file, size_t lineNumber)
{
    print("\n");
    print(file.c_str());
    print(":");
    print(lineNumber);
    print(":");
    print(" error:");
}

void TestOutput::printVisualStudioErrorInFileOnLine(std::string file, size_t lineNumber)
{
    print("\n");
    print(file.c_str());
    print("(");
    print(lineNumber);
    print("):");
    print(" error:");
}

void TestOutput::printVeryVerbose(const char* str)
{
    if (verbose_ == level_veryVerbose)
        printBuffer(str);
}

void ConsoleTestOutput::printBuffer(const char* s)
{
    while (*s) {
        putchar(*s);
        s++;
    }
    flush();
}

void ConsoleTestOutput::flush()
{
    fflush(stdout);
}

StringBufferTestOutput::~StringBufferTestOutput()
{
}

CompositeTestOutput::CompositeTestOutput()
    : outputOne_(nullptr)
    , outputTwo_(nullptr)
{
}

CompositeTestOutput::~CompositeTestOutput()
{
    delete outputOne_;
    delete outputTwo_;
}

void CompositeTestOutput::setOutputOne(TestOutput* output)
{
    delete outputOne_;
    outputOne_ = output;
}

void CompositeTestOutput::setOutputTwo(TestOutput* output)
{
    delete outputTwo_;
    outputTwo_ = output;
}

void CompositeTestOutput::printTestsStarted()
{
    if (outputOne_)
        outputOne_->printTestsStarted();
    if (outputTwo_)
        outputTwo_->printTestsStarted();
}

void CompositeTestOutput::printTestsEnded(const TestResult& result)
{
    if (outputOne_)
        outputOne_->printTestsEnded(result);
    if (outputTwo_)
        outputTwo_->printTestsEnded(result);
}

void CompositeTestOutput::printCurrentTestStarted(const UtestShell& test)
{
    if (outputOne_)
        outputOne_->printCurrentTestStarted(test);
    if (outputTwo_)
        outputTwo_->printCurrentTestStarted(test);
}

void CompositeTestOutput::printCurrentTestEnded(const TestResult& res)
{
    if (outputOne_)
        outputOne_->printCurrentTestEnded(res);
    if (outputTwo_)
        outputTwo_->printCurrentTestEnded(res);
}

void CompositeTestOutput::printCurrentGroupStarted(const UtestShell& test)
{
    if (outputOne_)
        outputOne_->printCurrentGroupStarted(test);
    if (outputTwo_)
        outputTwo_->printCurrentGroupStarted(test);
}

void CompositeTestOutput::printCurrentGroupEnded(const TestResult& res)
{
    if (outputOne_)
        outputOne_->printCurrentGroupEnded(res);
    if (outputTwo_)
        outputTwo_->printCurrentGroupEnded(res);
}

void CompositeTestOutput::verbose(VerbosityLevel level)
{
    if (outputOne_)
        outputOne_->verbose(level);
    if (outputTwo_)
        outputTwo_->verbose(level);
}

void CompositeTestOutput::color()
{
    if (outputOne_)
        outputOne_->color();
    if (outputTwo_)
        outputTwo_->color();
}

void CompositeTestOutput::printBuffer(const char* buffer)
{
    if (outputOne_)
        outputOne_->printBuffer(buffer);
    if (outputTwo_)
        outputTwo_->printBuffer(buffer);
}

void CompositeTestOutput::print(const char* buffer)
{
    if (outputOne_)
        outputOne_->print(buffer);
    if (outputTwo_)
        outputTwo_->print(buffer);
}

void CompositeTestOutput::print(long number)
{
    if (outputOne_)
        outputOne_->print(number);
    if (outputTwo_)
        outputTwo_->print(number);
}

void CompositeTestOutput::print(size_t number)
{
    if (outputOne_)
        outputOne_->print(number);
    if (outputTwo_)
        outputTwo_->print(number);
}

void CompositeTestOutput::printDouble(double number)
{
    if (outputOne_)
        outputOne_->printDouble(number);
    if (outputTwo_)
        outputTwo_->printDouble(number);
}

void CompositeTestOutput::printFailure(const TestFailure& failure)
{
    if (outputOne_)
        outputOne_->printFailure(failure);
    if (outputTwo_)
        outputTwo_->printFailure(failure);
}

void CompositeTestOutput::setProgressIndicator(const char* indicator)
{
    if (outputOne_)
        outputOne_->setProgressIndicator(indicator);
    if (outputTwo_)
        outputTwo_->setProgressIndicator(indicator);
}

void CompositeTestOutput::flush()
{
    if (outputOne_)
        outputOne_->flush();
    if (outputTwo_)
        outputTwo_->flush();
}

}
