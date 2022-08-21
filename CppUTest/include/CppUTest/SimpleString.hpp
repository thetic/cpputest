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

#ifndef D_SimpleString_h
#define D_SimpleString_h

#include <cstdarg>
#include <cstddef>
#include <string>

namespace strings {
std::string& replaceAll(std::string& str, const std::string& to, const std::string& with);
std::string& replaceAll(std::string& str, char to, char with);
size_t count(const std::string& string, const std::string& substring);
std::string subStringFromTill(const std::string&, char startChar, char lastExcludedChar);
std::string printable(const std::string&);
void padStringsToSameLength(std::string& str1, std::string& str2, char ch);
std::string lowercase(const std::string&);
unsigned atou(const char* str);
bool contains(const std::string& string, const std::string& substring);
bool ends_with(const std::string& string, const std::string& suffix);
bool starts_with(const std::string& string, const std::string& prefix);
}

class SimpleString {
    friend bool operator==(const SimpleString& left, const SimpleString& right)
    {
        return left.string_ == right.string_;
    }
    friend bool operator!=(const SimpleString& left, const SimpleString& right)
    {
        return left.string_ != right.string_;
    }

public:
    SimpleString(const char* value = "")
        : string_(value ? value : "") {};
    SimpleString(size_t repeatCount, char c)
        : string_(repeatCount, c) {};
    SimpleString(const SimpleString& other)
        : string_(other.string_) {};
    ~SimpleString() = default;
    SimpleString(const std::string& other)
        : string_(other)
    {
    }

    operator std::string() const { return std::string(string_); }

    SimpleString& operator=(const SimpleString& other)
    {
        string_ = other.string_;
        return *this;
    }
    SimpleString operator+(const SimpleString& rhs) const { return string_ + rhs.string_; }
    SimpleString& operator+=(const SimpleString& rhs)
    {
        string_ += rhs.string_;
        return *this;
    }
    SimpleString& operator+=(const char* rhs)
    {
        string_ += rhs;
        return *this;
    }

    char operator[](size_t pos) const { return string_[pos]; }

    SimpleString substr(size_t beginPos, size_t amount = std::string::npos) const { return string_.substr(beginPos, amount); }
    size_t find(char ch, size_t pos = 0) const { return string_.find(ch, pos); }

    const char* c_str() const { return string_.c_str(); }
    const char* data() const { return string_.data(); };
    size_t size() const { return string_.size(); }
    bool empty() const { return string_.empty(); }

private:
    std::string string_;
};

SimpleString StringFrom(bool value);
SimpleString StringFrom(const void* value);
SimpleString StringFrom(void (*value)());
SimpleString StringFrom(char value);
SimpleString StringFrom(const char* value);
SimpleString StringFromOrNull(const char* value);
SimpleString StringFrom(int value);
SimpleString StringFrom(unsigned int value);
SimpleString StringFrom(long value);
SimpleString StringFrom(unsigned long value);
SimpleString StringFrom(long long value);
SimpleString StringFrom(unsigned long long value);
SimpleString HexStringFrom(unsigned int value);
SimpleString HexStringFrom(int value);
SimpleString HexStringFrom(signed char value);
SimpleString HexStringFrom(long value);
SimpleString HexStringFrom(unsigned long value);
SimpleString HexStringFrom(long long value);
SimpleString HexStringFrom(unsigned long long value);
SimpleString HexStringFrom(const void* value);
SimpleString HexStringFrom(void (*value)());
SimpleString StringFrom(double value, int precision = 6);
SimpleString StringFrom(const SimpleString& other);
SimpleString StringFrom(const std::nullptr_t value);
SimpleString StringFrom(const std::string& other);

#ifdef __has_attribute
#if __has_attribute(format)
#ifdef __MINGW_PRINTF_FORMAT
__attribute__((format(__MINGW_PRINTF_FORMAT, 1, 2)))
#else
__attribute__((format(printf, 1, 2)))
#endif
#endif
#endif
SimpleString
StringFromFormat(const char* format, ...);

SimpleString VStringFromFormat(const char* format, va_list args);
SimpleString StringFromBinary(const unsigned char* value, size_t size);
SimpleString StringFromBinaryOrNull(const unsigned char* value, size_t size);
SimpleString StringFromBinaryWithSize(const unsigned char* value, size_t size);
SimpleString StringFromBinaryWithSizeOrNull(const unsigned char* value, size_t size);
SimpleString StringFromMaskedBits(unsigned long value, unsigned long mask, size_t byteCount);
SimpleString StringFromOrdinalNumber(unsigned int number);
SimpleString BracketsFormattedHexStringFrom(int value);
SimpleString BracketsFormattedHexStringFrom(unsigned int value);
SimpleString BracketsFormattedHexStringFrom(long value);
SimpleString BracketsFormattedHexStringFrom(unsigned long value);
SimpleString BracketsFormattedHexStringFrom(long long value);
SimpleString BracketsFormattedHexStringFrom(unsigned long long value);
SimpleString BracketsFormattedHexStringFrom(signed char value);
SimpleString BracketsFormattedHexString(SimpleString hexString);
SimpleString PrintableStringFromOrNull(const char* expected);

#endif
