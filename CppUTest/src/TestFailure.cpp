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

#include "CppUTest/TestFailure.hpp"
#include "CppUTest/TestHarness.hpp"
#include "CppUTest/TestOutput.hpp"

#include <cmath>
#include <typeinfo>
#if defined(__GNUC__)
#include <cxxabi.h>
#include <memory>
#endif

namespace cpputest {

namespace {
    void padStringsToSameLength(std::string& str1, std::string& str2, char padCharacter)
    {
        if (str1.size() > str2.size()) {
            padStringsToSameLength(str2, str1, padCharacter);
            return;
        }

        str1 = std::string(str2.size() - str1.size(), padCharacter) + str1;
    }
}

TestFailure::TestFailure(UtestShell* test, const char* fileName, size_t lineNumber, const std::string& theMessage)
    : testName_(test->getFormattedName())
    , testNameOnly_(test->getName())
    , fileName_(fileName)
    , lineNumber_(lineNumber)
    , testFileName_(test->getFile())
    , testLineNumber_(test->getLineNumber())
    , message_(theMessage)
{
}

TestFailure::TestFailure(UtestShell* test, const std::string& theMessage)
    : testName_(test->getFormattedName())
    , testNameOnly_(test->getName())
    , fileName_(test->getFile())
    , lineNumber_(test->getLineNumber())
    , testFileName_(test->getFile())
    , testLineNumber_(test->getLineNumber())
    , message_(theMessage)
{
}

TestFailure::TestFailure(UtestShell* test, const char* fileName, size_t lineNum)
    : testName_(test->getFormattedName())
    , testNameOnly_(test->getName())
    , fileName_(fileName)
    , lineNumber_(lineNum)
    , testFileName_(test->getFile())
    , testLineNumber_(test->getLineNumber())
    , message_("no message")
{
}

TestFailure::TestFailure(const TestFailure& f)
    : testName_(f.testName_)
    , testNameOnly_(f.testNameOnly_)
    , fileName_(f.fileName_)
    , lineNumber_(f.lineNumber_)
    , testFileName_(f.testFileName_)
    , testLineNumber_(f.testLineNumber_)
    , message_(f.message_)
{
}

TestFailure::~TestFailure()
{
}

std::string TestFailure::getFileName() const
{
    return fileName_;
}

std::string TestFailure::getTestFileName() const
{
    return testFileName_;
}

std::string TestFailure::getTestName() const
{
    return testName_;
}

std::string TestFailure::getTestNameOnly() const
{
    return testNameOnly_;
}

size_t TestFailure::getFailureLineNumber() const
{
    return lineNumber_;
}

size_t TestFailure::getTestLineNumber() const
{
    return testLineNumber_;
}

std::string TestFailure::getMessage() const
{
    return message_;
}

bool TestFailure::isOutsideTestFile() const
{
    return testFileName_ != fileName_;
}

bool TestFailure::isInHelperFunction() const
{
    return lineNumber_ < testLineNumber_;
}

std::string TestFailure::createButWasString(const std::string& expected, const std::string& actual)
{
    return StringFromFormat("expected <%s>\n\tbut was  <%s>", expected.c_str(), actual.c_str());
}

std::string TestFailure::createDifferenceAtPosString(const std::string& actual, size_t offset, size_t reportedPosition)
{
    std::string result;
    const size_t extraCharactersWindow = 20;
    const size_t halfOfExtraCharactersWindow = extraCharactersWindow / 2;

    std::string paddingForPreventingOutOfBounds(halfOfExtraCharactersWindow, ' ');
    std::string actualString = paddingForPreventingOutOfBounds + actual + paddingForPreventingOutOfBounds;
    std::string differentString = StringFromFormat("difference starts at position %lu at: <", (unsigned long)reportedPosition);

    result += "\n";
    result += StringFromFormat("\t%s%s>\n", differentString.c_str(), actualString.substr(offset, extraCharactersWindow).c_str());

    result += StringFromFormat("\t%s^", std::string((differentString.size() + halfOfExtraCharactersWindow), ' ').c_str());
    return result;
}

std::string TestFailure::createUserText(const char* text)
{
    std::string userMessage = "";
    if (text) {
        std::string text_string = text;
        if (!text_string.empty()) {
            // This is a kludge to turn off "Message: " for this case.
            // I don't think "Message: " adds anything, as you get to see the
            // message. I propose we remove "Message: " lead in
            if (text_string.find("LONGS_EQUAL") != 0)
                userMessage += "Message: ";
            userMessage += text_string;
            userMessage += "\n\t";
        }
    }
    return userMessage;
}

EqualsFailure::EqualsFailure(UtestShell* test, const char* fileName, size_t lineNumber, const char* expected, const char* actual, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    message_ += createButWasString(StringFromOrNull(expected), StringFromOrNull(actual));
}

EqualsFailure::EqualsFailure(UtestShell* test, const char* fileName, size_t lineNumber, const std::string& expected, const std::string& actual, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    message_ += createButWasString(expected, actual);
}

DoublesEqualFailure::DoublesEqualFailure(UtestShell* test, const char* fileName, size_t lineNumber, double expected, double actual, double threshold, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    message_ += createButWasString(StringFrom(expected, 7), StringFrom(actual, 7));
    message_ += " threshold used was <";
    message_ += StringFrom(threshold, 7);
    message_ += ">";

    if (std::isnan(expected) || std::isnan(actual) || std::isnan(threshold))
        message_ += "\n\tCannot make comparisons with Nan";
}

CheckEqualFailure::CheckEqualFailure(UtestShell* test, const char* fileName, size_t lineNumber, const std::string& expected, const std::string& actual, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    std::string printableExpected = PrintableStringFromOrNull(expected.c_str());
    std::string printableActual = PrintableStringFromOrNull(actual.c_str());

    message_ += createButWasString(printableExpected, printableActual);

    size_t failStart;
    for (failStart = 0; actual[failStart] == expected[failStart]; failStart++)
        ;
    size_t failStartPrintable;
    for (failStartPrintable = 0; printableActual[failStartPrintable] == printableExpected[failStartPrintable]; failStartPrintable++)
        ;
    message_ += createDifferenceAtPosString(printableActual, failStartPrintable, failStart);
}

ComparisonFailure::ComparisonFailure(UtestShell* test, const char* fileName, size_t lineNumber, const std::string& checkString, const std::string& comparisonString, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);
    message_ += checkString;
    message_ += "(";
    message_ += comparisonString;
    message_ += ") failed";
}

ContainsFailure::ContainsFailure(
    UtestShell* test,
    const char* fileName,
    size_t lineNumber,
    const char* expected,
    const char* actual,
    const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    message_ += StringFromFormat(
        "actual <%s>\n\tdid not contain  <%s>",
        StringFromOrNull(actual).c_str(),
        StringFromOrNull(expected).c_str());
}

CheckFailure::CheckFailure(UtestShell* test, const char* fileName, size_t lineNumber, const std::string& checkString, const std::string& conditionString, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    message_ += checkString;
    message_ += "(";
    message_ += conditionString;
    message_ += ") failed";
}

FailFailure::FailFailure(UtestShell* test, const char* fileName, size_t lineNumber, const std::string& message)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = message;
}

LongsEqualFailure::LongsEqualFailure(UtestShell* test, const char* fileName, size_t lineNumber, long expected, long actual, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    std::string aDecimal = StringFrom(actual);
    std::string eDecimal = StringFrom(expected);

    padStringsToSameLength(aDecimal, eDecimal, ' ');

    std::string actualReported = aDecimal + " " + std::string(BracketsFormattedHexStringFrom(actual));
    std::string expectedReported = eDecimal + " " + std::string(BracketsFormattedHexStringFrom(expected));
    message_ += createButWasString(expectedReported, actualReported);
}

UnsignedLongsEqualFailure::UnsignedLongsEqualFailure(UtestShell* test, const char* fileName, size_t lineNumber, unsigned long expected, unsigned long actual, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    std::string aDecimal = StringFrom(actual);
    std::string eDecimal = StringFrom(expected);

    padStringsToSameLength(aDecimal, eDecimal, ' ');

    std::string actualReported = aDecimal + " " + std::string(BracketsFormattedHexStringFrom(actual));
    std::string expectedReported = eDecimal + " " + std::string(BracketsFormattedHexStringFrom(expected));

    message_ += createButWasString(expectedReported, actualReported);
}

LongLongsEqualFailure::LongLongsEqualFailure(UtestShell* test, const char* fileName, size_t lineNumber, long long expected, long long actual, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    std::string aDecimal = StringFrom(actual);
    std::string eDecimal = StringFrom(expected);

    padStringsToSameLength(aDecimal, eDecimal, ' ');

    std::string actualReported = aDecimal + " " + std::string(BracketsFormattedHexStringFrom(actual));
    std::string expectedReported = eDecimal + " " + std::string(BracketsFormattedHexStringFrom(expected));
    message_ += createButWasString(expectedReported, actualReported);
}

UnsignedLongLongsEqualFailure::UnsignedLongLongsEqualFailure(UtestShell* test, const char* fileName, size_t lineNumber, unsigned long long expected, unsigned long long actual, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    std::string aDecimal = StringFrom(actual);
    std::string eDecimal = StringFrom(expected);

    padStringsToSameLength(aDecimal, eDecimal, ' ');

    std::string actualReported = aDecimal + " " + std::string(BracketsFormattedHexStringFrom(actual));
    std::string expectedReported = eDecimal + " " + std::string(BracketsFormattedHexStringFrom(expected));
    message_ += createButWasString(expectedReported, actualReported);
}

SignedBytesEqualFailure::SignedBytesEqualFailure(UtestShell* test, const char* fileName, size_t lineNumber, signed char expected, signed char actual, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    std::string aDecimal = StringFrom((int)actual);
    std::string eDecimal = StringFrom((int)expected);

    padStringsToSameLength(aDecimal, eDecimal, ' ');

    std::string actualReported = aDecimal + " " + std::string(BracketsFormattedHexStringFrom(actual));
    std::string expectedReported = eDecimal + " " + std::string(BracketsFormattedHexStringFrom(expected));
    message_ += createButWasString(expectedReported, actualReported);
}

StringEqualFailure::StringEqualFailure(UtestShell* test, const char* fileName, size_t lineNumber, const char* expected, const char* actual, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    std::string printableExpected = PrintableStringFromOrNull(expected);
    std::string printableActual = PrintableStringFromOrNull(actual);

    message_ += createButWasString(printableExpected, printableActual);
    if ((expected) && (actual)) {
        size_t failStart;
        for (failStart = 0; actual[failStart] == expected[failStart]; failStart++)
            ;
        size_t failStartPrintable;
        for (failStartPrintable = 0; printableActual[failStartPrintable] == printableExpected[failStartPrintable]; failStartPrintable++)
            ;
        message_ += createDifferenceAtPosString(printableActual, failStartPrintable, failStart);
    }
}

StringEqualNoCaseFailure::StringEqualNoCaseFailure(
    UtestShell* test,
    const char* fileName,
    size_t lineNumber,
    const char* expected,
    const char* actual,
    const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    std::string printableExpected = PrintableStringFromOrNull(expected);
    std::string printableActual = PrintableStringFromOrNull(actual);

    message_ += createButWasString(printableExpected, printableActual);
    if ((expected) && (actual)) {
        size_t failStart;
        for (failStart = 0; std::tolower(actual[failStart]) == std::tolower(expected[failStart]); failStart++)
            ;
        size_t failStartPrintable;
        for (failStartPrintable = 0;
             std::tolower(printableActual[failStartPrintable]) == std::tolower(printableExpected[failStartPrintable]);
             failStartPrintable++)
            ;
        message_ += createDifferenceAtPosString(printableActual, failStartPrintable, failStart);
    }
}

BinaryEqualFailure::BinaryEqualFailure(UtestShell* test, const char* fileName, size_t lineNumber, const unsigned char* expected,
    const unsigned char* actual, size_t size, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    std::string actualHex = StringFromBinaryOrNull(actual, size);

    message_ += createButWasString(StringFromBinaryOrNull(expected, size), actualHex);
    if ((expected) && (actual)) {
        size_t failStart;
        for (failStart = 0; actual[failStart] == expected[failStart]; failStart++)
            ;
        message_ += createDifferenceAtPosString(actualHex, (failStart * 3 + 1), failStart);
    }
}

BitsEqualFailure::BitsEqualFailure(UtestShell* test, const char* fileName, size_t lineNumber, unsigned long expected, unsigned long actual,
    unsigned long mask, size_t byteCount, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    message_ += createButWasString(StringFromMaskedBits(expected, mask, byteCount), StringFromMaskedBits(actual, mask, byteCount));
}

FeatureUnsupportedFailure::FeatureUnsupportedFailure(UtestShell* test, const char* fileName, size_t lineNumber,
    const std::string& featureName, const char* text)
    : TestFailure(test, fileName, lineNumber)
{
    message_ = createUserText(text);

    message_ += StringFromFormat("The feature \"%s\" is not supported in this environment or with the feature set selected when building the library.", featureName.c_str());
}

#if !CPPUTEST_NO_EXCEPTIONS
UnexpectedExceptionFailure::UnexpectedExceptionFailure(UtestShell* test)
    : TestFailure(test, "Unexpected exception of unknown type was thrown.")
{
}

static std::string getExceptionTypeName(const std::exception& e)
{
    const char* name = typeid(e).name();
#if defined(__GNUC__)
    int status = -1;

    std::unique_ptr< char, void (*)(void*) > demangledName(
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        std::free);

    return (status == 0) ? demangledName.get() : name;
#else
    return name;
#endif
}

UnexpectedExceptionFailure::UnexpectedExceptionFailure(UtestShell* test, const std::exception& e)
    : TestFailure(test, StringFromFormat("Unexpected exception of type '%s' was thrown: %s", getExceptionTypeName(e).c_str(), e.what()))
{
}
#endif

}
