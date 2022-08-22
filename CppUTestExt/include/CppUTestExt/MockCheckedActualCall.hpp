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

#ifndef INCLUDED_CPPUTESTEXT_MOCKCHECKEDACTUALCALL_HPP
#define INCLUDED_CPPUTESTEXT_MOCKCHECKEDACTUALCALL_HPP

#include "CppUTestExt/MockActualCall.hpp"
#include "CppUTestExt/MockExpectedCallsList.hpp"

#include "CppUTest/Utest.hpp"

namespace cpputest {
namespace extensions {

    class MockCheckedActualCall : public MockActualCall {
    public:
        MockCheckedActualCall(unsigned int callOrder, MockFailureReporter* reporter, const MockExpectedCallsList& expectations);
        ~MockCheckedActualCall() override;

        MockActualCall& withName(const std::string& name) override;
        MockActualCall& withCallOrder(unsigned int) override;
        MockActualCall& withBoolParameter(const std::string& name, bool value) override;
        MockActualCall& withIntParameter(const std::string& name, int value) override;
        MockActualCall& withUnsignedIntParameter(const std::string& name, unsigned int value) override;
        MockActualCall& withLongIntParameter(const std::string& name, long int value) override;
        MockActualCall& withUnsignedLongIntParameter(const std::string& name, unsigned long int value) override;
        MockActualCall& withLongLongIntParameter(const std::string& name, long long value) override;
        MockActualCall& withUnsignedLongLongIntParameter(const std::string& name, unsigned long long value) override;
        MockActualCall& withDoubleParameter(const std::string& name, double value) override;
        MockActualCall& withStringParameter(const std::string& name, const char* value) override;
        MockActualCall& withPointerParameter(const std::string& name, void* value) override;
        MockActualCall& withConstPointerParameter(const std::string& name, const void* value) override;
        MockActualCall& withFunctionPointerParameter(const std::string& name, void (*value)()) override;
        MockActualCall& withMemoryBufferParameter(const std::string& name, const unsigned char* value, size_t size) override;
        MockActualCall& withParameterOfType(const std::string& type, const std::string& name, const void* value) override;
        MockActualCall& withOutputParameter(const std::string& name, void* output) override;
        MockActualCall& withOutputParameterOfType(const std::string& type, const std::string& name, void* output) override;

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

        const char* returnStringValueOrDefault(const char* default_value) override;
        const char* returnStringValue() override;

        double returnDoubleValue() override;
        double returnDoubleValueOrDefault(double default_value) override;

        const void* returnConstPointerValue() override;
        const void* returnConstPointerValueOrDefault(const void* default_value) override;

        void* returnPointerValue() override;
        void* returnPointerValueOrDefault(void*) override;

        FunctionPointer returnFunctionPointerValue() override;
        FunctionPointer returnFunctionPointerValueOrDefault(FunctionPointer default_value) override;

        MockActualCall& onObject(const void* objectPtr) override;

        virtual bool isFulfilled() const;
        virtual bool hasFailed() const;

        virtual void checkExpectations();

        virtual void setMockFailureReporter(MockFailureReporter* reporter);

    protected:
        void setName(const std::string& name);
        std::string getName() const;
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
        std::string functionName_;
        unsigned int callOrder_;
        MockFailureReporter* reporter_;

        ActualCallState state_;
        bool expectationsChecked_;
        MockCheckedExpectedCall* matchingExpectation_;

        MockExpectedCallsList potentiallyMatchingExpectations_;
        const MockExpectedCallsList& allExpectations_;

        class MockOutputParametersListNode {
        public:
            std::string name_;
            std::string type_;
            void* ptr_;

            MockOutputParametersListNode* next_;
            MockOutputParametersListNode(const std::string& name, const std::string& type, void* ptr)
                : name_(name)
                , type_(type)
                , ptr_(ptr)
                , next_(nullptr)
            {
            }
        };

        MockOutputParametersListNode* outputParameterExpectations_;

        virtual void addOutputParameter(const std::string& name, const std::string& type, void* ptr);
        virtual void cleanUpOutputParameterList();
    };

    class MockActualCallTrace : public MockActualCall {
    public:
        MockActualCallTrace();
        ~MockActualCallTrace() override;

        MockActualCall& withName(const std::string& name) override;
        MockActualCall& withCallOrder(unsigned int) override;
        MockActualCall& withBoolParameter(const std::string& name, bool value) override;
        MockActualCall& withIntParameter(const std::string& name, int value) override;
        MockActualCall& withUnsignedIntParameter(const std::string& name, unsigned int value) override;
        MockActualCall& withLongIntParameter(const std::string& name, long int value) override;
        MockActualCall& withUnsignedLongIntParameter(const std::string& name, unsigned long int value) override;
        MockActualCall& withLongLongIntParameter(const std::string& name, long long value) override;
        MockActualCall& withUnsignedLongLongIntParameter(const std::string& name, unsigned long long value) override;
        MockActualCall& withDoubleParameter(const std::string& name, double value) override;
        MockActualCall& withStringParameter(const std::string& name, const char* value) override;
        MockActualCall& withPointerParameter(const std::string& name, void* value) override;
        MockActualCall& withConstPointerParameter(const std::string& name, const void* value) override;
        MockActualCall& withFunctionPointerParameter(const std::string& name, void (*value)()) override;
        MockActualCall& withMemoryBufferParameter(const std::string& name, const unsigned char* value, size_t size) override;
        MockActualCall& withParameterOfType(const std::string& typeName, const std::string& name, const void* value) override;
        MockActualCall& withOutputParameter(const std::string& name, void* output) override;
        MockActualCall& withOutputParameterOfType(const std::string& typeName, const std::string& name, void* output) override;

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

        const char* returnStringValueOrDefault(const char* default_value) override;
        const char* returnStringValue() override;

        double returnDoubleValue() override;
        double returnDoubleValueOrDefault(double default_value) override;

        void* returnPointerValue() override;
        void* returnPointerValueOrDefault(void*) override;

        const void* returnConstPointerValue() override;
        const void* returnConstPointerValueOrDefault(const void* default_value) override;

        FunctionPointer returnFunctionPointerValue() override;
        FunctionPointer returnFunctionPointerValueOrDefault(FunctionPointer default_value) override;

        MockActualCall& onObject(const void* objectPtr) override;

        const char* getTraceOutput();
        void clear();
        static MockActualCallTrace& instance();
        static void clearInstance();

    private:
        std::string traceBuffer_;

        static MockActualCallTrace* instance_;

        void addParameterName(const std::string& name);
    };

    class MockIgnoredActualCall : public MockActualCall {
    public:
        MockActualCall& withName(const std::string&) override { return *this; }
        MockActualCall& withCallOrder(unsigned int) override { return *this; }
        MockActualCall& withBoolParameter(const std::string&, bool) override { return *this; }
        MockActualCall& withIntParameter(const std::string&, int) override { return *this; }
        MockActualCall& withUnsignedIntParameter(const std::string&, unsigned int) override { return *this; }
        MockActualCall& withLongIntParameter(const std::string&, long int) override { return *this; }
        MockActualCall& withUnsignedLongIntParameter(const std::string&, unsigned long int) override { return *this; }
        MockActualCall& withLongLongIntParameter(const std::string&, long long) override { return *this; }
        MockActualCall& withUnsignedLongLongIntParameter(const std::string&, unsigned long long) override { return *this; }
        MockActualCall& withDoubleParameter(const std::string&, double) override { return *this; }
        MockActualCall& withStringParameter(const std::string&, const char*) override { return *this; }
        MockActualCall& withPointerParameter(const std::string&, void*) override { return *this; }
        MockActualCall& withConstPointerParameter(const std::string&, const void*) override { return *this; }
        MockActualCall& withFunctionPointerParameter(const std::string&, void (*)()) override { return *this; }
        MockActualCall& withMemoryBufferParameter(const std::string&, const unsigned char*, size_t) override { return *this; }
        MockActualCall& withParameterOfType(const std::string&, const std::string&, const void*) override { return *this; }
        MockActualCall& withOutputParameter(const std::string&, void*) override { return *this; }
        MockActualCall& withOutputParameterOfType(const std::string&, const std::string&, void*) override { return *this; }

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

        const char* returnStringValue() override { return ""; }
        const char* returnStringValueOrDefault(const char* value) override { return value; }

        void* returnPointerValue() override { return nullptr; }
        void* returnPointerValueOrDefault(void* value) override { return value; }

        const void* returnConstPointerValue() override { return nullptr; }
        const void* returnConstPointerValueOrDefault(const void* value) override { return value; }

        FunctionPointer returnFunctionPointerValue() override { return nullptr; }
        FunctionPointer returnFunctionPointerValueOrDefault(FunctionPointer value) override { return value; }

        MockActualCall& onObject(const void*) override { return *this; }

        static MockIgnoredActualCall& instance();
    };

}
}

#endif // INCLUDED_CPPUTESTEXT_MOCKCHECKEDACTUALCALL_HPP
