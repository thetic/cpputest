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

#ifndef INCLUDED_MOCKFAILUREREPORTERFORTEST_HPP
#define INCLUDED_MOCKFAILUREREPORTERFORTEST_HPP

#include "CppUTestExt/MockSupport.hpp"

#define CHECK_EXPECTED_MOCK_FAILURE(expectedFailure) CHECK_EXPECTED_MOCK_FAILURE_LOCATION(expectedFailure, __FILE__, __LINE__)
#define CHECK_NO_MOCK_FAILURE() CHECK_NO_MOCK_FAILURE_LOCATION(__FILE__, __LINE__)

class MockFailureReporterForTest : public cpputest::extensions::MockFailureReporter {
public:
    std::string mockFailureString;

    void failTest(const cpputest::extensions::MockFailure& failure) override;
    static MockFailureReporterForTest* getReporter();
    static void clearReporter();

private:
    static MockFailureReporterForTest* instance_;
};

class MockFailureReporterInstaller {
public:
    MockFailureReporterInstaller();
    ~MockFailureReporterInstaller();
};

cpputest::UtestShell* mockFailureTest();
std::string mockFailureString();
void CLEAR_MOCK_FAILURE();
void CHECK_EXPECTED_MOCK_FAILURE_LOCATION(const cpputest::extensions::MockFailure& expectedFailure, const char* file, size_t line);
void CHECK_NO_MOCK_FAILURE_LOCATION(const char* file, size_t line);

class MockExpectedCallsListForTest : public cpputest::extensions::MockExpectedCallsList {
public:
    ~MockExpectedCallsListForTest() override;
    cpputest::extensions::MockCheckedExpectedCall* addFunction(const std::string& name);
    cpputest::extensions::MockCheckedExpectedCall* addFunction(unsigned int numCalls, const std::string& name);
    cpputest::extensions::MockCheckedExpectedCall* addFunctionOrdered(const std::string& name, unsigned int order);
};

#endif // INCLUDED_MOCKFAILUREREPORTERFORTEST_HPP
