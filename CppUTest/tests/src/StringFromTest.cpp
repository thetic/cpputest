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

#include "CppUTest/StringFrom.hpp"

#include "CppUTest/TestHarness.hpp"
#include "CppUTest/TestTestingFixture.hpp"

#include <cmath>
#include <cstdarg>

#if defined(__LP64__) || defined(_LP64) || (defined(__WORDSIZE) && (__WORDSIZE == 64)) || defined(__x86_64) || defined(_WIN64)
#define CPPUTEST_64BIT
#if defined(_WIN64)
#define CPPUTEST_64BIT_32BIT_LONGS
#endif
#endif

TEST_GROUP(StringFrom) {};

TEST(StringFrom, NULLReportsNullString)
{
    STRCMP_EQUAL("(null)", StringFromOrNull((char*)nullptr).c_str());
}

TEST(StringFrom, NULLReportsNullStringPrintable)
{
    STRCMP_EQUAL("(null)", PrintableStringFromOrNull((char*)nullptr).c_str());
}

TEST(StringFrom, Booleans)
{
    std::string s1(StringFrom(true));
    std::string s2(StringFrom(false));
    CHECK(s1 == "true");
    CHECK(s2 == "false");
}

TEST(StringFrom, Pointers)
{
    std::string s(StringFrom((void*)0x1234));
    STRCMP_EQUAL("0x1234", s.c_str());
}

TEST(StringFrom, FunctionPointers)
{
    std::string s(StringFrom((void (*)())0x1234));
    STRCMP_EQUAL("0x1234", s.c_str());
}

TEST(StringFrom, Characters)
{
    std::string s(StringFrom('a'));
    STRCMP_EQUAL("a", s.c_str());
}

TEST(StringFrom, NegativeSignedBytes)
{
    STRCMP_EQUAL("-15", StringFrom((signed char)-15).c_str());
}

TEST(StringFrom, PositiveSignedBytes)
{
    STRCMP_EQUAL("4", StringFrom(4).c_str());
}

TEST(StringFrom, LongInts)
{
    std::string s(StringFrom((long)1));
    CHECK(s == "1");
}

TEST(StringFrom, UnsignedLongInts)
{
    std::string s(StringFrom((unsigned long)1));
    std::string s2(StringFrom((unsigned long)1));
    CHECK(s == s2);
}

TEST(StringFrom, LongLongInts)
{
    std::string s(StringFrom((long long)1));
    CHECK(s == "1");
}

TEST(StringFrom, UnsignedLongLongInts)
{
    std::string s(StringFrom((unsigned long long)1));
    std::string s2(StringFrom((unsigned long long)1));
    CHECK(s == s2);
}

TEST(StringFrom, Doubles)
{
    std::string s(StringFrom(1.2));
    STRCMP_EQUAL("1.2", s.c_str());
}

TEST(StringFrom, NaN)
{
    std::string s(StringFrom(NAN));
    STRCMP_EQUAL("Nan - Not a number", s.c_str());
}

TEST(StringFrom, Inf)
{
    std::string s(StringFrom(INFINITY));
    STRCMP_EQUAL("Inf - Infinity", s.c_str());
}

TEST(StringFrom, SmallDoubles)
{
    std::string s(StringFrom(1.2e-10));
    STRCMP_CONTAINS("1.2e", s.c_str());
}

TEST(StringFrom, Sizes)
{
    size_t size = 10;
    STRCMP_EQUAL("10", StringFrom((int)size).c_str());
}

TEST(StringFrom, nullptr_type)
{
    std::string s(StringFrom(nullptr));
    STRCMP_EQUAL("(null)", s.c_str());
}

TEST(StringFrom, HexStrings)
{
    STRCMP_EQUAL("f3", HexStringFrom((signed char)-13).c_str());

    std::string h1 = HexStringFrom(0xffffL);
    STRCMP_EQUAL("ffff", h1.c_str());

    std::string h15 = HexStringFrom(0xffffLL);
    STRCMP_EQUAL("ffff", h15.c_str());

    std::string h2 = HexStringFrom((void*)0xfffeL);
    STRCMP_EQUAL("fffe", h2.c_str());

    std::string h3 = HexStringFrom((void (*)())0xfffdL);
    STRCMP_EQUAL("fffd", h3.c_str());
}

TEST(StringFrom, StringFromFormat)
{
    std::string h1 = StringFromFormat("%s %s! %d", "Hello", "World", 2009);
    STRCMP_EQUAL("Hello World! 2009", h1.c_str());
}

TEST(StringFrom, StringFromFormatpointer)
{
    // this is not a great test. but %p is odd on mingw and even more odd on Solaris.
    std::string h1 = StringFromFormat("%p", (void*)1);
    if (h1.size() == 3)
        STRCMP_EQUAL("0x1", h1.c_str());
    else if (h1.size() == 8)
        STRCMP_EQUAL("00000001", h1.c_str());
    else if (h1.size() == 9)
        STRCMP_EQUAL("0000:0001", h1.c_str());
    else if (h1.size() == 16)
        STRCMP_EQUAL("0000000000000001", h1.c_str());
    else if (h1.size() == 1)
        STRCMP_EQUAL("1", h1.c_str());
    else
        FAIL("Off %p behavior");
}

TEST(StringFrom, StringFromCString)
{
    STRCMP_EQUAL("bla", StringFrom(std::string("bla")).c_str());
}

namespace {
int WrappedUpVSNPrintf(char* buf, size_t n, const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);

    int result = std::vsnprintf(buf, n, format, arguments);
    va_end(arguments);
    return result;
}
}

TEST(StringFrom, PlatformSpecificSprintf_fits)
{
    char buf[10];

    int count = WrappedUpVSNPrintf(buf, sizeof(buf), "%s", "12345");
    STRCMP_EQUAL("12345", buf);
    LONGS_EQUAL(5, count);
}

TEST(StringFrom, PlatformSpecificSprintf_doesNotFit)
{
    char buf[10];

    int count = WrappedUpVSNPrintf(buf, sizeof(buf), "%s", "12345678901");
    STRCMP_EQUAL("123456789", buf);
    LONGS_EQUAL(11, count);
}

#ifdef CPPUTEST_64BIT

TEST(StringFrom, 64BitAddressPrintsCorrectly)
{
    char* p = (char*)0x0012345678901234;
    std::string expected("0x12345678901234");
    std::string actual = StringFrom((void*)p);
    STRCMP_EQUAL(expected.c_str(), actual.c_str());
}

#ifndef CPPUTEST_64BIT_32BIT_LONGS

TEST(StringFrom, BracketsFormattedHexStringFromForLongOnDifferentPlatform)
{
    long value = -1;

    STRCMP_EQUAL("(0xffffffffffffffff)", BracketsFormattedHexStringFrom(value).c_str());
}

#else

TEST(StringFrom, BracketsFormattedHexStringFromForLongOnDifferentPlatform)
{
    long value = -1;

    STRCMP_EQUAL("(0xffffffff)", BracketsFormattedHexStringFrom(value).c_str());
}

#endif
#else
/*
 * This test case cannot pass on 32 bit systems.
 */
IGNORE_TEST(StringFrom, 64BitAddressPrintsCorrectly)
{
}

TEST(StringFrom, BracketsFormattedHexStringFromForLongOnDifferentPlatform)
{
    long value = -1;

    STRCMP_EQUAL("(0xffffffff)", BracketsFormattedHexStringFrom(value).c_str());
}
#endif

TEST(StringFrom, BuildStringFromUnsignedLongInteger)
{
    unsigned long int i = 0xffffffff;

    std::string result = StringFrom(i);
    const char* expected_string = "4294967295";
    CHECK_EQUAL(expected_string, result);
}

TEST(StringFrom, BuildStringFromUnsignedInteger)
{
    unsigned int i = 0xff;

    std::string result = StringFrom(i);
    const char* expected_string = "255";
    CHECK_EQUAL(expected_string, result);
}

TEST(StringFrom, fromStdString)
{
    std::string s("hello");
    std::string s1(StringFrom(s));

    STRCMP_EQUAL("hello", s1.c_str());
}

TEST(StringFrom, CHECK_EQUAL_unsigned_long)
{
    unsigned long i = 0xffffffffUL;
    CHECK_EQUAL(i, i);
}

TEST(StringFrom, unsigned_long)
{
    unsigned long i = 0xffffffffUL;

    std::string result = StringFrom(i);
    const char* expected_string = "4294967295";
    CHECK_EQUAL(expected_string, result);
}

TEST(StringFrom, Binary)
{
    const unsigned char value[] = { 0x00, 0x01, 0x2A, 0xFF };
    const char expectedString[] = "00 01 2A FF";

    STRCMP_EQUAL(expectedString, StringFromBinary(value, sizeof(value)).c_str());
    STRCMP_EQUAL(expectedString, StringFromBinaryOrNull(value, sizeof(value)).c_str());
    STRCMP_EQUAL("", StringFromBinary(value, 0).c_str());
    STRCMP_EQUAL("(null)", StringFromBinaryOrNull(nullptr, 0).c_str());
}

TEST(StringFrom, BinaryWithSize)
{
    const unsigned char value[] = { 0x12, 0xFE, 0xA1 };
    const char expectedString[] = "Size = 3 | HexContents = 12 FE A1";

    STRCMP_EQUAL(expectedString, StringFromBinaryWithSize(value, sizeof(value)).c_str());
    STRCMP_EQUAL(expectedString, StringFromBinaryWithSizeOrNull(value, sizeof(value)).c_str());
    STRCMP_EQUAL("Size = 0 | HexContents = ", StringFromBinaryWithSize(value, 0).c_str());
    STRCMP_EQUAL("(null)", StringFromBinaryWithSizeOrNull(nullptr, 0).c_str());
}

TEST(StringFrom, BinaryWithSizeLargerThan128)
{
    unsigned char value[129];
    value[127] = 0x00;
    value[128] = 0xff;

    STRCMP_CONTAINS("00 ...", StringFromBinaryWithSize(value, sizeof(value)).c_str());
}

#if (CHAR_BIT == 16)
TEST(StringFrom, MaskedBitsChar)
{
    STRCMP_EQUAL("xxxxxxxx xxxxxxxx", StringFromMaskedBits(0x00, 0x00, 1).c_str());
    STRCMP_EQUAL("xxxxxxxx 00000000", StringFromMaskedBits(0x00, 0xFF, 1).c_str());
    STRCMP_EQUAL("xxxxxxxx 11111111", StringFromMaskedBits(0xFF, 0xFF, 1).c_str());
    STRCMP_EQUAL("xxxxxxxx 1xxxxxxx", StringFromMaskedBits(0x80, 0x80, 1).c_str());
    STRCMP_EQUAL("xxxxxxxx xxxxxxx1", StringFromMaskedBits(0x01, 0x01, 1).c_str());
    STRCMP_EQUAL("xxxxxxxx 11xx11xx", StringFromMaskedBits(0xFF, 0xCC, 1).c_str());
}
#else
TEST(StringFrom, MaskedBitsChar)
{
    STRCMP_EQUAL("xxxxxxxx", StringFromMaskedBits(0x00, 0x00, 1).c_str());
    STRCMP_EQUAL("00000000", StringFromMaskedBits(0x00, 0xFF, 1).c_str());
    STRCMP_EQUAL("11111111", StringFromMaskedBits(0xFF, 0xFF, 1).c_str());
    STRCMP_EQUAL("1xxxxxxx", StringFromMaskedBits(0x80, 0x80, 1).c_str());
    STRCMP_EQUAL("xxxxxxx1", StringFromMaskedBits(0x01, 0x01, 1).c_str());
    STRCMP_EQUAL("11xx11xx", StringFromMaskedBits(0xFF, 0xCC, 1).c_str());
}
#endif

TEST(StringFrom, MaskedBits16Bit)
{
    STRCMP_EQUAL("xxxxxxxx xxxxxxxx", StringFromMaskedBits(0x0000, 0x0000, 2 * 8 / CHAR_BIT).c_str());
    STRCMP_EQUAL("00000000 00000000", StringFromMaskedBits(0x0000, 0xFFFF, 2 * 8 / CHAR_BIT).c_str());
    STRCMP_EQUAL("11111111 11111111", StringFromMaskedBits(0xFFFF, 0xFFFF, 2 * 8 / CHAR_BIT).c_str());
    STRCMP_EQUAL("1xxxxxxx xxxxxxxx", StringFromMaskedBits(0x8000, 0x8000, 2 * 8 / CHAR_BIT).c_str());
    STRCMP_EQUAL("xxxxxxxx xxxxxxx1", StringFromMaskedBits(0x0001, 0x0001, 2 * 8 / CHAR_BIT).c_str());
    STRCMP_EQUAL("11xx11xx 11xx11xx", StringFromMaskedBits(0xFFFF, 0xCCCC, 2 * 8 / CHAR_BIT).c_str());
}

TEST(StringFrom, MaskedBits32Bit)
{
    STRCMP_EQUAL("xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx", StringFromMaskedBits(0x00000000, 0x00000000, 4 * 8 / CHAR_BIT).c_str());
    STRCMP_EQUAL("00000000 00000000 00000000 00000000", StringFromMaskedBits(0x00000000, 0xFFFFFFFF, 4 * 8 / CHAR_BIT).c_str());
    STRCMP_EQUAL("11111111 11111111 11111111 11111111", StringFromMaskedBits(0xFFFFFFFF, 0xFFFFFFFF, 4 * 8 / CHAR_BIT).c_str());
    STRCMP_EQUAL("1xxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx", StringFromMaskedBits(0x80000000, 0x80000000, 4 * 8 / CHAR_BIT).c_str());
    STRCMP_EQUAL("xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxx1", StringFromMaskedBits(0x00000001, 0x00000001, 4 * 8 / CHAR_BIT).c_str());
    STRCMP_EQUAL("11xx11xx 11xx11xx 11xx11xx 11xx11xx", StringFromMaskedBits(0xFFFFFFFF, 0xCCCCCCCC, 4 * 8 / CHAR_BIT).c_str());
}

TEST(StringFrom, StringFromOrdinalNumberOnes)
{
    STRCMP_EQUAL("2nd", StringFromOrdinalNumber(2).c_str());
    STRCMP_EQUAL("3rd", StringFromOrdinalNumber(3).c_str());
    STRCMP_EQUAL("4th", StringFromOrdinalNumber(4).c_str());
    STRCMP_EQUAL("5th", StringFromOrdinalNumber(5).c_str());
    STRCMP_EQUAL("6th", StringFromOrdinalNumber(6).c_str());
    STRCMP_EQUAL("7th", StringFromOrdinalNumber(7).c_str());
}

TEST(StringFrom, StringFromOrdinalNumberTens)
{
    STRCMP_EQUAL("10th", StringFromOrdinalNumber(10).c_str());
    STRCMP_EQUAL("11th", StringFromOrdinalNumber(11).c_str());
    STRCMP_EQUAL("12th", StringFromOrdinalNumber(12).c_str());
    STRCMP_EQUAL("13th", StringFromOrdinalNumber(13).c_str());
    STRCMP_EQUAL("14th", StringFromOrdinalNumber(14).c_str());
    STRCMP_EQUAL("18th", StringFromOrdinalNumber(18).c_str());
}

TEST(StringFrom, StringFromOrdinalNumberOthers)
{
    STRCMP_EQUAL("21st", StringFromOrdinalNumber(21).c_str());
    STRCMP_EQUAL("22nd", StringFromOrdinalNumber(22).c_str());
    STRCMP_EQUAL("23rd", StringFromOrdinalNumber(23).c_str());
    STRCMP_EQUAL("24th", StringFromOrdinalNumber(24).c_str());
    STRCMP_EQUAL("32nd", StringFromOrdinalNumber(32).c_str());
    STRCMP_EQUAL("100th", StringFromOrdinalNumber(100).c_str());
    STRCMP_EQUAL("101st", StringFromOrdinalNumber(101).c_str());
}

TEST(StringFrom, BracketsFormattedHexStringFromForSignedChar)
{
    signed char value = 'c';

    STRCMP_EQUAL("(0x63)", BracketsFormattedHexStringFrom(value).c_str());
}

TEST(StringFrom, BracketsFormattedHexStringFromForUnsignedInt)
{
    unsigned int value = 1;

    STRCMP_EQUAL("(0x1)", BracketsFormattedHexStringFrom(value).c_str());
}

TEST(StringFrom, BracketsFormattedHexStringFromForUnsignedLong)
{
    unsigned long value = 1;

    STRCMP_EQUAL("(0x1)", BracketsFormattedHexStringFrom(value).c_str());
}

/* Handling of systems with a 16-bit int-width. */
TEST(StringFrom, BracketsFormattedHexStringFromForInt)
{
    int value = -1;

#if (INT_MAX == INT16_MAX)
    STRCMP_EQUAL("(0xffff)", BracketsFormattedHexStringFrom(value).c_str());
#else
    STRCMP_EQUAL("(0xffffffff)", BracketsFormattedHexStringFrom(value).c_str());
#endif
}

TEST(StringFrom, BracketsFormattedHexStringFromForLong)
{
    long value = 1;

    STRCMP_EQUAL("(0x1)", BracketsFormattedHexStringFrom(value).c_str());
}

TEST(StringFrom, BracketsFormattedHexStringFromForLongLong)
{
    long long value = 1;

    STRCMP_EQUAL("(0x1)", BracketsFormattedHexStringFrom(value).c_str());
}
TEST(StringFrom, BracketsFormattedHexStringFromForULongLong)
{
    unsigned long long value = 1;

    STRCMP_EQUAL("(0x1)", BracketsFormattedHexStringFrom(value).c_str());
}
