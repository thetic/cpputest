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

#include <cctype>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstring>
#include <string>

namespace {
const char* StrStr(const char* s1, const char* s2)
{
    if (!*s2)
        return s1;
    for (; *s1; s1++)
        if (std::strncmp(s1, s2, std::strlen(s2)) == 0)
            return s1;
    return nullptr;
}

bool isControlWithShortEscapeSequence(char ch)
{
    return '\a' <= ch && '\r' >= ch;
}

size_t printable_size(const std::string& string)
{
    size_t str_size = string.size();
    size_t printable_str_size = str_size;

    for (size_t i = 0; i < str_size; i++) {
        char c = string[i];
        if (isControlWithShortEscapeSequence(c)) {
            printable_str_size += 1;
        } else if (std::iscntrl(c)) {
            printable_str_size += 3;
        }
    }

    return printable_str_size;
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

    size_t size = (size_t)std::vsnprintf(defaultBuffer, sizeOfdefaultBuffer, format, args);
    if (size < sizeOfdefaultBuffer) {
        resultString = std::string(defaultBuffer);
    } else {
        size_t newBufferSize = size + 1;
        char* newBuffer = (char*)std::malloc(newBufferSize);
        std::vsnprintf(newBuffer, newBufferSize, format, argsCopy);
        resultString = std::string(newBuffer);

        std::free(newBuffer);
    }
    va_end(argsCopy);
    return resultString;
}
}

namespace cpputest {

// does not support + or - prefixes
unsigned strings::atou(const char* str)
{
    while (std::isspace(*str))
        str++;

    unsigned result = 0;
    for (; std::isdigit(*str) && *str >= '0'; str++) {
        result *= 10;
        result += static_cast< unsigned >(*str - '0');
    }
    return result;
}

bool strings::contains(const std::string& string, const std::string& substring)
{
    return string.find(substring) != std::string::npos;
}

bool strings::starts_with(const std::string& string, const std::string& prefix)
{
    return (string.find(prefix) == 0);
}

bool strings::ends_with(const std::string& string, const std::string& suffix)
{
    std::string::size_type pos = string.rfind(suffix);
    return (pos != std::string::npos) && (pos == (string.length() - suffix.length()));
}

size_t strings::count(const std::string& string, const std::string& substring)
{
    size_t num = 0;
    const char* str = string.data();
    const char* strpart = nullptr;
    if (*str) {
        strpart = StrStr(str, substring.data());
    }
    while (*str && strpart) {
        str = strpart;
        str++;
        num++;
        strpart = StrStr(str, substring.data());
    }
    return num;
}

std::string& strings::replaceAll(std::string& str, const std::string& to, const std::string& with)
{
    size_t pos = str.find(to);
    while ((pos != std::string::npos) && (pos < str.length())) {
        str.replace(pos, to.length(), with);
        pos = str.find(to, pos + with.length());
    }
    return str;
}

std::string& strings::replaceAll(std::string& str, char to, char with)
{
    for (auto& c : str) {
        if (c == to) {
            c = with;
        }
    }
    return str;
}

std::string strings::printable(const std::string& string)
{
    static const char* shortEscapeCodes[] = {
        "\\a",
        "\\b",
        "\\t",
        "\\n",
        "\\v",
        "\\f",
        "\\r"
    };

    std::string result(printable_size(string), ' ');

    size_t str_size = string.size();
    size_t j = 0;
    for (size_t i = 0; i < str_size; i++) {
        char c = string[i];
        if (isControlWithShortEscapeSequence(c)) {
            std::strncpy(&result[j], shortEscapeCodes[(unsigned char)(c - '\a')], 2);
            j += 2;
        } else if (std::iscntrl(c)) {
            std::string hexEscapeCode = StringFromFormat("\\x%02X ", c);
            std::strncpy(&result[j], hexEscapeCode.c_str(), 4);
            j += 4;
        } else {
            result[j] = c;
            j++;
        }
    }
    result[j] = 0;

    return result;
}

std::string strings::lowercase(const std::string& string)
{
    std::string result = string;
    for (auto& c : result)
        c = std::tolower(c);
    return result;
}

void strings::padStringsToSameLength(std::string& str1, std::string& str2, char padCharacter)
{
    if (str1.size() > str2.size()) {
        padStringsToSameLength(str2, str1, padCharacter);
        return;
    }

    str1 = std::string(str2.size() - str1.size(), padCharacter) + str1;
}

std::string strings::subStringFromTill(const std::string& string, char startChar, char lastExcludedChar)
{
    size_t beginPos = string.find(startChar);
    if (beginPos == std::string::npos)
        return "";

    size_t endPos = string.find(lastExcludedChar, beginPos);
    if (endPos == std::string::npos)
        return string.substr(beginPos);

    return string.substr(beginPos, endPos - beginPos);
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

std::string PrintableStringFromOrNull(const char* expected)
{
    return (expected) ? cpputest::strings::printable(expected) : "(null)";
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
    if (std::isnan(value))
        return "Nan - Not a number";
    else if (std::isinf(value))
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
