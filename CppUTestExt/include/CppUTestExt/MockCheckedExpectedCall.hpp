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

#ifndef D_MockCheckedExpectedCall_h
#define D_MockCheckedExpectedCall_h

#include "CppUTestExt/MockExpectedCall.hpp"
#include "CppUTestExt/MockNamedValue.hpp"

class MockCheckedExpectedCall : public MockExpectedCall {

public:
    MockCheckedExpectedCall();
    MockCheckedExpectedCall(unsigned int numCalls);
    ~MockCheckedExpectedCall() override;

    MockExpectedCall& withName(const SimpleString& name) override;
    MockExpectedCall& withCallOrder(unsigned int callOrder) override { return withCallOrder(callOrder, callOrder); }
    MockExpectedCall& withCallOrder(unsigned int initialCallOrder, unsigned int finalCallOrder) override;
    MockExpectedCall& withBoolParameter(const SimpleString& name, bool value) override;
    MockExpectedCall& withIntParameter(const SimpleString& name, int value) override;
    MockExpectedCall& withUnsignedIntParameter(const SimpleString& name, unsigned int value) override;
    MockExpectedCall& withLongIntParameter(const SimpleString& name, long int value) override;
    MockExpectedCall& withUnsignedLongIntParameter(const SimpleString& name, unsigned long int value) override;
    MockExpectedCall& withLongLongIntParameter(const SimpleString& name, long long value) override;
    MockExpectedCall& withUnsignedLongLongIntParameter(const SimpleString& name, unsigned long long value) override;
    MockExpectedCall& withDoubleParameter(const SimpleString& name, double value) override;
    MockExpectedCall& withDoubleParameter(const SimpleString& name, double value, double tolerance) override;
    MockExpectedCall& withStringParameter(const SimpleString& name, const char* value) override;
    MockExpectedCall& withPointerParameter(const SimpleString& name, void* value) override;
    MockExpectedCall& withConstPointerParameter(const SimpleString& name, const void* value) override;
    MockExpectedCall& withFunctionPointerParameter(const SimpleString& name, void (*value)()) override;
    MockExpectedCall& withMemoryBufferParameter(const SimpleString& name, const unsigned char* value, size_t size) override;
    MockExpectedCall& withParameterOfType(const SimpleString& typeName, const SimpleString& name, const void* value) override;
    MockExpectedCall& withOutputParameterReturning(const SimpleString& name, const void* value, size_t size) override;
    MockExpectedCall& withOutputParameterOfTypeReturning(const SimpleString& typeName, const SimpleString& name, const void* value) override;
    MockExpectedCall& withUnmodifiedOutputParameter(const SimpleString& name) override;
    MockExpectedCall& ignoreOtherParameters() override;

    MockExpectedCall& andReturnValue(bool value) override;
    MockExpectedCall& andReturnValue(int value) override;
    MockExpectedCall& andReturnValue(unsigned int value) override;
    MockExpectedCall& andReturnValue(long int value) override;
    MockExpectedCall& andReturnValue(unsigned long int value) override;
    MockExpectedCall& andReturnValue(long long value) override;
    MockExpectedCall& andReturnValue(unsigned long long value) override;
    MockExpectedCall& andReturnValue(double value) override;
    MockExpectedCall& andReturnValue(const char* value) override;
    MockExpectedCall& andReturnValue(void* value) override;
    MockExpectedCall& andReturnValue(const void* value) override;
    MockExpectedCall& andReturnValue(void (*value)()) override;

    virtual MockNamedValue returnValue();

    MockExpectedCall& onObject(void* objectPtr) override;

    virtual MockNamedValue getInputParameter(const SimpleString& name);
    virtual MockNamedValue getOutputParameter(const SimpleString& name);
    virtual SimpleString getInputParameterType(const SimpleString& name);
    virtual SimpleString getInputParameterValueString(const SimpleString& name);

    virtual bool hasInputParameterWithName(const SimpleString& name);
    virtual bool hasInputParameter(const MockNamedValue& parameter);
    virtual bool hasOutputParameterWithName(const SimpleString& name);
    virtual bool hasOutputParameter(const MockNamedValue& parameter);
    virtual bool relatesTo(const SimpleString& functionName);
    virtual bool relatesToObject(const void* objectPtr) const;

    virtual bool isFulfilled();
    virtual bool canMatchActualCalls();
    virtual bool isMatchingActualCallAndFinalized();
    virtual bool isMatchingActualCall();
    virtual bool areParametersMatchingActualCall();
    virtual bool isOutOfOrder() const;

    virtual void callWasMade(unsigned int callOrder);
    virtual void inputParameterWasPassed(const SimpleString& name);
    virtual void outputParameterWasPassed(const SimpleString& name);
    virtual void finalizeActualCallMatch();
    virtual void wasPassedToObject();
    virtual void resetActualCallMatchingState();

    virtual SimpleString callToString();
    virtual SimpleString missingParametersToString();

    enum { NO_EXPECTED_CALL_ORDER = 0 };

    virtual unsigned int getActualCallsFulfilled() const;

protected:
    void setName(const SimpleString& name);
    SimpleString getName() const;

private:
    SimpleString functionName_;

    class MockExpectedFunctionParameter : public MockNamedValue {
    public:
        MockExpectedFunctionParameter(const SimpleString& name);
        void setMatchesActualCall(bool b);
        bool isMatchingActualCall() const;

    private:
        bool matchesActualCall_;
    };

    MockExpectedFunctionParameter* item(MockNamedValueListNode* node);

    bool ignoreOtherParameters_;
    bool isActualCallMatchFinalized_;
    unsigned int initialExpectedCallOrder_;
    unsigned int finalExpectedCallOrder_;
    bool outOfOrder_;
    MockNamedValueList* inputParameters_;
    MockNamedValueList* outputParameters_;
    MockNamedValue returnValue_;
    void* objectPtr_;
    bool isSpecificObjectExpected_;
    bool wasPassedToObject_;
    unsigned int actualCalls_;
    unsigned int expectedCalls_;
};

class MockIgnoredExpectedCall : public MockExpectedCall {
public:
    MockExpectedCall& withName(const SimpleString&) override { return *this; }
    MockExpectedCall& withCallOrder(unsigned int) override { return *this; }
    MockExpectedCall& withCallOrder(unsigned int, unsigned int) override { return *this; }
    MockExpectedCall& withBoolParameter(const SimpleString&, bool) override { return *this; }
    MockExpectedCall& withIntParameter(const SimpleString&, int) override { return *this; }
    MockExpectedCall& withUnsignedIntParameter(const SimpleString&, unsigned int) override { return *this; }
    MockExpectedCall& withLongIntParameter(const SimpleString&, long int) override { return *this; }
    MockExpectedCall& withUnsignedLongIntParameter(const SimpleString&, unsigned long int) override { return *this; }
    MockExpectedCall& withLongLongIntParameter(const SimpleString&, long long) override { return *this; }
    MockExpectedCall& withUnsignedLongLongIntParameter(const SimpleString&, unsigned long long) override { return *this; }
    MockExpectedCall& withDoubleParameter(const SimpleString&, double) override { return *this; }
    MockExpectedCall& withDoubleParameter(const SimpleString&, double, double) override { return *this; }
    MockExpectedCall& withStringParameter(const SimpleString&, const char*) override { return *this; }
    MockExpectedCall& withPointerParameter(const SimpleString&, void*) override { return *this; }
    MockExpectedCall& withConstPointerParameter(const SimpleString&, const void*) override { return *this; }
    MockExpectedCall& withFunctionPointerParameter(const SimpleString&, void (*)()) override { return *this; }
    MockExpectedCall& withMemoryBufferParameter(const SimpleString&, const unsigned char*, size_t) override { return *this; }
    MockExpectedCall& withParameterOfType(const SimpleString&, const SimpleString&, const void*) override { return *this; }
    MockExpectedCall& withOutputParameterReturning(const SimpleString&, const void*, size_t) override { return *this; }
    MockExpectedCall& withOutputParameterOfTypeReturning(const SimpleString&, const SimpleString&, const void*) override { return *this; }
    MockExpectedCall& withUnmodifiedOutputParameter(const SimpleString&) override { return *this; }
    MockExpectedCall& ignoreOtherParameters() override { return *this; }

    MockExpectedCall& andReturnValue(bool) override { return *this; }
    MockExpectedCall& andReturnValue(int) override { return *this; }
    MockExpectedCall& andReturnValue(unsigned int) override { return *this; }
    MockExpectedCall& andReturnValue(long int) override { return *this; }
    MockExpectedCall& andReturnValue(unsigned long int) override { return *this; }
    MockExpectedCall& andReturnValue(long long) override { return *this; }
    MockExpectedCall& andReturnValue(unsigned long long) override { return *this; }
    MockExpectedCall& andReturnValue(double) override { return *this; }
    MockExpectedCall& andReturnValue(const char*) override { return *this; }
    MockExpectedCall& andReturnValue(void*) override { return *this; }
    MockExpectedCall& andReturnValue(const void*) override { return *this; }
    MockExpectedCall& andReturnValue(void (*)()) override { return *this; }

    MockExpectedCall& onObject(void*) override { return *this; }

    static MockExpectedCall& instance();
};

#endif
