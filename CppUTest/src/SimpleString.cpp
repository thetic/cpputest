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

#include <cctype>
#include <climits>
#include <cstring>
#include <string>

// does not support + or - prefixes
unsigned SimpleString::AtoU(const char* str)
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

const char* SimpleString::StrStr(const char* s1, const char* s2)
{
    if (!*s2)
        return s1;
    std::string::size_type pos = std::string(s1).find(s2);
    if (pos == npos)
        return nullptr;
    return &s1[pos];
}

SimpleString::SimpleString(const char* otherBuffer)
    : str_(otherBuffer ? otherBuffer : "")
{
}

SimpleString::SimpleString(const char* other, size_t repeatCount)
{
    for (size_t i = 0; i < repeatCount; ++i) {
        str_ += other;
    }
}

SimpleString::SimpleString(const SimpleString& other)
    : str_(other.str_)
{
}

SimpleString& SimpleString::operator=(const SimpleString& other)
{
    if (this != &other)
        str_ = other.str_;
    return *this;
}

bool SimpleString::contains(const SimpleString& other) const
{
    return str_.find(other.str_) != npos;
}

bool SimpleString::starts_with(const SimpleString& other) const
{
    if (other.size() == 0)
        return true;
    else if (size() == 0)
        return false;
    else
        return str_.find(other.str_) == 0;
}

bool SimpleString::ends_with(const SimpleString& other) const
{
    size_t length = size();
    size_t other_length = other.size();

    if (other_length == 0)
        return true;
    if (length == 0)
        return false;
    if (length < other_length)
        return false;

    return std::strcmp(str_.c_str() + length - other_length, other.str_.c_str()) == 0;
}

size_t SimpleString::count(const SimpleString& substr) const
{
    size_t num = 0;
    const char* str = str_.c_str();
    const char* strpart = nullptr;
    if (*str) {
        strpart = StrStr(str, substr.str_.c_str());
    }
    while (*str && strpart) {
        str = strpart;
        str++;
        num++;
        strpart = StrStr(str, substr.str_.c_str());
    }
    return num;
}

void SimpleString::split(const SimpleString& delimiter, SimpleStringCollection& col) const
{
    size_t num = count(delimiter);
    size_t extraEndToken = (ends_with(delimiter)) ? 0 : 1U;
    col.allocate(num + extraEndToken);

    const char* str = str_.c_str();
    const char* prev;
    for (size_t i = 0; i < num; ++i) {
        prev = str;
        str = StrStr(str, delimiter.str_.c_str()) + 1;
        col[i] = SimpleString(prev).substr(0, size_t(str - prev));
    }
    if (extraEndToken) {
        col[num] = str;
    }
}

void SimpleString::replace(char to, char with)
{
    size_t s = size();
    for (size_t i = 0; i < s; i++) {
        if (str_.c_str()[i] == to)
            str_[i] = with;
    }
}

void SimpleString::replace(const char* to, const char* with)
{
    size_t c = count(to);
    if (c == 0) {
        return;
    }
    size_t len = size();
    size_t tolen = std::strlen(to);
    size_t withlen = std::strlen(with);

    size_t newsize = len + (withlen * c) - (tolen * c) + 1;

    if (newsize > 1) {
        char* newbuf = (char*)PlatformSpecificMalloc(newsize);
        for (size_t i = 0, j = 0; i < len;) {
            if (std::strncmp(&str_.c_str()[i], to, tolen) == 0) {
                std::strncpy(&newbuf[j], with, withlen + 1);
                j += withlen;
                i += tolen;
            } else {
                newbuf[j] = str_.c_str()[i];
                j++;
                i++;
            }
        }
        newbuf[newsize - 1] = '\0';
        str_ = newbuf;
        PlatformSpecificFree(newbuf);
    } else
        str_.clear();
}

SimpleString SimpleString::lowerCase() const
{
    SimpleString str(*this);

    size_t str_size = str.size();
    for (size_t i = 0; i < str_size; i++)
        str.str_[i] = std::tolower(str.str_.c_str()[i]);

    return str;
}

const char* SimpleString::c_str() const
{
    return str_.c_str();
}

size_t SimpleString::size() const
{
    return str_.size();
}

bool SimpleString::empty() const
{
    return size() == 0;
}

bool operator==(const SimpleString& left, const SimpleString& right)
{
    return left.str_ == right.str_;
}

bool operator!=(const SimpleString& left, const SimpleString& right)
{
    return !(left == right);
}

SimpleString SimpleString::operator+(const SimpleString& rhs) const
{
    SimpleString t(str_.c_str());
    t += rhs.str_.c_str();
    return t;
}

SimpleString& SimpleString::operator+=(const SimpleString& rhs)
{
    return operator+=(rhs.str_.c_str());
}

SimpleString& SimpleString::operator+=(const char* rhs)
{
    str_ += rhs;
    return *this;
}

SimpleString SimpleString::substr(size_t beginPos, size_t amount) const
{
    return SimpleString(str_.substr(beginPos, amount).c_str());
}

char SimpleString::operator[](size_t pos) const
{
    return str_[pos];
}

size_t SimpleString::find(char ch, size_t starting_position) const
{
    return str_.find(ch, starting_position);
}

SimpleString StringFrom(bool value)
{
    return SimpleString(StringFromFormat("%s", value ? "true" : "false"));
}

SimpleString StringFrom(const char* value)
{
    return SimpleString(value);
}

SimpleString StringFromOrNull(const char* expected)
{
    return (expected) ? StringFrom(expected) : StringFrom("(null)");
}

SimpleString StringFrom(int value)
{
    return StringFromFormat("%d", value);
}

SimpleString StringFrom(long value)
{
    return StringFromFormat("%ld", value);
}

SimpleString StringFrom(const void* value)
{
    return SimpleString("0x") + HexStringFrom(value);
}

SimpleString StringFrom(void (*value)())
{
    return SimpleString("0x") + HexStringFrom(value);
}

SimpleString HexStringFrom(long value)
{
    return StringFromFormat("%lx", value);
}

SimpleString HexStringFrom(int value)
{
    return StringFromFormat("%x", value);
}

SimpleString HexStringFrom(signed char value)
{
    SimpleString result = StringFromFormat("%x", value);
    if (value < 0) {
        size_t size = result.size();
        result = result.substr(size - (CHAR_BIT / 4));
    }
    return result;
}

SimpleString HexStringFrom(unsigned long value)
{
    return StringFromFormat("%lx", value);
}

SimpleString HexStringFrom(unsigned int value)
{
    return StringFromFormat("%x", value);
}

SimpleString BracketsFormattedHexStringFrom(int value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

SimpleString BracketsFormattedHexStringFrom(unsigned int value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

SimpleString BracketsFormattedHexStringFrom(long value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

SimpleString BracketsFormattedHexStringFrom(unsigned long value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

SimpleString BracketsFormattedHexStringFrom(signed char value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

SimpleString BracketsFormattedHexString(SimpleString hexString)
{
    return SimpleString("(0x") + hexString + ")";
}

SimpleString StringFrom(const std::nullptr_t value)
{
    (void)value;
    return "(null)";
}

SimpleString StringFrom(long long value)
{
    return StringFromFormat("%lld", value);
}

SimpleString StringFrom(unsigned long long value)
{
    return StringFromFormat("%llu", value);
}

SimpleString HexStringFrom(long long value)
{
    return StringFromFormat("%llx", value);
}

SimpleString HexStringFrom(unsigned long long value)
{
    return StringFromFormat("%llx", value);
}

SimpleString HexStringFrom(const void* value)
{
    return HexStringFrom((unsigned long long)value);
}

SimpleString HexStringFrom(void (*value)())
{
    return HexStringFrom((unsigned long long)value);
}

SimpleString BracketsFormattedHexStringFrom(long long value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

SimpleString BracketsFormattedHexStringFrom(unsigned long long value)
{
    return BracketsFormattedHexString(HexStringFrom(value));
}

SimpleString StringFrom(double value, int precision)
{
    if (PlatformSpecificIsNan(value))
        return "Nan - Not a number";
    else if (PlatformSpecificIsInf(value))
        return "Inf - Infinity";
    else
        return StringFromFormat("%.*g", precision, value);
}

SimpleString StringFrom(char value)
{
    return StringFromFormat("%c", value);
}

SimpleString StringFrom(const SimpleString& value)
{
    return SimpleString(value);
}

SimpleString StringFromFormat(const char* format, ...)
{
    SimpleString resultString;
    va_list arguments;
    va_start(arguments, format);

    resultString = VStringFromFormat(format, arguments);
    va_end(arguments);
    return resultString;
}

SimpleString StringFrom(unsigned int i)
{
    return StringFromFormat("%u", i);
}

SimpleString StringFrom(const std::string& value)
{
    return SimpleString(value.c_str());
}

SimpleString StringFrom(unsigned long i)
{
    return StringFromFormat("%lu", i);
}

SimpleString VStringFromFormat(const char* format, va_list args)
{
    va_list argsCopy;
    va_copy(argsCopy, args);
    enum {
        sizeOfdefaultBuffer = 100
    };
    char defaultBuffer[sizeOfdefaultBuffer];
    SimpleString resultString;

    size_t size = (size_t)PlatformSpecificVSNprintf(defaultBuffer, sizeOfdefaultBuffer, format, args);
    if (size < sizeOfdefaultBuffer) {
        resultString = SimpleString(defaultBuffer);
    } else {
        size_t newBufferSize = size + 1;
        char* newBuffer = (char*)PlatformSpecificMalloc(newBufferSize);
        PlatformSpecificVSNprintf(newBuffer, newBufferSize, format, argsCopy);
        resultString = SimpleString(newBuffer);

        free(newBuffer);
    }
    va_end(argsCopy);
    return resultString;
}

SimpleString StringFromBinary(const unsigned char* value, size_t size)
{
    SimpleString result;

    for (size_t i = 0; i < size; i++) {
        result += StringFromFormat("%02X ", value[i]);
    }
    result = result.substr(0, result.size() - 1);

    return result;
}

SimpleString StringFromBinaryOrNull(const unsigned char* value, size_t size)
{
    return (value) ? StringFromBinary(value, size) : StringFrom("(null)");
}

SimpleString StringFromBinaryWithSize(const unsigned char* value, size_t size)
{
    SimpleString result = StringFromFormat("Size = %u | HexContents = ", (unsigned)size);
    size_t displayedSize = ((size > 128) ? 128 : size);
    result += StringFromBinaryOrNull(value, displayedSize);
    if (size > displayedSize) {
        result += " ...";
    }
    return result;
}

SimpleString StringFromBinaryWithSizeOrNull(const unsigned char* value, size_t size)
{
    return (value) ? StringFromBinaryWithSize(value, size) : StringFrom("(null)");
}

SimpleString StringFromMaskedBits(unsigned long value, unsigned long mask, size_t byteCount)
{
    SimpleString result;
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

SimpleString StringFromOrdinalNumber(unsigned int number)
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

SimpleStringCollection::SimpleStringCollection()
{
    collection_ = nullptr;
    size_ = 0;
}

void SimpleStringCollection::allocate(size_t _size)
{
    delete[] collection_;

    size_ = _size;
    collection_ = new SimpleString[size_];
}

SimpleStringCollection::~SimpleStringCollection()
{
    delete[](collection_);
}

size_t SimpleStringCollection::size() const
{
    return size_;
}

SimpleString& SimpleStringCollection::operator[](size_t index)
{
    if (index >= size_) {
        empty_ = "";
        return empty_;
    }

    return collection_[index];
}
