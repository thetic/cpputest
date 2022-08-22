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

#include "CppUTest/CommandLineTestRunner.hpp"
#include "CppUTest/TestHarness.hpp"
#include "CppUTest/TestTestingFixture.hpp"

#if defined(CPPUTEST_HAVE_FORK) && defined(CPPUTEST_HAVE_WAITPID)
#include <cerrno>
#include <csignal>
#include <unistd.h>
#endif

static const std::string str1("abc");
static const std::string str2("def");
static const std::string str3(str1 + str2);

TEST_GROUP(UTestPlatformsTest_PlatformSpecificRunTestInASeperateProcess)
{
    cpputest::TestTestingFixture fixture;
};

// There is a possibility that a compiler provides fork but not waitpid.
#if !defined(CPPUTEST_HAVE_FORK) || !defined(CPPUTEST_HAVE_WAITPID)

TEST(UTestPlatformsTest_PlatformSpecificRunTestInASeperateProcess, DummyFailsWithMessage)
{
    fixture.setRunTestsInSeperateProcess();
    fixture.runAllTests();
    fixture.assertPrintContains("-p doesn't work on this platform, as it is lacking fork.\b");
}

#else

static void failFunction_()
{
    FAIL("This test fails");
}

[[noreturn]] static void exitNonZeroFunction_()
{
    exit(1);
}

static void stoppedTestFunction_()
{
    kill(getpid(), SIGSTOP);
}

TEST(UTestPlatformsTest_PlatformSpecificRunTestInASeperateProcess, TestInSeparateProcessWorks)
{
    fixture.setRunTestsInSeperateProcess();
    fixture.runAllTests();
    fixture.assertPrintContains("OK (1 tests, 1 ran, 0 checks, 0 ignored, 0 filtered out");
}

TEST(UTestPlatformsTest_PlatformSpecificRunTestInASeperateProcess, FailureInSeparateProcessWorks)
{
    fixture.setRunTestsInSeperateProcess();
    fixture.setTestFunction(failFunction_);
    fixture.runAllTests();
    fixture.assertPrintContains("Failed in separate process");
    fixture.assertPrintContains("Errors (1 failures, 1 tests, 1 ran, 0 checks, 0 ignored, 0 filtered out");
}

TEST(UTestPlatformsTest_PlatformSpecificRunTestInASeperateProcess, StoppedInSeparateProcessWorks)
{
    fixture.setRunTestsInSeperateProcess();
    fixture.setTestFunction(stoppedTestFunction_);
    fixture.runAllTests();
    fixture.assertPrintContains("Stopped in separate process - continuing");
    fixture.assertPrintContains("Errors (1 failures, 1 tests, 1 ran");
}

TEST(UTestPlatformsTest_PlatformSpecificRunTestInASeperateProcess, MultipleTestsInSeparateProcessAreCountedProperly)
{
    fixture.setRunTestsInSeperateProcess();
    fixture.runTestWithMethod(nullptr);
    fixture.runTestWithMethod(stoppedTestFunction_);
    fixture.runTestWithMethod(nullptr);
    fixture.runTestWithMethod(exitNonZeroFunction_);
    fixture.runTestWithMethod(nullptr);
    fixture.assertPrintContains("Failed in separate process");
    fixture.assertPrintContains("Stopped in separate process");
    fixture.assertPrintContains("Errors (2 failures, 5 tests, 5 ran, 0 checks, 0 ignored, 0 filtered out");
}

#endif
