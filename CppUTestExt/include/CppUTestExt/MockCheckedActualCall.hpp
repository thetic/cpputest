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

#ifndef D_MockCheckedActualCall_h
#define D_MockCheckedActualCall_h

#include "CppUTestExt/MockActualCall.hpp"
#include "CppUTestExt/MockExpectedCallsList.hpp"

class MockCheckedActualCall : public MockActualCall
{
public:
    MockCheckedActualCall(unsigned int callOrder, MockFailureReporter* reporter, const MockExpectedCallsList& expectations);
    ~MockCheckedActualCall() override;

    MockActualCall& withName(const SimpleString& name) override;
    MockActualCall& withCallOrder(unsigned int) override;
    MockActualCall& withBoolParameter(const SimpleString& name, bool value) override;
    MockActualCall& withIntParameter(const SimpleString& name, int value) override;
    MockActualCall& withUnsignedIntParameter(const SimpleString& name, unsigned int value) override;
    MockActualCall& withLongIntParameter(const SimpleString& name, long int value) override;
    MockActualCall& withUnsignedLongIntParameter(const SimpleString& name, unsigned long int value) override;
    MockActualCall& withLongLongIntParameter(const SimpleString& name, long long value) override;
    MockActualCall& withUnsignedLongLongIntParameter(const SimpleString& name, unsigned long long value) override;
    MockActualCall& withDoubleParameter(const SimpleString& name, double value) override;
    MockActualCall& withStringParameter(const SimpleString& name, const char* value) override;
    MockActualCall& withPointerParameter(const SimpleString& name, void* value) override;
    MockActualCall& withConstPointerParameter(const SimpleString& name, const void* value) override;
    MockActualCall& withFunctionPointerParameter(const SimpleString& name, void (*value)()) override;
    MockActualCall& withMemoryBufferParameter(const SimpleString& name, const unsigned char* value, size_t size) override;
    MockActualCall& withParameterOfType(const SimpleString& type, const SimpleString& name, const void* value) override;
    MockActualCall& withOutputParameter(const SimpleString& name, void* output) override;
    MockActualCall& withOutputParameterOfType(const SimpleString& type, const SimpleString& name, void* output) override;

    bool hasReturnValue() override;
    MockNamedValue returnValue() override;

    bool returnBoolValueOrDefault(bool default_value) override;
    bool returnBoolValue() override;

    int returnIntValueOrDefault(int default_value) override;
    int returnIntValue() override;

    unsigned long int returnUnsignedLongIntValue() override;
    unsigned long int returnUnsignedLongIntValueOrDefault(unsigned long int) override;

    long int returnLongIntValue() override;
    long int returnLongIntValueOrDefault(long int default_value) override;

    unsigned long long returnUnsignedLongLongIntValue() override;
    unsigned long long returnUnsignedLongLongIntValueOrDefault(unsigned long long default_value) override;

    long long returnLongLongIntValue() override;
    long long returnLongLongIntValueOrDefault(long long default_value) override;

    unsigned int returnUnsignedIntValue() override;
    unsigned int returnUnsignedIntValueOrDefault(unsigned int default_value) override;

    const char * returnStringValueOrDefault(const char * default_value) override;
    const char * returnStringValue() override;

    double returnDoubleValue() override;
    double returnDoubleValueOrDefault(double default_value) override;

    const void * returnConstPointerValue() override;
    const void * returnConstPointerValueOrDefault(const void * default_value) override;

    void * returnPointerValue() override;
    void * returnPointerValueOrDefault(void *) override;

    FunctionPointer returnFunctionPointerValue() override;
    FunctionPointer returnFunctionPointerValueOrDefault(FunctionPointer default_value) override;

    MockActualCall& onObject(const void* objectPtr) override;

    virtual bool isFulfilled() const;
    virtual bool hasFailed() const;

    virtual void checkExpectations();

    virtual void setMockFailureReporter(MockFailureReporter* reporter);
protected:
    void setName(const SimpleString& name);
    SimpleString getName() const;
    virtual UtestShell* getTest() const;
    virtual void callHasSucceeded();
    virtual void copyOutputParameters(MockCheckedExpectedCall* call);
    virtual void completeCallWhenMatchIsFound();
    virtual void failTest(const MockFailure& failure);
    virtual void checkInputParameter(const MockNamedValue& actualParameter);
    virtual void checkOutputParameter(const MockNamedValue& outputParameter);
    virtual void discardCurrentlyMatchingExpectations();

    enum ActualCallState {
        CALL_IN_PROGRESS,
        CALL_FAILED,
        CALL_SUCCEED
    };
    virtual void setState(ActualCallState state);

private:
    SimpleString functionName_;
    unsigned int callOrder_;
    MockFailureReporter* reporter_;

    ActualCallState state_;
    bool expectationsChecked_;
    MockCheckedExpectedCall* matchingExpectation_;

    MockExpectedCallsList potentiallyMatchingExpectations_;
    const MockExpectedCallsList& allExpectations_;

    class MockOutputParametersListNode
    {
    public:
        SimpleString name_;
        SimpleString type_;
        void* ptr_;

        MockOutputParametersListNode* next_;
        MockOutputParametersListNode(const SimpleString& name, const SimpleString& type, void* ptr)
            : name_(name), type_(type), ptr_(ptr), next_(nullptr) {}
    };

    MockOutputParametersListNode* outputParameterExpectations_;

    virtual void addOutputParameter(const SimpleString& name, const SimpleString& type, void* ptr);
    virtual void cleanUpOutputParameterList();
};

class MockActualCallTrace : public MockActualCall
{
public:
    MockActualCallTrace();
    ~MockActualCallTrace() override;

    MockActualCall& withName(const SimpleString& name) override;
    MockActualCall& withCallOrder(unsigned int) override;
    MockActualCall& withBoolParameter(const SimpleString& name, bool value) override;
    MockActualCall& withIntParameter(const SimpleString& name, int value) override;
    MockActualCall& withUnsignedIntParameter(const SimpleString& name, unsigned int value) override;
    MockActualCall& withLongIntParameter(const SimpleString& name, long int value) override;
    MockActualCall& withUnsignedLongIntParameter(const SimpleString& name, unsigned long int value) override;
    MockActualCall& withLongLongIntParameter(const SimpleString& name, long long value) override;
    MockActualCall& withUnsignedLongLongIntParameter(const SimpleString& name, unsigned long long value) override;
    MockActualCall& withDoubleParameter(const SimpleString& name, double value) override;
    MockActualCall& withStringParameter(const SimpleString& name, const char* value) override;
    MockActualCall& withPointerParameter(const SimpleString& name, void* value) override;
    MockActualCall& withConstPointerParameter(const SimpleString& name, const void* value) override;
    MockActualCall& withFunctionPointerParameter(const SimpleString& name, void (*value)()) override;
    MockActualCall& withMemoryBufferParameter(const SimpleString& name, const unsigned char* value, size_t size) override;
    MockActualCall& withParameterOfType(const SimpleString& typeName, const SimpleString& name, const void* value) override;
    MockActualCall& withOutputParameter(const SimpleString& name, void* output) override;
    MockActualCall& withOutputParameterOfType(const SimpleString& typeName, const SimpleString& name, void* output) override;

    bool hasReturnValue() override;
    MockNamedValue returnValue() override;

    bool returnBoolValueOrDefault(bool default_value) override;
    bool returnBoolValue() override;

    int returnIntValueOrDefault(int default_value) override;
    int returnIntValue() override;

    unsigned long int returnUnsignedLongIntValue() override;
    unsigned long int returnUnsignedLongIntValueOrDefault(unsigned long int) override;

    long int returnLongIntValue() override;
    long int returnLongIntValueOrDefault(long int default_value) override;

    unsigned long long returnUnsignedLongLongIntValue() override;
    unsigned long long returnUnsignedLongLongIntValueOrDefault(unsigned long long default_value) override;

    long long returnLongLongIntValue() override;
    long long returnLongLongIntValueOrDefault(long long default_value) override;

    unsigned int returnUnsignedIntValue() override;
    unsigned int returnUnsignedIntValueOrDefault(unsigned int default_value) override;

    const char * returnStringValueOrDefault(const char * default_value) override;
    const char * returnStringValue() override;

    double returnDoubleValue() override;
    double returnDoubleValueOrDefault(double default_value) override;

    void * returnPointerValue() override;
    void * returnPointerValueOrDefault(void *) override;

    const void * returnConstPointerValue() override;
    const void * returnConstPointerValueOrDefault(const void * default_value) override;

    FunctionPointer returnFunctionPointerValue() override;
    FunctionPointer returnFunctionPointerValueOrDefault(FunctionPointer default_value) override;

    MockActualCall& onObject(const void* objectPtr) override;

    const char* getTraceOutput();
    void clear();
    static MockActualCallTrace& instance();
    static void clearInstance();

private:
    SimpleString traceBuffer_;

    static MockActualCallTrace* instance_;

    void addParameterName(const SimpleString& name);
};

class MockIgnoredActualCall: public MockActualCall
{
public:
    MockActualCall& withName(const SimpleString&) override { return *this;}
    MockActualCall& withCallOrder(unsigned int) override { return *this; }
    MockActualCall& withBoolParameter(const SimpleString&, bool) override { return *this; }
    MockActualCall& withIntParameter(const SimpleString&, int) override { return *this; }
    MockActualCall& withUnsignedIntParameter(const SimpleString&, unsigned int) override { return *this; }
    MockActualCall& withLongIntParameter(const SimpleString&, long int) override { return *this; }
    MockActualCall& withUnsignedLongIntParameter(const SimpleString&, unsigned long int) override { return *this; }
    MockActualCall& withLongLongIntParameter(const SimpleString&, long long) override { return *this; }
    MockActualCall& withUnsignedLongLongIntParameter(const SimpleString&, unsigned long long) override { return *this; }
    MockActualCall& withDoubleParameter(const SimpleString&, double) override { return *this; }
    MockActualCall& withStringParameter(const SimpleString&, const char*) override { return *this; }
    MockActualCall& withPointerParameter(const SimpleString& , void*) override { return *this; }
    MockActualCall& withConstPointerParameter(const SimpleString& , const void*) override { return *this; }
    MockActualCall& withFunctionPointerParameter(const SimpleString& , void (*)()) override { return *this; }
    MockActualCall& withMemoryBufferParameter(const SimpleString&, const unsigned char*, size_t) override  { return *this; }
    MockActualCall& withParameterOfType(const SimpleString&, const SimpleString&, const void*) override { return *this; }
    MockActualCall& withOutputParameter(const SimpleString&, void*) override { return *this; }
    MockActualCall& withOutputParameterOfType(const SimpleString&, const SimpleString&, void*) override { return *this; }

    bool hasReturnValue() override { return false; }
    MockNamedValue returnValue() override { return MockNamedValue(""); }

    bool returnBoolValueOrDefault(bool value) override { return value; }
    bool returnBoolValue() override { return false; }

    int returnIntValue() override { return 0; }
    int returnIntValueOrDefault(int value) override { return value; }

    unsigned long int returnUnsignedLongIntValue() override { return 0; }
    unsigned long int returnUnsignedLongIntValueOrDefault(unsigned long int value) override { return value; }

    long int returnLongIntValue() override { return 0; }
    long int returnLongIntValueOrDefault(long int value) override { return value; }

    unsigned long long returnUnsignedLongLongIntValue() override { return 0; }
    unsigned long long returnUnsignedLongLongIntValueOrDefault(unsigned long long value) override { return value; }

    long long returnLongLongIntValue() override { return 0; }
    long long returnLongLongIntValueOrDefault(long long value) override { return value; }

    unsigned int returnUnsignedIntValue() override { return 0; }
    unsigned int returnUnsignedIntValueOrDefault(unsigned int value) override { return value; }

    double returnDoubleValue() override { return 0.0; }
    double returnDoubleValueOrDefault(double value) override { return value; }

    const char * returnStringValue() override { return ""; }
    const char * returnStringValueOrDefault(const char * value) override { return value; }

    void * returnPointerValue() override { return nullptr; }
    void * returnPointerValueOrDefault(void * value) override { return value; }

    const void * returnConstPointerValue() override { return nullptr; }
    const void * returnConstPointerValueOrDefault(const void * value) override { return value; }

    FunctionPointer returnFunctionPointerValue() override { return nullptr; }
    FunctionPointer returnFunctionPointerValueOrDefault(FunctionPointer value) override { return value; }

    MockActualCall& onObject(const void* ) override { return *this; }

    static MockIgnoredActualCall& instance();
};

#endif
