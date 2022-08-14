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

#include "CppUTest/SimpleString.hpp"

#include "CppUTest/PlatformSpecificFunctions.hpp"
#include "CppUTest/TestHarness.hpp"

#include <regex>
#include <string>

#include <cctype>
#include <climits>
#include <cstring>

namespace Strings {

bool starts_with(const std::string& str, const std::string& substr)
{
    return str.find(substr) == 0;
}

bool ends_with(const std::string& str, const std::string& substr)
{
    return str.rfind(substr) == (str.size() - substr.size());
}

bool contains(const std::string& str, const std::string& substr)
{
    return str.find(substr) != std::string::npos;
}

std::string replace(std::string& orig, const std::string& to, const std::string& with)
{
    return orig = std::regex_replace(orig, std::regex(to), with);
}

}

std::string StringFrom(bool value)
{
    return std::string(StringFromFormat("%s", value ? "true" : "false"));
}

std::string StringFrom(const char* value)
{
    return std::string(value);
}

std::string StringFromOrNull(const char* expected)
{
    return (expected) ? StringFrom(expected) : StringFrom("(null)");
}

std::string StringFrom(int value)
{
    return StringFromFormat("%d", value);
}

std::string StringFrom(long value)
{
    return StringFromFormat("%ld", value);
}

std::string StringFrom(const void* value)
{
    return std::string("0x") + HexStringFrom(value);
}

std::string StringFrom(void (*value)())
{
    return std::string("0x") + HexStringFrom(value);
}

std::string HexStringFrom(long value)
{
    return StringFromFormat("%lx", value);
}

std::string HexStringFrom(int value)
{
    return StringFromFormat("%x", value);
}

std::string HexStringFrom(signed char value)
{
    std::string result = StringFromFormat("%x", value);
    if (value < 0) {
        size_t size = result.size();
        result = result.substr(size - (CHAR_BIT / 4));
    }
    return result;
}

std::string HexStringFrom(unsigned long value)
{
    return StringFromFormat("%lx", value);
}

std::string HexStringFrom(unsigned int value)
{
    return StringFromFormat("%x", value);
}

std::string BracketsFormattedHexStringFrom(int value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

std::string BracketsFormattedHexStringFrom(unsigned int value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

std::string BracketsFormattedHexStringFrom(long value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

std::string BracketsFormattedHexStringFrom(unsigned long value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

std::string BracketsFormattedHexStringFrom(signed char value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

std::string BracketsFormattedHexString(std::string hexString)
{
    return std::string("(0x") + hexString + ")";
}

std::string StringFrom(const std::nullptr_t value)
{
    (void)value;
    return "(null)";
}

std::string StringFrom(long long value)
{
    return StringFromFormat("%lld", value);
}

std::string StringFrom(unsigned long long value)
{
    return StringFromFormat("%llu", value);
}

std::string HexStringFrom(long long value)
{
    return StringFromFormat("%llx", value);
}

std::string HexStringFrom(unsigned long long value)
{
    return StringFromFormat("%llx", value);
}

std::string HexStringFrom(const void* value)
{
    return HexStringFrom((unsigned long long)value);
}

std::string HexStringFrom(void (*value)())
{
    return HexStringFrom((unsigned long long)value);
}

std::string BracketsFormattedHexStringFrom(long long value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

std::string BracketsFormattedHexStringFrom(unsigned long long value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

std::string StringFrom(double value, int precision)
{
    if (PlatformSpecificIsNan(value))
        return "Nan - Not a number";
    else if (PlatformSpecificIsInf(value))
        return "Inf - Infinity";
    else
        return StringFromFormat("%.*g", precision, value);
}

std::string StringFrom(char value)
{
    return StringFromFormat("%c", value);
}

std::string StringFromFormat(const char* format, ...)
{
    std::string resultString;
    va_list arguments;
    va_start(arguments, format);

    resultString = VStringFromFormat(format, arguments);
    va_end(arguments);
    return resultString;
}

std::string StringFrom(unsigned int i)
{
    return StringFromFormat("%u", i);
}

std::string StringFrom(const std::string& value)
{
    return std::string(value.c_str());
}

std::string StringFrom(unsigned long i)
{
    return StringFromFormat("%lu", i);
}

std::string VStringFromFormat(const char* format, va_list args)
{
    va_list argsCopy;
    va_copy(argsCopy, args);
    enum {
        sizeOfdefaultBuffer = 100
    };
    char defaultBuffer[sizeOfdefaultBuffer];
    std::string resultString;

    size_t size = (size_t)PlatformSpecificVSNprintf(defaultBuffer, sizeOfdefaultBuffer, format, args);
    if (size < sizeOfdefaultBuffer) {
        resultString = std::string(defaultBuffer);
    } else {
        size_t newBufferSize = size + 1;
        char* newBuffer = (char*)PlatformSpecificMalloc(newBufferSize);
        PlatformSpecificVSNprintf(newBuffer, newBufferSize, format, argsCopy);
        resultString = std::string(newBuffer);

        free(newBuffer);
    }
    va_end(argsCopy);
    return resultString;
}

std::string StringFromBinary(const unsigned char* value, size_t size)
{
    std::string result;

    for (size_t i = 0; i < size; i++) {
        result += StringFromFormat("%02X ", value[i]);
    }
    result = result.substr(0, result.size() - 1);

    return result;
}

std::string StringFromBinaryOrNull(const unsigned char* value, size_t size)
{
    return (value) ? StringFromBinary(value, size) : StringFrom("(null)");
}

std::string StringFromBinaryWithSize(const unsigned char* value, size_t size)
{
    std::string result = StringFromFormat("Size = %u | HexContents = ", (unsigned)size);
    size_t displayedSize = ((size > 128) ? 128 : size);
    result += StringFromBinaryOrNull(value, displayedSize);
    if (size > displayedSize) {
        result += " ...";
    }
    return result;
}

std::string StringFromBinaryWithSizeOrNull(const unsigned char* value, size_t size)
{
    return (value) ? StringFromBinaryWithSize(value, size) : StringFrom("(null)");
}

std::string StringFromMaskedBits(unsigned long value, unsigned long mask, size_t byteCount)
{
    std::string result;
    size_t bitCount = (byteCount > sizeof(unsigned long)) ? (sizeof(unsigned long) * CHAR_BIT) : (byteCount * CHAR_BIT);
    const unsigned long msbMask = (((unsigned long)1) << (bitCount - 1));

    for (size_t i = 0; i < bitCount; i++) {
        if (mask & msbMask) {
            result += (value & msbMask) ? "1" : "0";
        } else {
            result += "x";
        }

        if (((i % 8) == 7) && (i != (bitCount - 1))) {
            result += " ";
        }

        value <<= 1;
        mask <<= 1;
    }

    return result;
}

std::string StringFromOrdinalNumber(unsigned int number)
{
    unsigned int onesDigit = number % 10;

    const char* suffix;
    if (number >= 11 && number <= 13) {
        suffix = "th";
    } else if (3 == onesDigit) {
        suffix = "rd";
    } else if (2 == onesDigit) {
        suffix = "nd";
    } else if (1 == onesDigit) {
        suffix = "st";
    } else {
        suffix = "th";
    }

    return StringFromFormat("%u%s", number, suffix);
}
