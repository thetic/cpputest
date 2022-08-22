#ifndef INCLUDED_CPPUTEST_TEAMCITYTESTOUTPUT_HPP
#define INCLUDED_CPPUTEST_TEAMCITYTESTOUTPUT_HPP

#include "CppUTest/TestOutput.hpp"

#include <string>

namespace cpputest {

class TeamCityTestOutput : public ConsoleTestOutput {
public:
    TeamCityTestOutput(void);
    ~TeamCityTestOutput(void) override;

    void printCurrentTestStarted(const UtestShell& test) override;
    void printCurrentTestEnded(const TestResult& res) override;
    void printCurrentGroupStarted(const UtestShell& test) override;
    void printCurrentGroupEnded(const TestResult& res) override;

    void printFailure(const TestFailure& failure) override;

protected:
private:
    void printEscaped(const char* s);
    const UtestShell* currtest_;
    std::string currGroup_;
};

}

#endif // INCLUDED_CPPUTEST_TEAMCITYTESTOUTPUT_HPP
