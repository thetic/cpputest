#include "CppUTest/TeamCityTestOutput.hpp"

#include "CppUTest/PlatformSpecificFunctions.h"
#include "CppUTest/PlatformSpecificFunctions.hpp"
#include "CppUTest/TestHarness.hpp"

class TeamCityOutputToBuffer : public TeamCityTestOutput {
public:
    explicit TeamCityOutputToBuffer()
    {
    }

    ~TeamCityOutputToBuffer() override
    {
    }

    void printBuffer(const char* s) override
    {
        output += s;
    }

    void flush() override
    {
        output = "";
    }

    const std::string& getOutput()
    {
        return output;
    }

private:
    std::string output;
};

static long millisTime;

static long MockGetPlatformSpecificTimeInMillis()
{
    return millisTime;
}

TEST_GROUP(TeamCityOutputTest)
{
    TeamCityTestOutput* tcout;
    TeamCityOutputToBuffer* mock;
    UtestShell* tst;
    TestFailure *f, *f2, *f3;
    TestResult* result;

    void setup() override
    {
        mock = new TeamCityOutputToBuffer();
        tcout = mock;
        tst = new UtestShell("group", "test", "file", 10);
        f = new TestFailure(tst, "failfile", 20, "failure message");
        f2 = new TestFailure(tst, "file", 20, "message");
        f3 = new TestFailure(tst, "file", 30, "apos' pipe| [brackets]\r\nCRLF");
        result = new TestResult(*mock);
        result->setTotalExecutionTime(10);
        millisTime = 0;
        UT_PTR_SET(TestOutput::time_in_millis, MockGetPlatformSpecificTimeInMillis);
    }
    void teardown() override
    {
        delete tcout;
        delete tst;
        delete f;
        delete f2;
        delete f3;
        delete result;
    }
};

TEST(TeamCityOutputTest, PrintGroupStarted)
{
    result->currentGroupStarted(tst);
    STRCMP_EQUAL("##teamcity[testSuiteStarted name='group']\n", mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, PrintGroupStartedAndEnded)
{
    const char* expected = "##teamcity[testSuiteStarted name='group']\n"
                           "##teamcity[testSuiteFinished name='group']\n";
    result->currentGroupStarted(tst);
    result->currentGroupEnded(tst);
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, PrintGroupEndedButNotStarted)
{
    result->currentGroupEnded(tst);
    STRCMP_EQUAL("", mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, PrintTestStarted)
{
    tcout->printCurrentTestStarted(*tst);
    STRCMP_EQUAL("##teamcity[testStarted name='test']\n", mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, PrintTestStartedAndEnded)
{
    result->currentTestStarted(tst);
    millisTime = 42;
    result->currentTestEnded(tst);
    STRCMP_EQUAL("##teamcity[testStarted name='test']\n##teamcity[testFinished name='test' duration='42']\n",
        mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, PrintTestEndedButNotStarted)
{
    result->currentTestEnded(tst);
    STRCMP_EQUAL("", mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, PrintTestIgnored)
{
    const char* expected = "##teamcity[testStarted name='test']\n"
                           "##teamcity[testIgnored name='test']\n"
                           "##teamcity[testFinished name='test' duration='41']\n";

    IgnoredUtestShell* itst = new IgnoredUtestShell("group", "test", "file", 10);
    result->currentTestStarted(itst);
    millisTime = 41;
    result->currentTestEnded(itst);
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
    delete itst;
}

TEST(TeamCityOutputTest, PrintWithFailureInSameFile)
{
    tcout->printFailure(*f2);
    const char* expected = "##teamcity[testFailed name='test' message='file:20' "
                           "details='message']\n";
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, PrintWithEscapedCharacters)
{
    tcout->printFailure(*f3);
    const char* expected = "##teamcity[testFailed name='test' message='file:30' "
                           "details='apos|' pipe|| |[brackets|]"
                           "|r|nCRLF']\n";
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, PrintFailureWithFailInDifferentFile)
{
    tcout->printFailure(*f);
    const char* expected = "##teamcity[testFailed name='test' message='TEST failed (file:10): failfile:20' "
                           "details='failure message']\n";
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, TestGroupEscaped_Start)
{
    tst->setGroupName("'[]\n\r");
    result->currentGroupStarted(tst);
    const char* expected = "##teamcity[testSuiteStarted name='|'|[|]|n|r']\n";
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, TestGroupEscaped_End)
{
    tst->setGroupName("'[]\n\r");
    result->currentGroupStarted(tst);
    result->currentGroupEnded(tst);
    const char* expected = "##teamcity[testSuiteStarted name='|'|[|]|n|r']\n"
                           "##teamcity[testSuiteFinished name='|'|[|]|n|r']\n";
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, TestNameEscaped_Start)
{
    tst->setTestName("'[]\n\r");
    result->currentTestStarted(tst);
    const char* expected = "##teamcity[testStarted name='|'|[|]|n|r']\n";
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, TestNameEscaped_End)
{
    tst->setTestName("'[]\n\r");
    result->currentTestStarted(tst);
    result->currentTestEnded(tst);
    const char* expected = "##teamcity[testStarted name='|'|[|]|n|r']\n"
                           "##teamcity[testFinished name='|'|[|]|n|r' duration='0']\n";
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, TestNameEscaped_Ignore)
{
    IgnoredUtestShell itst("group", "'[]\n\r", "file", 10);
    result->currentTestStarted(&itst);
    const char* expected = "##teamcity[testStarted name='|'|[|]|n|r']\n"
                           "##teamcity[testIgnored name='|'|[|]|n|r']\n";
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
}

TEST(TeamCityOutputTest, TestNameEscaped_Fail)
{
    tst->setTestName("'[]\n\r");
    TestFailure fail(tst, "failfile", 20, "failure message");
    tcout->printFailure(fail);
    const char* expected = "##teamcity[testFailed name='|'|[|]|n|r' message='TEST failed (file:10): failfile:20' "
                           "details='failure message']\n";
    STRCMP_EQUAL(expected, mock->getOutput().c_str());
}

/* Todo:
 * -Detect when running in TeamCity and switch output to -o teamcity automatically
 */
