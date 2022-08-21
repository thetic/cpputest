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

#ifndef D_MockExpectedCall_h
#define D_MockExpectedCall_h

#include <cstddef>
#include <string>

class MockNamedValue;

extern std::string StringFrom(const MockNamedValue& parameter);

class MockExpectedCall {
public:
    MockExpectedCall();
    virtual ~MockExpectedCall();

    virtual MockExpectedCall& withName(const std::string& name) = 0;
    virtual MockExpectedCall& withCallOrder(unsigned int) = 0;
    virtual MockExpectedCall& withCallOrder(unsigned int, unsigned int) = 0;
    MockExpectedCall& withParameter(const std::string& name, bool value) { return withBoolParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, int value) { return withIntParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, unsigned int value) { return withUnsignedIntParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, long int value) { return withLongIntParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, unsigned long int value) { return withUnsignedLongIntParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, long long value) { return withLongLongIntParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, unsigned long long value) { return withUnsignedLongLongIntParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, double value) { return withDoubleParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, double value, double tolerance) { return withDoubleParameter(name, value, tolerance); }
    MockExpectedCall& withParameter(const std::string& name, const char* value) { return withStringParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, void* value) { return withPointerParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, const void* value) { return withConstPointerParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, void (*value)()) { return withFunctionPointerParameter(name, value); }
    MockExpectedCall& withParameter(const std::string& name, const unsigned char* value, size_t size) { return withMemoryBufferParameter(name, value, size); }
    virtual MockExpectedCall& withParameterOfType(const std::string& typeName, const std::string& name, const void* value) = 0;
    virtual MockExpectedCall& withOutputParameterReturning(const std::string& name, const void* value, size_t size) = 0;
    virtual MockExpectedCall& withOutputParameterOfTypeReturning(const std::string& typeName, const std::string& name, const void* value) = 0;
    virtual MockExpectedCall& withUnmodifiedOutputParameter(const std::string& name) = 0;
    virtual MockExpectedCall& ignoreOtherParameters() = 0;

    virtual MockExpectedCall& withBoolParameter(const std::string& name, bool value) = 0;
    virtual MockExpectedCall& withIntParameter(const std::string& name, int value) = 0;
    virtual MockExpectedCall& withUnsignedIntParameter(const std::string& name, unsigned int value) = 0;
    virtual MockExpectedCall& withLongIntParameter(const std::string& name, long int value) = 0;
    virtual MockExpectedCall& withUnsignedLongIntParameter(const std::string& name, unsigned long int value) = 0;
    virtual MockExpectedCall& withLongLongIntParameter(const std::string& name, long long value) = 0;
    virtual MockExpectedCall& withUnsignedLongLongIntParameter(const std::string& name, unsigned long long value) = 0;
    virtual MockExpectedCall& withDoubleParameter(const std::string& name, double value) = 0;
    virtual MockExpectedCall& withDoubleParameter(const std::string& name, double value, double tolerance) = 0;
    virtual MockExpectedCall& withStringParameter(const std::string& name, const char* value) = 0;
    virtual MockExpectedCall& withPointerParameter(const std::string& name, void* value) = 0;
    virtual MockExpectedCall& withFunctionPointerParameter(const std::string& name, void (*value)()) = 0;
    virtual MockExpectedCall& withConstPointerParameter(const std::string& name, const void* value) = 0;
    virtual MockExpectedCall& withMemoryBufferParameter(const std::string& name, const unsigned char* value, size_t size) = 0;
    virtual MockExpectedCall& andReturnValue(bool value) = 0;
    virtual MockExpectedCall& andReturnValue(int value) = 0;
    virtual MockExpectedCall& andReturnValue(unsigned int value) = 0;
    virtual MockExpectedCall& andReturnValue(long int value) = 0;
    virtual MockExpectedCall& andReturnValue(unsigned long int value) = 0;
    virtual MockExpectedCall& andReturnValue(long long value) = 0;
    virtual MockExpectedCall& andReturnValue(unsigned long long value) = 0;
    virtual MockExpectedCall& andReturnValue(double value) = 0;
    virtual MockExpectedCall& andReturnValue(const char* value) = 0;
    virtual MockExpectedCall& andReturnValue(void* value) = 0;
    virtual MockExpectedCall& andReturnValue(const void* value) = 0;
    virtual MockExpectedCall& andReturnValue(void (*value)()) = 0;

    virtual MockExpectedCall& onObject(void* objectPtr) = 0;
};

#endif
