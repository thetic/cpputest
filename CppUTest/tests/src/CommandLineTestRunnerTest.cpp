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
#include "CppUTest/JUnitTestOutput.hpp"
#include "CppUTest/StringCollection.hpp"
#include "CppUTest/TestHarness.hpp"
#include "CppUTest/TestPlugin.hpp"
#include "CppUTest/TestRegistry.hpp"
#include "CppUTest/TestTestingFixture.hpp"

class DummyPluginWhichCountsThePlugins : public cpputest::TestPlugin {
public:
    bool returnValue;
    int amountOfPlugins;

    DummyPluginWhichCountsThePlugins(const std::string& name, cpputest::TestRegistry* registry)
        : TestPlugin(name)
        , returnValue(true)
        , amountOfPlugins(0)
        , registry_(registry)
    {
    }

    virtual bool parseArguments(int, const char* const*, int) override
    {
        /* Remove ourselves from the count */
        amountOfPlugins = registry_->countPlugins() - 1;
        return returnValue;
    }

private:
    cpputest::TestRegistry* registry_;
};

class CommandLineTestRunnerWithStringBufferOutput : public cpputest::CommandLineTestRunner {
public:
    cpputest::StringBufferTestOutput* fakeJUnitOutputWhichIsReallyABuffer_;
    cpputest::StringBufferTestOutput* fakeConsoleOutputWhichIsReallyABuffer;
    cpputest::StringBufferTestOutput* fakeTCOutputWhichIsReallyABuffer;

    CommandLineTestRunnerWithStringBufferOutput(int argc, const char* const* argv, cpputest::TestRegistry* registry)
        : CommandLineTestRunner(argc, argv, registry)
        , fakeJUnitOutputWhichIsReallyABuffer_(nullptr)
        , fakeConsoleOutputWhichIsReallyABuffer(nullptr)
        , fakeTCOutputWhichIsReallyABuffer(nullptr)
    {
    }

    cpputest::TestOutput* createConsoleOutput() override
    {
        fakeConsoleOutputWhichIsReallyABuffer = new cpputest::StringBufferTestOutput;
        return fakeConsoleOutputWhichIsReallyABuffer;
    }

    cpputest::TestOutput* createJUnitOutput(const std::string&) override
    {
        fakeJUnitOutputWhichIsReallyABuffer_ = new cpputest::StringBufferTestOutput;
        return fakeJUnitOutputWhichIsReallyABuffer_;
    }

    cpputest::TestOutput* createTeamCityOutput() override
    {
        fakeTCOutputWhichIsReallyABuffer = new cpputest::StringBufferTestOutput;
        return fakeTCOutputWhichIsReallyABuffer;
    }
};

static std::string file;
static std::string console;

TEST_GROUP(CommandLineTestRunner)
{
    cpputest::TestRegistry registry;
    cpputest::UtestShell* test1;
    cpputest::UtestShell* test2;
    DummyPluginWhichCountsThePlugins* pluginCountingPlugin;

    void setup() override
    {
        test1 = new cpputest::UtestShell("group1", "test1", "file1", 1);
        test2 = new cpputest::UtestShell("group2", "test2", "file2", 2);
        registry.addTest(test1);
        pluginCountingPlugin = new DummyPluginWhichCountsThePlugins("PluginCountingPlugin", &registry);

        file = "";
        console = "";
        UT_PTR_SET(cpputest::TestOutput::fopen, fopen_fake);
        UT_PTR_SET(cpputest::TestOutput::fputs, fputs_fake);
        UT_PTR_SET(cpputest::TestOutput::fclose, fclose_fake);
        UT_PTR_SET(cpputest::TestOutput::putchar, putchar_fake);
    }
    void teardown() override
    {
        delete pluginCountingPlugin;
        delete test2;
        delete test1;
    }

    std::string runAndGetOutput(const int argc, const char* argv[])
    {
        CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(argc, argv, &registry);
        commandLineTestRunner.runAllTestsMain();
        return commandLineTestRunner.fakeConsoleOutputWhichIsReallyABuffer->getOutput();
    }

    static std::FILE* fopen_fake(const char*, const char*)
    {
        return nullptr;
    }

    static int fputs_fake(const char* str, std::FILE*)
    {
        file += str;
        return 0;
    }

    static int fclose_fake(std::FILE*)
    {
        return 0;
    }

    static int putchar_fake(int c)
    {
        console += StringFrom((char)c);
        return c;
    }
};

TEST(CommandLineTestRunner, OnePluginGetsInstalledDuringTheRunningTheTests)
{
    const char* argv[] = { "tests.exe", "-psomething" };

    registry.installPlugin(pluginCountingPlugin);

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    commandLineTestRunner.runAllTestsMain();
    registry.removePluginByName("PluginCountingPlugin");

    LONGS_EQUAL(0, registry.countPlugins());
    LONGS_EQUAL(1, pluginCountingPlugin->amountOfPlugins);
}

TEST(CommandLineTestRunner, NoPluginsAreInstalledAtTheEndOfARunWhenTheArgumentsAreInvalid)
{
    const char* argv[] = { "tests.exe", "-fdskjnfkds" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    commandLineTestRunner.runAllTestsMain();

    LONGS_EQUAL(0, registry.countPlugins());
}

TEST(CommandLineTestRunner, ReturnsOneWhenTheArgumentsAreInvalid)
{
    const char* argv[] = { "tests.exe", "-some-invalid=parameter" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    int returned = commandLineTestRunner.runAllTestsMain();

    LONGS_EQUAL(1, returned);
}

TEST(CommandLineTestRunner, ReturnsOnePrintsHelpOnHelp)
{
    const char* argv[] = { "tests.exe", "-h" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    int returned = commandLineTestRunner.runAllTestsMain();

    LONGS_EQUAL(1, returned);
    STRCMP_CONTAINS("Thanks for using CppUTest.", commandLineTestRunner.fakeConsoleOutputWhichIsReallyABuffer->getOutput().c_str());
}

TEST(CommandLineTestRunner, ReturnsZeroWhenNoErrors)
{
    const char* argv[] = { "tests.exe" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(1, argv, &registry);
    int returned = commandLineTestRunner.runAllTestsMain();

    LONGS_EQUAL(0, returned);
}

TEST(CommandLineTestRunner, ReturnsOneWhenNoTestsMatchProvidedFilter)
{
    const char* argv[] = { "tests.exe", "-g", "NoSuchGroup" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(3, argv, &registry);
    int returned = commandLineTestRunner.runAllTestsMain();

    LONGS_EQUAL(1, returned);
}

TEST(CommandLineTestRunner, TeamcityOutputEnabled)
{
    const char* argv[] = { "tests.exe", "-oteamcity" };
    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    commandLineTestRunner.runAllTestsMain();
    CHECK(commandLineTestRunner.fakeTCOutputWhichIsReallyABuffer != nullptr);
}

TEST(CommandLineTestRunner, JunitOutputEnabled)
{
    const char* argv[] = { "tests.exe", "-ojunit" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    commandLineTestRunner.runAllTestsMain();
    CHECK(commandLineTestRunner.fakeJUnitOutputWhichIsReallyABuffer_ != nullptr);
}

TEST(CommandLineTestRunner, JunitOutputAndVerboseEnabled)
{
    const char* argv[] = { "tests.exe", "-ojunit", "-v" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(3, argv, &registry);
    commandLineTestRunner.runAllTestsMain();
    STRCMP_CONTAINS("TEST(group1, test1)", commandLineTestRunner.fakeJUnitOutputWhichIsReallyABuffer_->getOutput().c_str());
    STRCMP_CONTAINS("TEST(group1, test1)", commandLineTestRunner.fakeConsoleOutputWhichIsReallyABuffer->getOutput().c_str());
}

TEST(CommandLineTestRunner, veryVerboseSetOnOutput)
{
    const char* argv[] = { "tests.exe", "-vv" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    commandLineTestRunner.runAllTestsMain();
    STRCMP_CONTAINS("TEST(group1, test1)", commandLineTestRunner.fakeConsoleOutputWhichIsReallyABuffer->getOutput().c_str());
    STRCMP_CONTAINS("destroyTest", commandLineTestRunner.fakeConsoleOutputWhichIsReallyABuffer->getOutput().c_str());
}

TEST(CommandLineTestRunner, defaultTestsAreRunInOrderTheyAreInRepository)
{
    const char* argv[] = { "tests.exe", "-v" };

    registry.addTest(test2);
    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    commandLineTestRunner.runAllTestsMain();

    cpputest::StringCollection stringCollection(
        commandLineTestRunner.fakeConsoleOutputWhichIsReallyABuffer->getOutput(),
        '\n');
    STRCMP_CONTAINS("test2", stringCollection[0].c_str());
    STRCMP_CONTAINS("test1", stringCollection[1].c_str());
}

TEST(CommandLineTestRunner, testsCanBeRunInReverseOrder)
{
    const char* argv[] = { "tests.exe", "-v", "-b" };

    registry.addTest(test2);
    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(3, argv, &registry);
    commandLineTestRunner.runAllTestsMain();

    cpputest::StringCollection stringCollection(
        commandLineTestRunner.fakeConsoleOutputWhichIsReallyABuffer->getOutput(),
        '\n');
    STRCMP_CONTAINS("test1", stringCollection[0].c_str());
    STRCMP_CONTAINS("test2", stringCollection[1].c_str());
}

TEST(CommandLineTestRunner, listTestGroupNamesShouldWorkProperly)
{
    const char* argv[] = { "tests.exe", "-lg" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    commandLineTestRunner.runAllTestsMain();

    STRCMP_CONTAINS("group", commandLineTestRunner.fakeConsoleOutputWhichIsReallyABuffer->getOutput().c_str());
}

TEST(CommandLineTestRunner, listTestGroupAndCaseNamesShouldWorkProperly)
{
    const char* argv[] = { "tests.exe", "-ln" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    commandLineTestRunner.runAllTestsMain();

    STRCMP_CONTAINS("group1.test1", commandLineTestRunner.fakeConsoleOutputWhichIsReallyABuffer->getOutput().c_str());
}

TEST(CommandLineTestRunner, listTestLocationsShouldWorkProperly)
{
    const char* argv[] = { "tests.exe", "-ll" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    commandLineTestRunner.runAllTestsMain();

    STRCMP_CONTAINS("group1.test1", commandLineTestRunner.fakeConsoleOutputWhichIsReallyABuffer->getOutput().c_str());
}

TEST(CommandLineTestRunner, randomShuffleSeedIsPrintedAndRandFuncIsExercised)
{
    // more than 1 item in test list ensures that shuffle algorithm calls rand_()
    cpputest::UtestShell* anotherTest = new cpputest::UtestShell("group", "test2", "file", 1);
    registry.addTest(anotherTest);

    const char* argv[] = { "tests.exe", "-s" };
    std::string text = runAndGetOutput(2, argv);
    STRCMP_CONTAINS("shuffling enabled with seed:", text.c_str());

    delete anotherTest;
}

TEST(CommandLineTestRunner, specificShuffleSeedIsPrintedVerbose)
{
    const char* argv[] = { "tests.exe", "-s2", "-v" };
    std::string text = runAndGetOutput(3, argv);
    STRCMP_CONTAINS("shuffling enabled with seed: 2", text.c_str());
}

TEST(CommandLineTestRunner, realJunitOutputShouldBeCreatedAndWorkProperly)
{
    const char* argv[] = {
        "tests.exe",
        "-ojunit",
        "-v",
        "-kpackage",
    };

    cpputest::CommandLineTestRunner commandLineTestRunner(4, argv, &registry);
    commandLineTestRunner.runAllTestsMain();

    STRCMP_CONTAINS("<testcase classname=\"package.group1\" name=\"test1\"", file.c_str());
    STRCMP_CONTAINS("TEST(group1, test1)", console.c_str());
}

TEST(CommandLineTestRunner, realTeamCityOutputShouldBeCreatedAndWorkProperly)
{
    const char* argv[] = {
        "tests.exe",
        "-oteamcity",
        "-v",
        "-kpackage",
    };

    cpputest::CommandLineTestRunner commandLineTestRunner(4, argv, &registry);
    commandLineTestRunner.runAllTestsMain();

    STRCMP_CONTAINS("##teamcity[testSuiteStarted name='group1'", console.c_str());
    STRCMP_CONTAINS("##teamcity[testStarted name='test1'", console.c_str());
    STRCMP_CONTAINS("##teamcity[testFinished name='test1'", console.c_str());
    STRCMP_CONTAINS("##teamcity[testSuiteFinished name='group1'", console.c_str());
}

class RunIgnoredUtest : public cpputest::Utest {
public:
    static bool Checker;
    void testBody() override
    {
        Checker = true;
    }
};

bool RunIgnoredUtest::Checker = false;

class RunIgnoredUtestShell : public cpputest::IgnoredUtestShell {
public:
    RunIgnoredUtestShell(const char* groupName, const char* testName, const char* fileName, size_t lineNumber)
        : IgnoredUtestShell(groupName, testName, fileName, lineNumber)
    {
    }
    virtual cpputest::Utest* createTest() override { return new RunIgnoredUtest; }
};

TEST_GROUP(RunIgnoredTest)
{
    cpputest::TestRegistry registry;
    RunIgnoredUtestShell* runIgnoredTest;
    DummyPluginWhichCountsThePlugins* pluginCountingPlugin;

    void setup() override
    {
        runIgnoredTest = new RunIgnoredUtestShell("group", "test", "file", 1);
        registry.addTest(runIgnoredTest);
        pluginCountingPlugin = new DummyPluginWhichCountsThePlugins("PluginCountingPlugin", &registry);
    }
    void teardown() override
    {
        delete pluginCountingPlugin;
        delete runIgnoredTest;
        RunIgnoredUtest::Checker = false;
    }
};

TEST(RunIgnoredTest, IgnoreTestWillBeIgnoredIfNoOptionSpecified)
{
    const char* argv[] = { "tests.exe" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(1, argv, &registry);
    commandLineTestRunner.runAllTestsMain();

    CHECK_FALSE(RunIgnoredUtest::Checker);
}

TEST(RunIgnoredTest, IgnoreTestWillGetRunIfOptionSpecified)
{
    const char* argv[] = { "tests.exe", "-ri" };

    CommandLineTestRunnerWithStringBufferOutput commandLineTestRunner(2, argv, &registry);
    commandLineTestRunner.runAllTestsMain();

    CHECK_TRUE(RunIgnoredUtest::Checker);
}
