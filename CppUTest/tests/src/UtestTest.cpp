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

#include "CppUTest/Utest.hpp"

#include "CppUTest/TestHarness.hpp"
#include "CppUTest/TestTestingFixture.hpp"

#include <cmath>

TEST_GROUP(UtestShell)
{
    cpputest::TestTestingFixture fixture;
};

static void failMethod_()
{
    FAIL("This test fails");
}

static void passingTestMethod_()
{
    CHECK(true);
}

static void passingCheckEqualTestMethod_()
{
    CHECK_EQUAL(1, 1);
}

static void exitTestMethod_()
{
    TEST_EXIT;
    FAIL("Should not get here");
}

TEST(UtestShell, compareDoubles)
{
    CHECK(cpputest::doubles_equal(1.0, 1.001, 0.01));
    CHECK(!cpputest::doubles_equal(1.0, 1.1, 0.05));
    double a = 1.2345678;
    CHECK(cpputest::doubles_equal(a, a, 0.000000001));
}

#if CPPUTEST_HAS_NAN == 1
TEST(UtestShell, compareDoublesNaN)
{
    CHECK(!cpputest::doubles_equal(NAN, 1.001, 0.01));
    CHECK(!cpputest::doubles_equal(1.0, NAN, 0.01));
    CHECK(!cpputest::doubles_equal(1.0, 1.001, NAN));
}
#endif

#if CPPUTEST_HAS_INF == 1
TEST(UtestShell, compareDoublesInf)
{
    CHECK(!cpputest::doubles_equal(INFINITY, 1.0, 0.01));
    CHECK(!cpputest::doubles_equal(1.0, INFINITY, 0.01));
    CHECK(cpputest::doubles_equal(1.0, -1.0, INFINITY));
    CHECK(cpputest::doubles_equal(INFINITY, INFINITY, 0.01));
    CHECK(cpputest::doubles_equal(INFINITY, INFINITY, INFINITY));
}
#endif

TEST(UtestShell, FailWillIncreaseTheAmountOfChecks)
{
    fixture.setTestFunction(failMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getCheckCount());
}

TEST(UtestShell, PassedCheckEqualWillIncreaseTheAmountOfChecks)
{
    fixture.setTestFunction(passingCheckEqualTestMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getCheckCount());
}

TEST(UtestShell, MacrosUsedInSetup)
{
    fixture.setSetup(failMethod_);
    fixture.setTestFunction(passingTestMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getFailureCount());
}

TEST(UtestShell, MacrosUsedInTearDown)
{
    fixture.setTeardown(failMethod_);
    fixture.setTestFunction(passingTestMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getFailureCount());
}

TEST(UtestShell, ExitLeavesQuietly)
{
    fixture.setTestFunction(exitTestMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(0, fixture.getFailureCount());
}

static bool cpputestHasCrashed;

static void crashMethod()
{
    cpputestHasCrashed = true;
}

TEST(UtestShell, FailWillNotCrashIfNotEnabled)
{
    cpputestHasCrashed = false;
    cpputest::UtestShell::setCrashMethod(crashMethod);

    fixture.setTestFunction(failMethod_);
    fixture.runAllTests();

    CHECK_FALSE(cpputestHasCrashed);
    LONGS_EQUAL(1, fixture.getFailureCount());

    cpputest::UtestShell::resetCrashMethod();
}

TEST(UtestShell, FailWillCrashIfEnabled)
{
    cpputestHasCrashed = false;
    cpputest::UtestShell::setCrashOnFail();
    cpputest::UtestShell::setCrashMethod(crashMethod);

    fixture.setTestFunction(failMethod_);
    fixture.runAllTests();

    CHECK(cpputestHasCrashed);

    cpputest::UtestShell::restoreDefaultTestTerminator();
    cpputest::UtestShell::resetCrashMethod();
}

static int teardownCalled = 0;

static void teardownMethod_()
{
    teardownCalled++;
}

TEST(UtestShell, TeardownCalledAfterTestFailure)
{
    teardownCalled = 0;
    fixture.setTeardown(teardownMethod_);
    fixture.setTestFunction(failMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getFailureCount());
    LONGS_EQUAL(1, teardownCalled);
}

static int stopAfterFailure = 0;
static void stopAfterFailureMethod_()
{
    FAIL("fail");
    stopAfterFailure++;
}

TEST(UtestShell, TestStopsAfterTestFailure)
{
    stopAfterFailure = 0;
    fixture.setTestFunction(stopAfterFailureMethod_);
    fixture.runAllTests();
    CHECK(fixture.hasTestFailed());
    LONGS_EQUAL(1, fixture.getFailureCount());
    LONGS_EQUAL(0, stopAfterFailure);
}

TEST(UtestShell, TestStopsAfterSetupFailure)
{
    stopAfterFailure = 0;
    fixture.setSetup(stopAfterFailureMethod_);
    fixture.setTeardown(stopAfterFailureMethod_);
    fixture.setTestFunction(failMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(2, fixture.getFailureCount());
    LONGS_EQUAL(0, stopAfterFailure);
}

TEST(UtestShell, veryVebose)
{
    cpputest::UtestShell shell("Group", "name", __FILE__, __LINE__);
    cpputest::StringBufferTestOutput normalOutput;
    normalOutput.verbose(cpputest::TestOutput::level_veryVerbose);
    cpputest::NullTestPlugin plugin;

    cpputest::TestResult result(normalOutput);
    shell.runOneTestInCurrentProcess(&plugin, result);
    STRCMP_CONTAINS("\n------ before runTest", normalOutput.getOutput().c_str());
}

class defaultUtestShell : public cpputest::UtestShell {
};

TEST(UtestShell, this_test_covers_the_UtestShell_createTest_and_Utest_testBody_methods)
{
    defaultUtestShell shell;
    fixture.addTest(&shell);
    fixture.runAllTests();
    LONGS_EQUAL(2, fixture.getTestCount());
}

static void StubPlatformSpecificRunTestInASeperateProcess(cpputest::UtestShell* shell, cpputest::TestPlugin*, cpputest::TestResult* result)
{
    result->addFailure(cpputest::TestFailure(shell, "Failed in separate process"));
}

TEST(UtestShell, RunInSeparateProcessTest)
{
    UT_PTR_SET(cpputest::UtestShell::run_test_process, StubPlatformSpecificRunTestInASeperateProcess);
    fixture.getRegistry()->setRunTestsInSeperateProcess();
    fixture.runAllTests();
    fixture.assertPrintContains("Failed in separate process");
}

// There is a possibility that a compiler provides fork but not waitpid.
#if !defined(CPPUTEST_HAVE_FORK) || !defined(CPPUTEST_HAVE_WAITPID)

IGNORE_TEST(UtestShell, TestDefaultCrashMethodInSeparateProcessTest)
{
}

#else

#if defined(__has_feature)
#if __has_feature(address_sanitizer) || __has_feature(undefined_behavior_sanitizer)
#define SANITIZING
#endif
#endif

#ifndef SANITIZING

TEST(UtestShell, TestDefaultCrashMethodInSeparateProcessTest)
{
    fixture.setTestFunction(cpputest::UtestShell::crash);
    fixture.setRunTestsInSeperateProcess();
    fixture.runAllTests();
    fixture.assertPrintContains("Failed in separate process - killed by signal");

    /* Signal 11 usually happens, but with clang3.7 on Linux, it produced signal 4 */
    CHECK((fixture.getOutput().find("signal 11") != std::string::npos) || (fixture.getOutput().find("signal 4") != std::string::npos));
}

#endif

#endif

#if !CPPUTEST_NO_EXCEPTIONS

static bool destructorWasCalledOnFailedTest = false;

static void destructorCalledForLocalObjects_()
{
    cpputest::SetBooleanOnDestructorCall pleaseCallTheDestructor(destructorWasCalledOnFailedTest);
    destructorWasCalledOnFailedTest = false;
    FAIL("fail");
}

TEST(UtestShell, DestructorIsCalledForLocalObjectsWhenTheTestFails)
{
    fixture.setTestFunction(destructorCalledForLocalObjects_);
    fixture.runAllTests();
    CHECK(destructorWasCalledOnFailedTest);
}

#endif

TEST_GROUP(IgnoredUtestShell)
{
    cpputest::TestTestingFixture fixture;
    cpputest::IgnoredUtestShell ignoredTest;
    cpputest::ExecFunctionTestShell normalUtestShell;

    void setup() override
    {
        fixture.addTest(&ignoredTest);
        fixture.addTest(&normalUtestShell);
    }
};

TEST(IgnoredUtestShell, doesIgnoreCount)
{
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getIgnoreCount());
}

TEST(IgnoredUtestShell, printsIGNORE_TESTwhenVerbose)
{
    fixture.setOutputVerbose();
    fixture.runAllTests();
    fixture.assertPrintContains("IGNORE_TEST");
}

TEST(IgnoredUtestShell, runIgnoredOptionSpecifiedThenIncreaseRunCount)
{
    ignoredTest.setRunIgnored();
    fixture.runAllTests();
    LONGS_EQUAL(3, fixture.getRunCount());
    LONGS_EQUAL(0, fixture.getIgnoreCount());
}

TEST(IgnoredUtestShell, runIgnoredOptionNotSpecifiedThenIncreaseIgnoredCount)
{
    fixture.runAllTests();
    LONGS_EQUAL(2, fixture.getRunCount());
    LONGS_EQUAL(1, fixture.getIgnoreCount());
}

TEST(IgnoredUtestShell, runIgnoredOptionSpecifiedWillNotInfluenceNormalTestCount)
{
    normalUtestShell.setRunIgnored();
    fixture.runAllTests();
    LONGS_EQUAL(2, fixture.getRunCount());
    LONGS_EQUAL(1, fixture.getIgnoreCount());
}

TEST(IgnoredUtestShell, runIgnoredOptionSpecifiedThenReturnTESTInFormattedName)
{
    ignoredTest.setGroupName("TestGroup");
    ignoredTest.setTestName("TestName");
    ignoredTest.setRunIgnored();
    fixture.runAllTests();
    STRCMP_EQUAL("TEST(TestGroup, TestName)", ignoredTest.getFormattedName().c_str());
}

TEST(IgnoredUtestShell, runIgnoredOptionNotSpecifiedThenReturnIGNORETESTInFormattedName)
{
    ignoredTest.setGroupName("TestGroup");
    ignoredTest.setTestName("TestName");
    fixture.runAllTests();
    STRCMP_EQUAL("IGNORE_TEST(TestGroup, TestName)", ignoredTest.getFormattedName().c_str());
}

TEST(IgnoredUtestShell, runIgnoredOptionNotSpecifiedThenWillRunReturnFalse)
{
    CHECK_FALSE(ignoredTest.willRun());
}

TEST(IgnoredUtestShell, runIgnoredOptionSpecifiedThenWillRunReturnTrue)
{
    ignoredTest.setRunIgnored();
    CHECK_TRUE(ignoredTest.willRun());
}

TEST_BASE(MyOwnTest)
{
    MyOwnTest()
        : inTest(false)
    {
    }
    bool inTest;

    void setup() override
    {
        CHECK(!inTest);
        inTest = true;
    }
    void teardown() override
    {
        CHECK(inTest);
        inTest = false;
    }
};

TEST_GROUP_BASE(UtestMyOwn, MyOwnTest) {};

TEST(UtestMyOwn, test)
{
    CHECK(inTest);
}

class NullParameterTest : public cpputest::UtestShell {
};

TEST(UtestMyOwn, NullParameters)
{
    NullParameterTest nullTest; /* Bug fix tests for creating a test without a name, fix in SimpleString */
    cpputest::TestFilter emptyFilter;
    CHECK(nullTest.shouldRun(&emptyFilter, &emptyFilter));
}

class AllocateAndDeallocateInConstructorAndDestructor {
    char* memory_;
    char* morememory_;

public:
    AllocateAndDeallocateInConstructorAndDestructor()
    {
        memory_ = new char[100];
        morememory_ = nullptr;
    }
    void allocateMoreMemory()
    {
        morememory_ = new char[123];
    }

    ~AllocateAndDeallocateInConstructorAndDestructor()
    {
        delete[] memory_;
        delete[] morememory_;
    }
};

TEST_GROUP(CanHaveMemberVariablesInTestGroupThatAllocateMemoryWithoutCausingMemoryLeaks)
{
    AllocateAndDeallocateInConstructorAndDestructor dummy;
};

TEST(CanHaveMemberVariablesInTestGroupThatAllocateMemoryWithoutCausingMemoryLeaks, testInTestGroupName)
{
    dummy.allocateMoreMemory();
}

static int getZero()
{
    return 0;
}

static int getOne()
{
    return 1;
}

TEST_GROUP(UtestShellPointerArrayTest)
{
    cpputest::UtestShell* test0;
    cpputest::UtestShell* test1;
    cpputest::UtestShell* test2;

    void setup() override
    {
        test0 = new cpputest::IgnoredUtestShell();
        test1 = new cpputest::IgnoredUtestShell();
        test2 = new cpputest::IgnoredUtestShell();

        test0->addTest(test1);
        test1->addTest(test2);
    }

    void teardown() override
    {
        delete test0;
        delete test1;
        delete test2;
    }
};

TEST(UtestShellPointerArrayTest, empty)
{
    cpputest::UtestShellPointerArray tests(nullptr);
    tests.shuffle(0);
    CHECK(nullptr == tests.getFirstTest());
}

TEST(UtestShellPointerArrayTest, testsAreInOrder)
{
    cpputest::UtestShellPointerArray tests(test0);
    CHECK(tests.get(0) == test0);
    CHECK(tests.get(1) == test1);
    CHECK(tests.get(2) == test2);
}

TEST(UtestShellPointerArrayTest, relinkingTestsWillKeepThemTheSameWhenNothingWasDone)
{
    cpputest::UtestShellPointerArray tests(test0);
    tests.relinkTestsInOrder();
    CHECK(tests.get(0) == test0);
    CHECK(tests.get(1) == test1);
    CHECK(tests.get(2) == test2);
}

TEST(UtestShellPointerArrayTest, firstTestisNotTheFirstTestWithSeed1234)
{
    cpputest::UtestShellPointerArray tests(test0);
    tests.shuffle(1234);
    CHECK(tests.getFirstTest() != test0);
}

TEST(UtestShellPointerArrayTest, ShuffleListTestWithRandomAlwaysReturningZero)
{
    UT_PTR_SET(cpputest::UtestShellPointerArray::rand_, getZero);

    cpputest::UtestShellPointerArray tests(test0);
    tests.shuffle(3);
    CHECK(tests.get(0) == test1);
    CHECK(tests.get(1) == test2);
    CHECK(tests.get(2) == test0);
}

// swaps with 4 mod 3 (1) then 4 mod 2 (0): 1, [2], [0] --> [1], [0], 2 --> 0, 1, 2
TEST(UtestShellPointerArrayTest, ShuffleListTestWithRandomAlwaysReturningOne)
{
    UT_PTR_SET(cpputest::UtestShellPointerArray::rand_, getOne);

    cpputest::UtestShellPointerArray tests(test0);
    tests.shuffle(3);
    CHECK(tests.get(0) == test0);
    CHECK(tests.get(1) == test2);
    CHECK(tests.get(2) == test1);
}

TEST(UtestShellPointerArrayTest, reverse)
{
    UT_PTR_SET(cpputest::UtestShellPointerArray::rand_, getOne);

    cpputest::UtestShellPointerArray tests(test0);
    tests.reverse();
    CHECK(tests.get(0) == test2);
    CHECK(tests.get(1) == test1);
    CHECK(tests.get(2) == test0);
}
