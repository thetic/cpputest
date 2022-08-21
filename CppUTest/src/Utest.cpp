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

#include "CppUTest/PlatformSpecificFunctions.h"
#include "CppUTest/PlatformSpecificFunctions.hpp"
#include "CppUTest/TestFailure.hpp"
#include "CppUTest/TestOutput.hpp"
#include "CppUTest/TestPlugin.hpp"
#include "CppUTest/TestRegistry.hpp"
#include "CppUTest/TestResult.hpp"

#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstring>

#if defined(__GNUC__) && __GNUC__ >= 11
#define NEEDS_DISABLE_NULL_WARNING
#endif /* GCC >= 11 */

namespace {
std::jmp_buf test_exit_jmp_buf[10];
int jmp_buf_index = 0;

int do_set_jump(void (*function)(void* data), void* data)
{
    if (0 == setjmp(test_exit_jmp_buf[jmp_buf_index])) {
        jmp_buf_index++;
        function(data);
        jmp_buf_index--;
        return 1;
    }
    return 0;
}

[[noreturn]] void do_long_jump()
{
    jmp_buf_index--;
    std::longjmp(test_exit_jmp_buf[jmp_buf_index], 1);
}

#if !CPPUTEST_NO_EXCEPTIONS
void restore_jump_buffer()
{
    jmp_buf_index--;
}
#endif

}

bool doubles_equal(double d1, double d2, double threshold)
{
    if (std::isnan(d1) || std::isnan(d2) || std::isnan(threshold))
        return false;

    if (std::isinf(d1) && std::isinf(d2)) {
        return true;
    }

    return std::fabs(d1 - d2) <= threshold;
}

/* Sometimes stubs use the CppUTest assertions.
 * Its not correct to do so, but this small helper class will prevent a segmentation fault and instead
 * will give an error message and also the file/line of the check that was executed outside the tests.
 */
class OutsideTestRunnerUTest : public UtestShell {
public:
    static OutsideTestRunnerUTest& instance();
    virtual TestResult& getTestResult()
    {
        return defaultTestResult;
    }
    ~OutsideTestRunnerUTest() override
    {
    }

private:
    OutsideTestRunnerUTest()
        : UtestShell("\n\t NOTE: Assertion happened without being in a test run (perhaps in main?)", "\n\t       Something is very wrong. Check this assertion and fix", "unknown file", 0)
        , defaultTestResult(defaultOutput)
    {
    }
    ConsoleTestOutput defaultOutput;
    TestResult defaultTestResult;
};

OutsideTestRunnerUTest& OutsideTestRunnerUTest::instance()
{
    static OutsideTestRunnerUTest instance_;
    return instance_;
}

/*
 * Below helpers are used for the do_set_jump and do_long_jump. They pass a method for what needs to happen after
 * the jump, so that the stack stays right.
 *
 */

static void helperDoTestSetup(void* data)
{
    ((Utest*)data)->setup();
}

static void helperDoTestBody(void* data)
{
    ((Utest*)data)->testBody();
}

static void helperDoTestTeardown(void* data)
{
    ((Utest*)data)->teardown();
}

struct HelperTestRunInfo {
    HelperTestRunInfo(UtestShell* shell, TestPlugin* plugin, TestResult* result)
        : shell_(shell)
        , plugin_(plugin)
        , result_(result)
    {
    }

    UtestShell* shell_;
    TestPlugin* plugin_;
    TestResult* result_;
};

static void helperDoRunOneTestInCurrentProcess(void* data)
{
    HelperTestRunInfo* runInfo = (HelperTestRunInfo*)data;

    UtestShell* shell = runInfo->shell_;
    TestPlugin* plugin = runInfo->plugin_;
    TestResult* result = runInfo->result_;

    shell->runOneTestInCurrentProcess(plugin, *result);
}

static void helperDoRunOneTestSeperateProcess(void* data)
{
    HelperTestRunInfo* runInfo = (HelperTestRunInfo*)data;

    UtestShell* shell = runInfo->shell_;
    TestPlugin* plugin = runInfo->plugin_;
    TestResult* result = runInfo->result_;
    PlatformSpecificRunTestInASeperateProcess(shell, plugin, result);
}

/******************************** */

static const NormalTestTerminator normalTestTerminator;
static const CrashingTestTerminator crashingTestTerminator;

const TestTerminator* UtestShell::currentTestTerminator_ = &normalTestTerminator;

bool UtestShell::rethrowExceptions_ = false;

/******************************** */

UtestShell::UtestShell()
    : group_("UndefinedTestGroup")
    , name_("UndefinedTest")
    , file_("UndefinedFile")
    , lineNumber_(0)
    , next_(nullptr)
    , isRunAsSeperateProcess_(false)
    , hasFailed_(false)
{
}

UtestShell::UtestShell(const char* groupName, const char* testName, const char* fileName, size_t lineNumber)
    : group_(groupName)
    , name_(testName)
    , file_(fileName)
    , lineNumber_(lineNumber)
    , next_(nullptr)
    , isRunAsSeperateProcess_(false)
    , hasFailed_(false)
{
}

UtestShell::UtestShell(const char* groupName, const char* testName, const char* fileName, size_t lineNumber, UtestShell* nextTest)
    : group_(groupName)
    , name_(testName)
    , file_(fileName)
    , lineNumber_(lineNumber)
    , next_(nextTest)
    , isRunAsSeperateProcess_(false)
    , hasFailed_(false)
{
}

UtestShell::~UtestShell()
{
}

// LCOV_EXCL_START - actually covered but not in .gcno due to race condition
#ifdef NEEDS_DISABLE_NULL_WARNING
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#endif /* NEEDS_DISABLE_NULL_WARNING */

static void defaultCrashMethod()
{
    std::raise(SIGSEGV);
}

#ifdef NEEDS_DISABLE_NULL_WARNING
#pragma GCC diagnostic pop
#endif /* NEEDS_DISABLE_NULL_WARNING */
// LCOV_EXCL_STOP

static void (*pleaseCrashMeRightNow)() = defaultCrashMethod;

void UtestShell::setCrashMethod(void (*crashme)())
{
    pleaseCrashMeRightNow = crashme;
}

void UtestShell::resetCrashMethod()
{
    pleaseCrashMeRightNow = defaultCrashMethod;
}

void UtestShell::crash()
{
    pleaseCrashMeRightNow();
}

void UtestShell::runOneTest(TestPlugin* plugin, TestResult& result)
{
    hasFailed_ = false;
    result.countRun();
    HelperTestRunInfo runInfo(this, plugin, &result);
    if (isRunInSeperateProcess())
        do_set_jump(helperDoRunOneTestSeperateProcess, &runInfo);
    else
        do_set_jump(helperDoRunOneTestInCurrentProcess, &runInfo);
}

Utest* UtestShell::createTest()
{
    return new Utest();
}

void UtestShell::destroyTest(Utest* test)
{
    delete test;
}

void UtestShell::runOneTestInCurrentProcess(TestPlugin* plugin, TestResult& result)
{
    result.printVeryVerbose("\n-- before runAllPreTestAction: ");
    plugin->runAllPreTestAction(*this, result);
    result.printVeryVerbose("\n-- after runAllPreTestAction: ");

    // save test context, so that test class can be tested
    UtestShell* savedTest = UtestShell::getCurrent();
    TestResult* savedResult = UtestShell::getTestResult();

    UtestShell::setTestResult(&result);
    UtestShell::setCurrentTest(this);

    Utest* testToRun = nullptr;

#if !CPPUTEST_NO_EXCEPTIONS
    try {
#endif
        result.printVeryVerbose("\n---- before createTest: ");
        testToRun = createTest();
        result.printVeryVerbose("\n---- after createTest: ");

        result.printVeryVerbose("\n------ before runTest: ");
        testToRun->run();
        result.printVeryVerbose("\n------ after runTest: ");

        UtestShell::setCurrentTest(savedTest);
        UtestShell::setTestResult(savedResult);
#if !CPPUTEST_NO_EXCEPTIONS
    } catch (...) {
        destroyTest(testToRun);
        throw;
    }
#endif

    result.printVeryVerbose("\n---- before destroyTest: ");
    destroyTest(testToRun);
    result.printVeryVerbose("\n---- after destroyTest: ");

    result.printVeryVerbose("\n-- before runAllPostTestAction: ");
    plugin->runAllPostTestAction(*this, result);
    result.printVeryVerbose("\n-- after runAllPostTestAction: ");
}

UtestShell* UtestShell::getNext() const
{
    return next_;
}

UtestShell* UtestShell::addTest(UtestShell* test)
{
    next_ = test;
    return this;
}

size_t UtestShell::countTests()
{
    return next_ ? next_->countTests() + 1 : 1;
}

std::string UtestShell::getMacroName() const
{
    return "TEST";
}

const std::string UtestShell::getName() const
{
    return name_;
}

const std::string UtestShell::getGroup() const
{
    return group_;
}

std::string UtestShell::getFormattedName() const
{
    std::string formattedName(getMacroName());
    formattedName += "(";
    formattedName += group_;
    formattedName += ", ";
    formattedName += name_;
    formattedName += ")";

    return formattedName;
}

bool UtestShell::hasFailed() const
{
    return hasFailed_;
}

void UtestShell::countCheck()
{
    getTestResult()->countCheck();
}

bool UtestShell::willRun() const
{
    return true;
}

bool UtestShell::isRunInSeperateProcess() const
{
    return isRunAsSeperateProcess_;
}

void UtestShell::setRunInSeperateProcess()
{
    isRunAsSeperateProcess_ = true;
}

void UtestShell::setRunIgnored()
{
}

void UtestShell::setFileName(const char* fileName)
{
    file_ = fileName;
}

void UtestShell::setLineNumber(size_t lineNumber)
{
    lineNumber_ = lineNumber;
}

void UtestShell::setGroupName(const char* groupName)
{
    group_ = groupName;
}

void UtestShell::setTestName(const char* testName)
{
    name_ = testName;
}

const std::string UtestShell::getFile() const
{
    return file_;
}

size_t UtestShell::getLineNumber() const
{
    return lineNumber_;
}

bool UtestShell::match(const char* target, const TestFilter* filters) const
{
    if (filters == nullptr)
        return true;

    for (; filters != nullptr; filters = filters->getNext())
        if (filters->match(target))
            return true;

    return false;
}

bool UtestShell::shouldRun(const TestFilter* groupFilters, const TestFilter* nameFilters) const
{
    return match(group_, groupFilters) && match(name_, nameFilters);
}

void UtestShell::failWith(const TestFailure& failure)
{
    failWith(failure, getCurrentTestTerminator());
} // LCOV_EXCL_LINE

void UtestShell::failWith(const TestFailure& failure, const TestTerminator& terminator)
{
    addFailure(failure);
    terminator.exitCurrentTest();
} // LCOV_EXCL_LINE

void UtestShell::addFailure(const TestFailure& failure)
{
    hasFailed_ = true;
    getTestResult()->addFailure(failure);
}

void UtestShell::exitTest(const TestTerminator& terminator)
{
    terminator.exitCurrentTest();
} // LCOV_EXCL_LINE

void UtestShell::assertTrue(bool condition, const char* checkString, const char* conditionString, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (!condition)
        failWith(CheckFailure(this, fileName, lineNumber, checkString, conditionString, text), testTerminator);
}

void UtestShell::fail(const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    failWith(FailFailure(this, fileName, lineNumber, text), testTerminator);
} // LCOV_EXCL_LINE

void UtestShell::assertCstrEqual(const char* expected, const char* actual, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (actual == nullptr && expected == nullptr)
        return;
    if (actual == nullptr || expected == nullptr)
        failWith(StringEqualFailure(this, fileName, lineNumber, expected, actual, text), testTerminator);
    if (std::strcmp(expected, actual) != 0)
        failWith(StringEqualFailure(this, fileName, lineNumber, expected, actual, text), testTerminator);
}

void UtestShell::assertCstrNEqual(const char* expected, const char* actual, size_t length, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (actual == nullptr && expected == nullptr)
        return;
    if (actual == nullptr || expected == nullptr)
        failWith(StringEqualFailure(this, fileName, lineNumber, expected, actual, text), testTerminator);
    if (std::strncmp(expected, actual, length) != 0)
        failWith(StringEqualFailure(this, fileName, lineNumber, expected, actual, text), testTerminator);
}

void UtestShell::assertCstrNoCaseEqual(const char* expected, const char* actual, const char* text, const char* fileName, size_t lineNumber)
{
    getTestResult()->countCheck();
    if (actual == nullptr && expected == nullptr)
        return;
    if (actual == nullptr || expected == nullptr)
        failWith(StringEqualNoCaseFailure(this, fileName, lineNumber, expected, actual, text));
    if (strings::lowercase(expected) != strings::lowercase(actual))
        failWith(StringEqualNoCaseFailure(this, fileName, lineNumber, expected, actual, text));
}

void UtestShell::assertCstrContains(
    const char* expected,
    const char* actual,
    const char* text,
    const char* fileName,
    size_t lineNumber)
{
    getTestResult()->countCheck();
    if (actual == nullptr && expected == nullptr)
        return;
    if (actual == nullptr || expected == nullptr)
        failWith(ContainsFailure(this, fileName, lineNumber, expected, actual, text));
    if (!strings::contains(actual, expected))
        failWith(ContainsFailure(this, fileName, lineNumber, expected, actual, text));
}

void UtestShell::assertCstrNoCaseContains(
    const char* expected,
    const char* actual,
    const char* text,
    const char* fileName,
    size_t lineNumber)
{
    getTestResult()->countCheck();
    if (actual == nullptr && expected == nullptr)
        return;
    if (actual == nullptr || expected == nullptr)
        failWith(ContainsFailure(this, fileName, lineNumber, expected, actual, text));
    if (!strings::contains(strings::lowercase(actual), strings::lowercase(expected)))
        failWith(ContainsFailure(this, fileName, lineNumber, expected, actual, text));
}

void UtestShell::assertLongsEqual(long expected, long actual, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (expected != actual)
        failWith(LongsEqualFailure(this, fileName, lineNumber, expected, actual, text), testTerminator);
}

void UtestShell::assertUnsignedLongsEqual(unsigned long expected, unsigned long actual, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (expected != actual)
        failWith(UnsignedLongsEqualFailure(this, fileName, lineNumber, expected, actual, text), testTerminator);
}

void UtestShell::assertLongLongsEqual(long long expected, long long actual, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (expected != actual)
        failWith(LongLongsEqualFailure(this, fileName, lineNumber, expected, actual, text), testTerminator);
}

void UtestShell::assertUnsignedLongLongsEqual(unsigned long long expected, unsigned long long actual, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (expected != actual)
        failWith(UnsignedLongLongsEqualFailure(this, fileName, lineNumber, expected, actual, text), testTerminator);
}

void UtestShell::assertSignedBytesEqual(signed char expected, signed char actual, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (expected != actual)
        failWith(SignedBytesEqualFailure(this, fileName, lineNumber, expected, actual, text), testTerminator);
}

void UtestShell::assertPointersEqual(const void* expected, const void* actual, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (expected != actual)
        failWith(EqualsFailure(this, fileName, lineNumber, StringFrom(expected), StringFrom(actual), text), testTerminator);
}

void UtestShell::assertFunctionPointersEqual(void (*expected)(), void (*actual)(), const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (expected != actual)
        failWith(EqualsFailure(this, fileName, lineNumber, StringFrom(expected), StringFrom(actual), text), testTerminator);
}

void UtestShell::assertDoublesEqual(double expected, double actual, double threshold, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (!doubles_equal(expected, actual, threshold))
        failWith(DoublesEqualFailure(this, fileName, lineNumber, expected, actual, threshold, text), testTerminator);
}

void UtestShell::assertBinaryEqual(const void* expected, const void* actual, size_t length, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (length == 0)
        return;
    if (actual == nullptr && expected == nullptr)
        return;
    if (actual == nullptr || expected == nullptr)
        failWith(BinaryEqualFailure(this, fileName, lineNumber, (const unsigned char*)expected, (const unsigned char*)actual, length, text), testTerminator);
    if (std::memcmp(expected, actual, length) != 0)
        failWith(BinaryEqualFailure(this, fileName, lineNumber, (const unsigned char*)expected, (const unsigned char*)actual, length, text), testTerminator);
}

void UtestShell::assertBitsEqual(unsigned long expected, unsigned long actual, unsigned long mask, size_t byteCount, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if ((expected & mask) != (actual & mask))
        failWith(BitsEqualFailure(this, fileName, lineNumber, expected, actual, mask, byteCount, text), testTerminator);
}

void UtestShell::assertEquals(bool failed, const char* expected, const char* actual, const char* text, const char* file, size_t line, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (failed)
        failWith(CheckEqualFailure(this, file, line, expected, actual, text), testTerminator);
}

void UtestShell::assertCompare(bool comparison, const char* checkString, const char* comparisonString, const char* text, const char* fileName, size_t lineNumber, const TestTerminator& testTerminator)
{
    getTestResult()->countCheck();
    if (!comparison)
        failWith(ComparisonFailure(this, fileName, lineNumber, checkString, comparisonString, text), testTerminator);
}

void UtestShell::print(const char* text, const char* fileName, size_t lineNumber)
{
    std::string stringToPrint = "\n";
    stringToPrint += fileName;
    stringToPrint += ":";
    stringToPrint += StringFrom(lineNumber);
    stringToPrint += " ";
    stringToPrint += text;
    getTestResult()->print(stringToPrint.c_str());
}

void UtestShell::print(const std::string& text, const char* fileName, size_t lineNumber)
{
    print(text.c_str(), fileName, lineNumber);
}

void UtestShell::printVeryVerbose(const char* text)
{
    getTestResult()->printVeryVerbose(text);
}

TestResult* UtestShell::testResult_ = nullptr;
UtestShell* UtestShell::currentTest_ = nullptr;

void UtestShell::setTestResult(TestResult* result)
{
    testResult_ = result;
}

void UtestShell::setCurrentTest(UtestShell* test)
{
    currentTest_ = test;
}

TestResult* UtestShell::getTestResult()
{
    if (testResult_ == nullptr)
        return &OutsideTestRunnerUTest::instance().getTestResult();
    return testResult_;
}

UtestShell* UtestShell::getCurrent()
{
    if (currentTest_ == nullptr)
        return &OutsideTestRunnerUTest::instance();
    return currentTest_;
}

const TestTerminator& UtestShell::getCurrentTestTerminator()
{
    return *currentTestTerminator_;
}

void UtestShell::setCrashOnFail()
{
    currentTestTerminator_ = &crashingTestTerminator;
}

void UtestShell::restoreDefaultTestTerminator()
{
    currentTestTerminator_ = &normalTestTerminator;
}

void UtestShell::setRethrowExceptions(bool rethrowExceptions)
{
    rethrowExceptions_ = rethrowExceptions;
}

bool UtestShell::isRethrowingExceptions()
{
    return rethrowExceptions_;
}

ExecFunctionTestShell::~ExecFunctionTestShell()
{
}

////////////// Utest ////////////

Utest::Utest()
{
}

Utest::~Utest()
{
}

#if CPPUTEST_NO_EXCEPTIONS
void Utest::run()
{
    if (do_set_jump(helperDoTestSetup, this)) {
        do_set_jump(helperDoTestBody, this);
    }
    do_set_jump(helperDoTestTeardown, this);
}
#else
void Utest::run()
{
    UtestShell* current = UtestShell::getCurrent();
    int jumpResult = 0;
    try {
        current->printVeryVerbose("\n-------- before setup: ");
        jumpResult = do_set_jump(helperDoTestSetup, this);
        current->printVeryVerbose("\n-------- after  setup: ");

        if (jumpResult) {
            current->printVeryVerbose("\n----------  before body: ");
            do_set_jump(helperDoTestBody, this);
            current->printVeryVerbose("\n----------  after body: ");
        }
    } catch (CppUTestFailedException&) {
        restore_jump_buffer();
    } catch (const std::exception& e) {
        current->addFailure(UnexpectedExceptionFailure(current, e));
        restore_jump_buffer();
        if (current->isRethrowingExceptions()) {
            throw;
        }
    } catch (...) {
        current->addFailure(UnexpectedExceptionFailure(current));
        restore_jump_buffer();
        if (current->isRethrowingExceptions()) {
            throw;
        }
    }

    try {
        current->printVeryVerbose("\n--------  before teardown: ");
        do_set_jump(helperDoTestTeardown, this);
        current->printVeryVerbose("\n--------  after teardown: ");
    } catch (CppUTestFailedException&) {
        restore_jump_buffer();
    } catch (const std::exception& e) {
        current->addFailure(UnexpectedExceptionFailure(current, e));
        restore_jump_buffer();
        if (current->isRethrowingExceptions()) {
            throw;
        }
    } catch (...) {
        current->addFailure(UnexpectedExceptionFailure(current));
        restore_jump_buffer();
        if (current->isRethrowingExceptions()) {
            throw;
        }
    }
}
#endif

void Utest::setup()
{
}

void Utest::testBody()
{
}

void Utest::teardown()
{
}

/////////////////// Terminators

TestTerminator::~TestTerminator()
{
}

void NormalTestTerminator::exitCurrentTest() const
{
#if CPPUTEST_NO_EXCEPTIONS
    TestTerminatorWithoutExceptions().exitCurrentTest();
#else
    throw CppUTestFailedException();
#endif
}

NormalTestTerminator::~NormalTestTerminator()
{
}

void TestTerminatorWithoutExceptions::exitCurrentTest() const
{
    do_long_jump();
} // LCOV_EXCL_LINE

TestTerminatorWithoutExceptions::~TestTerminatorWithoutExceptions()
{
}

void CrashingTestTerminator::exitCurrentTest() const
{
    UtestShell::crash();
    NormalTestTerminator::exitCurrentTest();
}

CrashingTestTerminator::~CrashingTestTerminator()
{
}

//////////////////// ExecFunction
//
ExecFunction::ExecFunction()
{
}

ExecFunction::~ExecFunction()
{
}

ExecFunctionWithoutParameters::ExecFunctionWithoutParameters(void (*testFunction)())
    : testFunction_(testFunction)
{
}

ExecFunctionWithoutParameters::~ExecFunctionWithoutParameters()
{
}

void ExecFunctionWithoutParameters::exec()
{
    if (testFunction_)
        testFunction_();
}

//////////////////// ExecFunctionTest

ExecFunctionTest::ExecFunctionTest(ExecFunctionTestShell* shell)
    : shell_(shell)
{
}

void ExecFunctionTest::testBody()
{
    if (shell_->testFunction_)
        shell_->testFunction_->exec();
}

void ExecFunctionTest::setup()
{
    if (shell_->setup_)
        shell_->setup_();
}

void ExecFunctionTest::teardown()
{
    if (shell_->teardown_)
        shell_->teardown_();
}

/////////////// IgnoredUtestShell /////////////
IgnoredUtestShell::IgnoredUtestShell()
    : runIgnored_(false)
{
}

IgnoredUtestShell::IgnoredUtestShell(const char* groupName, const char* testName, const char* fileName, size_t lineNumber)
    : UtestShell(groupName, testName, fileName, lineNumber)
    , runIgnored_(false)
{
}

IgnoredUtestShell::~IgnoredUtestShell()
{
}

bool IgnoredUtestShell::willRun() const
{
    if (runIgnored_)
        return UtestShell::willRun();

    return false;
}

std::string IgnoredUtestShell::getMacroName() const
{
    if (runIgnored_)
        return "TEST";

    return "IGNORE_TEST";
}

void IgnoredUtestShell::runOneTest(TestPlugin* plugin, TestResult& result)
{
    if (runIgnored_) {
        UtestShell::runOneTest(plugin, result);
        return;
    }

    result.countIgnored();
}

void IgnoredUtestShell::setRunIgnored()
{
    runIgnored_ = true;
}

//////////////////// UtestShellPointerArray

UtestShellPointerArray::UtestShellPointerArray(UtestShell* firstTest)
    : arrayOfTests_(nullptr)
    , count_(0)
{
    count_ = (firstTest) ? firstTest->countTests() : 0;
    if (count_ == 0)
        return;

    arrayOfTests_ = new UtestShell*[count_];

    UtestShell* currentTest = firstTest;
    for (size_t i = 0; i < count_; i++) {
        arrayOfTests_[i] = currentTest;
        currentTest = currentTest->getNext();
    }
}

UtestShellPointerArray::~UtestShellPointerArray()
{
    delete[] arrayOfTests_;
}

void UtestShellPointerArray::swap(size_t index1, size_t index2)
{
    UtestShell* e2 = arrayOfTests_[index2];
    UtestShell* e1 = arrayOfTests_[index1];
    arrayOfTests_[index1] = e2;
    arrayOfTests_[index2] = e1;
}

void UtestShellPointerArray::shuffle(size_t seed)
{
    if (count_ == 0)
        return;

    PlatformSpecificSrand((unsigned int)seed);

    for (size_t i = count_ - 1; i >= 1; --i) {
        if (count_ == 0)
            return;

        const size_t j = ((size_t)PlatformSpecificRand()) % (i + 1); // distribution biased by modulo, but good enough for shuffling
        swap(i, j);
    }
    relinkTestsInOrder();
}

void UtestShellPointerArray::reverse()
{
    if (count_ == 0)
        return;

    size_t halfCount = count_ / 2;
    for (size_t i = 0; i < halfCount; i++) {
        size_t j = count_ - i - 1;
        swap(i, j);
    }
    relinkTestsInOrder();
}

void UtestShellPointerArray::relinkTestsInOrder()
{
    UtestShell* tests = nullptr;
    for (size_t i = 0; i < count_; i++)
        tests = arrayOfTests_[count_ - i - 1]->addTest(tests);
}

UtestShell* UtestShellPointerArray::getFirstTest() const
{
    return get(0);
}

UtestShell* UtestShellPointerArray::get(size_t index) const
{
    if (index >= count_)
        return nullptr;
    return arrayOfTests_[index];
}

////////////// TestInstaller ////////////

TestInstaller::TestInstaller(UtestShell& shell, const char* groupName, const char* testName, const char* fileName, size_t lineNumber)
{
    shell.setGroupName(groupName);
    shell.setTestName(testName);
    shell.setFileName(fileName);
    shell.setLineNumber(lineNumber);
    TestRegistry::getCurrentRegistry()->addTest(&shell);
}

TestInstaller::~TestInstaller()
{
}

void TestInstaller::unDo()
{
    TestRegistry::getCurrentRegistry()->unDoLastAddTest();
}
