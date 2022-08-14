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

///////////////////////////////////////////////////////////////////////////////
//
// Failure is a class which holds information for a specific
// test failure. It can be overriden for more complex failure messages
//
///////////////////////////////////////////////////////////////////////////////

#ifndef D_TestFailure_H
#define D_TestFailure_H

#include <string>

#if !CPPUTEST_NO_EXCEPTIONS
#include <stdexcept>
#endif

class UtestShell;
class TestOutput;

class TestFailure {
public:
    TestFailure(UtestShell*, const char* fileName, size_t lineNumber, const std::string& theMessage);
    TestFailure(UtestShell*, const std::string& theMessage);
    TestFailure(UtestShell*, const char* fileName, size_t lineNumber);
    TestFailure(const TestFailure&);
    virtual ~TestFailure();
    virtual std::string getFileName() const;
    virtual std::string getTestName() const;
    virtual std::string getTestNameOnly() const;
    virtual size_t getFailureLineNumber() const;
    virtual std::string getMessage() const;
    virtual std::string getTestFileName() const;
    virtual size_t getTestLineNumber() const;
    bool isOutsideTestFile() const;
    bool isInHelperFunction() const;

protected:
    std::string createButWasString(
        const std::string& expected,
        const std::string& actual);
    std::string createDifferenceAtPosString(
        const std::string& actual,
        size_t offset,
        size_t reportedPosition);
    std::string createUserText(const std::string& text);
    std::string testName_;
    std::string testNameOnly_;
    std::string fileName_;
    size_t lineNumber_;
    std::string testFileName_;
    size_t testLineNumber_;
    std::string message_;
    TestFailure& operator=(const TestFailure&);
};

class EqualsFailure : public TestFailure {
public:
    EqualsFailure(
        UtestShell*,
        const char* fileName,
        size_t lineNumber,
        const char* expected,
        const char* actual,
        const std::string& text);
    EqualsFailure(
        UtestShell*,
        const char* fileName,
        size_t lineNumber,
        const std::string& expected,
        const std::string& actual,
        const std::string& text);
};

class DoublesEqualFailure : public TestFailure {
public:
    DoublesEqualFailure(
        UtestShell*,
        const char* fileName,
        size_t lineNumber,
        double expected,
        double actual,
        double threshold,
        const std::string& text);
};

class CheckEqualFailure : public TestFailure {
public:
    CheckEqualFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        const std::string& expected,
        const std::string& actual,
        const std::string& text);
};

class ComparisonFailure : public TestFailure {
public:
    ComparisonFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        const std::string& checkString,
        const std::string& comparisonString,
        const std::string& text);
};

class ContainsFailure : public TestFailure {
public:
    ContainsFailure(
        UtestShell*,
        const char* fileName,
        size_t lineNumber,
        const std::string& expected,
        const std::string& actual,
        const std::string& text);
};

class CheckFailure : public TestFailure {
public:
    CheckFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        const std::string& checkString,
        const std::string& conditionString,
        const std::string& textString = "");
};

class FailFailure : public TestFailure {
public:
    FailFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        const std::string& message);
};

class LongsEqualFailure : public TestFailure {
public:
    LongsEqualFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        long expected,
        long actual,
        const std::string& text);
};

class UnsignedLongsEqualFailure : public TestFailure {
public:
    UnsignedLongsEqualFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        unsigned long expected,
        unsigned long actual,
        const std::string& text);
};

class LongLongsEqualFailure : public TestFailure {
public:
    LongLongsEqualFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        long long expected,
        long long actual,
        const std::string& text);
};

class UnsignedLongLongsEqualFailure : public TestFailure {
public:
    UnsignedLongLongsEqualFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        unsigned long long expected,
        unsigned long long actual,
        const std::string& text);
};

class SignedBytesEqualFailure : public TestFailure {
public:
    SignedBytesEqualFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        signed char expected,
        signed char actual,
        const std::string& text);
};

class StringEqualFailure : public TestFailure {
public:
    StringEqualFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        const char* expected,
        const char* actual,
        const std::string& text);
};

class StringEqualNoCaseFailure : public TestFailure {
public:
    StringEqualNoCaseFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        const char* expected,
        const char* actual,
        const std::string& text);
};

class BinaryEqualFailure : public TestFailure {
public:
    BinaryEqualFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        const unsigned char* expected,
        const unsigned char* actual,
        size_t size,
        const std::string& text);
};

class BitsEqualFailure : public TestFailure {
public:
    BitsEqualFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        unsigned long expected,
        unsigned long actual,
        unsigned long mask,
        size_t byteCount,
        const std::string& text);
};

class FeatureUnsupportedFailure : public TestFailure {
public:
    FeatureUnsupportedFailure(
        UtestShell* test,
        const char* fileName,
        size_t lineNumber,
        const std::string& featureName,
        const std::string& text);
};

#if !CPPUTEST_NO_EXCEPTIONS
class UnexpectedExceptionFailure : public TestFailure {
public:
    UnexpectedExceptionFailure(UtestShell* test);
    UnexpectedExceptionFailure(UtestShell* test, const std::exception& e);
};
#endif

#endif
