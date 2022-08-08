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

#ifndef D_JUnitTestOutput_h
#define D_JUnitTestOutput_h

#include "CppUTest/TestOutput.hpp"
#include "CppUTest/SimpleString.hpp"

struct JUnitTestOutputImpl;
struct JUnitTestCaseResultNode;

class JUnitTestOutput: public TestOutput
{
public:
    JUnitTestOutput();
    ~JUnitTestOutput() override;

    void printTestsStarted() override;
    void printTestsEnded(const TestResult& result) override;
    void printCurrentTestStarted(const UtestShell& test) override;
    void printCurrentTestEnded(const TestResult& res) override;
    void printCurrentGroupStarted(const UtestShell& test) override;
    void printCurrentGroupEnded(const TestResult& res) override;

    void printBuffer(const char*) override;
    void print(const char*) override;
    void print(long) override;
    void print(size_t) override;
    void printFailure(const TestFailure& failure) override;

    void flush() override;

    virtual SimpleString createFileName(const SimpleString& group);
    void setPackageName(const SimpleString &package);

protected:

    JUnitTestOutputImpl* impl_;
    void resetTestGroupResult();

    virtual void openFileForWrite(const SimpleString& fileName);
    virtual void writeTestGroupToFile();
    virtual void writeToFile(const SimpleString& buffer);
    virtual void closeFile();

    virtual void writeXmlHeader();
    virtual void writeTestSuiteSummary();
    virtual void writeProperties();
    virtual void writeTestCases();
    virtual SimpleString encodeXmlText(const SimpleString& textbody);
    virtual SimpleString encodeFileName(const SimpleString& fileName);
    virtual void writeFailure(JUnitTestCaseResultNode* node);
    virtual void writeFileEnding();
};

#endif
