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

namespace Strings {
std::string replace(std::string& orig, const std::string& to, const std::string& with);
bool starts_with(const std::string& str, const std::string& substr);
bool ends_with(const std::string& str, const std::string& substr);
bool contains(const std::string& str, const std::string& substr);
}

std::string StringFrom(bool value);
std::string StringFrom(const void* value);
std::string StringFrom(void (*value)());
std::string StringFrom(char value);
std::string StringFrom(const char* value);
std::string StringFromOrNull(const char* value);
std::string StringFrom(int value);
std::string StringFrom(unsigned int value);
std::string StringFrom(long value);
std::string StringFrom(unsigned long value);
std::string StringFrom(long long value);
std::string StringFrom(unsigned long long value);
std::string HexStringFrom(unsigned int value);
std::string HexStringFrom(int value);
std::string HexStringFrom(signed char value);
std::string HexStringFrom(long value);
std::string HexStringFrom(unsigned long value);
std::string HexStringFrom(long long value);
std::string HexStringFrom(unsigned long long value);
std::string HexStringFrom(const void* value);
std::string HexStringFrom(void (*value)());
std::string StringFrom(double value, int precision = 6);
std::string StringFrom(const std::nullptr_t value);
std::string StringFrom(const std::string& other);

#ifdef __has_attribute
#if __has_attribute(format)
#ifdef __MINGW_PRINTF_FORMAT
__attribute__((format(__MINGW_PRINTF_FORMAT, 1, 2)))
#else
__attribute__((format(printf, 1, 2)))
#endif
#endif
#endif
std::string
StringFromFormat(const char* format, ...);

std::string VStringFromFormat(const char* format, va_list args);
std::string StringFromBinary(const unsigned char* value, size_t size);
std::string StringFromBinaryOrNull(const unsigned char* value, size_t size);
std::string StringFromBinaryWithSize(const unsigned char* value, size_t size);
std::string StringFromBinaryWithSizeOrNull(const unsigned char* value, size_t size);
std::string StringFromMaskedBits(unsigned long value, unsigned long mask, size_t byteCount);
std::string StringFromOrdinalNumber(unsigned int number);
std::string BracketsFormattedHexStringFrom(int value);
std::string BracketsFormattedHexStringFrom(unsigned int value);
std::string BracketsFormattedHexStringFrom(long value);
std::string BracketsFormattedHexStringFrom(unsigned long value);
std::string BracketsFormattedHexStringFrom(long long value);
std::string BracketsFormattedHexStringFrom(unsigned long long value);
std::string BracketsFormattedHexStringFrom(signed char value);
std::string BracketsFormattedHexString(std::string hexString);

#endif
