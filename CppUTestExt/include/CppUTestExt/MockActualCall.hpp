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

#ifndef D_MockActualCall_h
#define D_MockActualCall_h

#include "CppUTestExt/MockExpectedCallsList.hpp"
#include "CppUTestExt/MockNamedValue.hpp"

#include "CppUTest/TestHarness.hpp"

class MockFailureReporter;
class MockFailure;

class MockActualCall {
public:
    MockActualCall();
    virtual ~MockActualCall();

    virtual MockActualCall& withName(const std::string& name) = 0;
    virtual MockActualCall& withCallOrder(unsigned int callOrder) = 0;
    MockActualCall& withParameter(const std::string& name, bool value) { return withBoolParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, int value) { return withIntParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, unsigned int value) { return withUnsignedIntParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, long int value) { return withLongIntParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, unsigned long int value) { return withUnsignedLongIntParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, long long value) { return withLongLongIntParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, unsigned long long value) { return withUnsignedLongLongIntParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, double value) { return withDoubleParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, const char* value) { return withStringParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, void* value) { return withPointerParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, void (*value)()) { return withFunctionPointerParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, const void* value) { return withConstPointerParameter(name, value); }
    MockActualCall& withParameter(const std::string& name, const unsigned char* value, size_t size) { return withMemoryBufferParameter(name, value, size); }
    virtual MockActualCall& withParameterOfType(const std::string& typeName, const std::string& name, const void* value) = 0;
    virtual MockActualCall& withOutputParameter(const std::string& name, void* output) = 0;
    virtual MockActualCall& withOutputParameterOfType(const std::string& typeName, const std::string& name, void* output) = 0;

    virtual MockActualCall& withBoolParameter(const std::string& name, bool value) = 0;
    virtual MockActualCall& withIntParameter(const std::string& name, int value) = 0;
    virtual MockActualCall& withUnsignedIntParameter(const std::string& name, unsigned int value) = 0;
    virtual MockActualCall& withLongIntParameter(const std::string& name, long int value) = 0;
    virtual MockActualCall& withUnsignedLongIntParameter(const std::string& name, unsigned long int value) = 0;
    virtual MockActualCall& withLongLongIntParameter(const std::string& name, long long value) = 0;
    virtual MockActualCall& withUnsignedLongLongIntParameter(const std::string& name, unsigned long long value) = 0;
    virtual MockActualCall& withDoubleParameter(const std::string& name, double value) = 0;
    virtual MockActualCall& withStringParameter(const std::string& name, const char* value) = 0;
    virtual MockActualCall& withPointerParameter(const std::string& name, void* value) = 0;
    virtual MockActualCall& withFunctionPointerParameter(const std::string& name, void (*value)()) = 0;
    virtual MockActualCall& withConstPointerParameter(const std::string& name, const void* value) = 0;
    virtual MockActualCall& withMemoryBufferParameter(const std::string& name, const unsigned char* value, size_t size) = 0;

    virtual bool hasReturnValue() = 0;
    virtual MockNamedValue returnValue() = 0;

    virtual bool returnBoolValueOrDefault(bool default_value) = 0;
    virtual bool returnBoolValue() = 0;

    virtual int returnIntValueOrDefault(int default_value) = 0;
    virtual int returnIntValue() = 0;

    virtual unsigned long int returnUnsignedLongIntValue() = 0;
    virtual unsigned long int returnUnsignedLongIntValueOrDefault(unsigned long int default_value) = 0;

    virtual long int returnLongIntValue() = 0;
    virtual long int returnLongIntValueOrDefault(long int default_value) = 0;

    virtual unsigned long long returnUnsignedLongLongIntValue() = 0;
    virtual unsigned long long returnUnsignedLongLongIntValueOrDefault(unsigned long long default_value) = 0;

    virtual long long returnLongLongIntValue() = 0;
    virtual long long returnLongLongIntValueOrDefault(long long default_value) = 0;

    virtual unsigned int returnUnsignedIntValue() = 0;
    virtual unsigned int returnUnsignedIntValueOrDefault(unsigned int default_value) = 0;

    virtual const char* returnStringValueOrDefault(const char* default_value) = 0;
    virtual const char* returnStringValue() = 0;

    virtual double returnDoubleValue() = 0;
    virtual double returnDoubleValueOrDefault(double default_value) = 0;

    virtual void* returnPointerValue() = 0;
    virtual void* returnPointerValueOrDefault(void* default_value) = 0;

    virtual const void* returnConstPointerValue() = 0;
    virtual const void* returnConstPointerValueOrDefault(const void* default_value) = 0;

    using FunctionPointer = void (*)();
    virtual FunctionPointer returnFunctionPointerValue() = 0;
    virtual FunctionPointer returnFunctionPointerValueOrDefault(FunctionPointer default_value) = 0;

    virtual MockActualCall& onObject(const void* objectPtr) = 0;
};

#endif
