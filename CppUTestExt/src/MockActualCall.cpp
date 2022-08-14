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

#include "CppUTestExt/MockCheckedActualCall.hpp"
#include "CppUTestExt/MockCheckedExpectedCall.hpp"
#include "CppUTestExt/MockFailure.hpp"

#include "CppUTest/PlatformSpecificFunctions.hpp"
#include "CppUTest/TestHarness.hpp"

MockActualCall::MockActualCall()
{
}

MockActualCall::~MockActualCall()
{
}

void MockCheckedActualCall::setName(const std::string& name)
{
    functionName_ = name;
}

std::string MockCheckedActualCall::getName() const
{
    return functionName_;
}

MockCheckedActualCall::MockCheckedActualCall(unsigned int callOrder, MockFailureReporter* reporter, const MockExpectedCallsList& allExpectations)
    : callOrder_(callOrder)
    , reporter_(reporter)
    , state_(CALL_SUCCEED)
    , expectationsChecked_(false)
    , matchingExpectation_(nullptr)
    , allExpectations_(allExpectations)
    , outputParameterExpectations_(nullptr)
{
    potentiallyMatchingExpectations_.addPotentiallyMatchingExpectations(allExpectations);
}

MockCheckedActualCall::~MockCheckedActualCall()
{
    cleanUpOutputParameterList();
}

void MockCheckedActualCall::setMockFailureReporter(MockFailureReporter* reporter)
{
    reporter_ = reporter;
}

UtestShell* MockCheckedActualCall::getTest() const
{
    return reporter_->getTestToFail();
}

void MockCheckedActualCall::failTest(const MockFailure& failure)
{
    if (!hasFailed()) {
        setState(CALL_FAILED);
        reporter_->failTest(failure);
    }
}

void MockCheckedActualCall::copyOutputParameters(MockCheckedExpectedCall* expectedCall)
{
    for (MockOutputParametersListNode* p = outputParameterExpectations_; p; p = p->next_) {
        MockNamedValue outputParameter = expectedCall->getOutputParameter(p->name_);
        MockNamedValueCopier* copier = outputParameter.getCopier();
        if (copier) {
            copier->copy(p->ptr_, outputParameter.getConstObjectPointer());
        } else if ((outputParameter.getType() == "const void*") && (p->type_ == "void*")) {
            const void* data = outputParameter.getConstPointerValue();
            size_t size = outputParameter.getSize();
            PlatformSpecificMemCpy(p->ptr_, data, size);
        } else if (outputParameter.getName() != "") {
            std::string type = expectedCall->getOutputParameter(p->name_).getType();
            MockNoWayToCopyCustomTypeFailure failure(getTest(), type);
            failTest(failure);
        }
    }
}

void MockCheckedActualCall::completeCallWhenMatchIsFound()
{
    // Expectations that don't ignore parameters have higher fulfillment preference than those that ignore parameters

    matchingExpectation_ = potentiallyMatchingExpectations_.removeFirstFinalizedMatchingExpectation();
    if (matchingExpectation_) {
        copyOutputParameters(matchingExpectation_);
        callHasSucceeded();
    } else {
        MockCheckedExpectedCall* matchingExpectationWithIgnoredParameters = potentiallyMatchingExpectations_.getFirstMatchingExpectation();
        if (matchingExpectationWithIgnoredParameters) {
            copyOutputParameters(matchingExpectationWithIgnoredParameters);
        }
    }
}

void MockCheckedActualCall::callHasSucceeded()
{
    setState(CALL_SUCCEED);
}

void MockCheckedActualCall::discardCurrentlyMatchingExpectations()
{
    if (matchingExpectation_) {
        matchingExpectation_->resetActualCallMatchingState();
        matchingExpectation_ = nullptr;
    }
    potentiallyMatchingExpectations_.onlyKeepUnmatchingExpectations();
}

MockActualCall& MockCheckedActualCall::withName(const std::string& name)
{
    setName(name);
    setState(CALL_IN_PROGRESS);

    potentiallyMatchingExpectations_.onlyKeepExpectationsRelatedTo(name);
    if (potentiallyMatchingExpectations_.isEmpty()) {
        MockUnexpectedCallHappenedFailure failure(getTest(), name, allExpectations_);
        failTest(failure);
        return *this;
    }

    completeCallWhenMatchIsFound();

    return *this;
}

MockActualCall& MockCheckedActualCall::withCallOrder(unsigned int)
{
    return *this;
}

void MockCheckedActualCall::checkInputParameter(const MockNamedValue& actualParameter)
{
    if (hasFailed()) {
        return;
    }

    setState(CALL_IN_PROGRESS);
    discardCurrentlyMatchingExpectations();

    potentiallyMatchingExpectations_.onlyKeepExpectationsWithInputParameter(actualParameter);

    if (potentiallyMatchingExpectations_.isEmpty()) {
        MockUnexpectedInputParameterFailure failure(getTest(), getName(), actualParameter, allExpectations_);
        failTest(failure);
        return;
    }

    potentiallyMatchingExpectations_.parameterWasPassed(actualParameter.getName());
    completeCallWhenMatchIsFound();
}

void MockCheckedActualCall::checkOutputParameter(const MockNamedValue& outputParameter)
{
    if (hasFailed()) {
        return;
    }

    setState(CALL_IN_PROGRESS);
    discardCurrentlyMatchingExpectations();

    potentiallyMatchingExpectations_.onlyKeepExpectationsWithOutputParameter(outputParameter);

    if (potentiallyMatchingExpectations_.isEmpty()) {
        MockUnexpectedOutputParameterFailure failure(getTest(), getName(), outputParameter, allExpectations_);
        failTest(failure);
        return;
    }

    potentiallyMatchingExpectations_.outputParameterWasPassed(outputParameter.getName());
    completeCallWhenMatchIsFound();
}

MockActualCall& MockCheckedActualCall::withBoolParameter(const std::string& name, bool value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withUnsignedIntParameter(const std::string& name, unsigned int value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withIntParameter(const std::string& name, int value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withUnsignedLongIntParameter(const std::string& name, unsigned long int value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withLongIntParameter(const std::string& name, long int value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withUnsignedLongLongIntParameter(const std::string& name, unsigned long long value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withLongLongIntParameter(const std::string& name, long long value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withDoubleParameter(const std::string& name, double value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withStringParameter(const std::string& name, const char* value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withPointerParameter(const std::string& name, void* value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withConstPointerParameter(const std::string& name, const void* value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withFunctionPointerParameter(const std::string& name, void (*value)())
{
    MockNamedValue actualParameter(name);
    actualParameter.setValue(value);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withMemoryBufferParameter(const std::string& name, const unsigned char* value, size_t size)
{
    MockNamedValue actualParameter(name);
    actualParameter.setMemoryBuffer(value, size);
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withParameterOfType(const std::string& type, const std::string& name, const void* value)
{
    MockNamedValue actualParameter(name);
    actualParameter.setConstObjectPointer(type, value);

    if (actualParameter.getComparator() == nullptr) {
        MockNoWayToCompareCustomTypeFailure failure(getTest(), type);
        failTest(failure);
        return *this;
    }
    checkInputParameter(actualParameter);
    return *this;
}

MockActualCall& MockCheckedActualCall::withOutputParameter(const std::string& name, void* output)
{
    addOutputParameter(name, "void*", output);

    MockNamedValue outputParameter(name);
    outputParameter.setValue(output);
    checkOutputParameter(outputParameter);

    return *this;
}

MockActualCall& MockCheckedActualCall::withOutputParameterOfType(const std::string& type, const std::string& name, void* output)
{
    addOutputParameter(name, type, output);

    MockNamedValue outputParameter(name);
    outputParameter.setConstObjectPointer(type, output);
    checkOutputParameter(outputParameter);

    return *this;
}

bool MockCheckedActualCall::isFulfilled() const
{
    return state_ == CALL_SUCCEED;
}

bool MockCheckedActualCall::hasFailed() const
{
    return state_ == CALL_FAILED;
}

void MockCheckedActualCall::checkExpectations()
{
    if (expectationsChecked_) {
        return;
    }

    expectationsChecked_ = true;

    if (state_ != CALL_IN_PROGRESS) {
        if (state_ == CALL_SUCCEED) {
            matchingExpectation_->callWasMade(callOrder_);
        }
        potentiallyMatchingExpectations_.resetActualCallMatchingState();
        return;
    }

    if (potentiallyMatchingExpectations_.hasFinalizedMatchingExpectations())
        FAIL("Actual call is in progress, but there are finalized matching expectations when checking expectations. This cannot happen."); // LCOV_EXCL_LINE

    matchingExpectation_ = potentiallyMatchingExpectations_.removeFirstMatchingExpectation();
    if (matchingExpectation_) {
        matchingExpectation_->finalizeActualCallMatch();
        callHasSucceeded();
        matchingExpectation_->callWasMade(callOrder_);
        potentiallyMatchingExpectations_.resetActualCallMatchingState();
        return;
    }

    if (potentiallyMatchingExpectations_.hasUnmatchingExpectationsBecauseOfMissingParameters()) {
        MockExpectedParameterDidntHappenFailure failure(getTest(), getName(), allExpectations_, potentiallyMatchingExpectations_);
        failTest(failure);
    } else {
        MockExpectedObjectDidntHappenFailure failure(getTest(), getName(), allExpectations_);
        failTest(failure);
    }
}

void MockCheckedActualCall::setState(ActualCallState state)
{
    state_ = state;
}

MockNamedValue MockCheckedActualCall::returnValue()
{
    checkExpectations();
    if (matchingExpectation_)
        return matchingExpectation_->returnValue();
    return MockNamedValue("no return value");
}

bool MockCheckedActualCall::returnBoolValueOrDefault(bool default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnBoolValue();
}

bool MockCheckedActualCall::returnBoolValue()
{
    return returnValue().getBoolValue();
}

int MockCheckedActualCall::returnIntValueOrDefault(int default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnIntValue();
}

int MockCheckedActualCall::returnIntValue()
{
    return returnValue().getIntValue();
}

unsigned long int MockCheckedActualCall::returnUnsignedLongIntValue()
{
    return returnValue().getUnsignedLongIntValue();
}

unsigned long int MockCheckedActualCall::returnUnsignedLongIntValueOrDefault(unsigned long int default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnUnsignedLongIntValue();
}

long int MockCheckedActualCall::returnLongIntValue()
{
    return returnValue().getLongIntValue();
}

long int MockCheckedActualCall::returnLongIntValueOrDefault(long int default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnLongIntValue();
}

unsigned long long MockCheckedActualCall::returnUnsignedLongLongIntValue()
{
    return returnValue().getUnsignedLongLongIntValue();
}

unsigned long long MockCheckedActualCall::returnUnsignedLongLongIntValueOrDefault(unsigned long long default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnUnsignedLongLongIntValue();
}

long long MockCheckedActualCall::returnLongLongIntValue()
{
    return returnValue().getLongLongIntValue();
}

long long MockCheckedActualCall::returnLongLongIntValueOrDefault(long long default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnLongLongIntValue();
}

double MockCheckedActualCall::returnDoubleValue()
{
    return returnValue().getDoubleValue();
}

double MockCheckedActualCall::returnDoubleValueOrDefault(double default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnDoubleValue();
}

unsigned int MockCheckedActualCall::returnUnsignedIntValue()
{
    return returnValue().getUnsignedIntValue();
}

unsigned int MockCheckedActualCall::returnUnsignedIntValueOrDefault(unsigned int default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnUnsignedIntValue();
}

void* MockCheckedActualCall::returnPointerValueOrDefault(void* default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnPointerValue();
}

void* MockCheckedActualCall::returnPointerValue()
{
    return returnValue().getPointerValue();
}

const void* MockCheckedActualCall::returnConstPointerValue()
{
    return returnValue().getConstPointerValue();
}

const void* MockCheckedActualCall::returnConstPointerValueOrDefault(const void* default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnConstPointerValue();
}

MockActualCall::FunctionPointer MockCheckedActualCall::returnFunctionPointerValue()
{
    return returnValue().getFunctionPointerValue();
}

MockActualCall::FunctionPointer MockCheckedActualCall::returnFunctionPointerValueOrDefault(FunctionPointer default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnFunctionPointerValue();
}

const char* MockCheckedActualCall::returnStringValueOrDefault(const char* default_value)
{
    if (!hasReturnValue()) {
        return default_value;
    }
    return returnStringValue();
}

const char* MockCheckedActualCall::returnStringValue()
{
    return returnValue().getStringValue();
}

bool MockCheckedActualCall::hasReturnValue()
{
    return !returnValue().getName().empty();
}

MockActualCall& MockCheckedActualCall::onObject(const void* objectPtr)
{
    if (hasFailed()) {
        return *this;
    }

    // Currently matching expectations are not discarded because the passed object
    // is ignored if not specifically set in the expectation

    potentiallyMatchingExpectations_.onlyKeepExpectationsOnObject(objectPtr);

    if ((!matchingExpectation_) && potentiallyMatchingExpectations_.isEmpty()) {
        MockUnexpectedObjectFailure failure(getTest(), getName(), objectPtr, allExpectations_);
        failTest(failure);
        return *this;
    }

    potentiallyMatchingExpectations_.wasPassedToObject();

    if (!matchingExpectation_) {
        completeCallWhenMatchIsFound();
    }

    return *this;
}

void MockCheckedActualCall::addOutputParameter(const std::string& name, const std::string& type, void* ptr)
{
    MockOutputParametersListNode* newNode = new MockOutputParametersListNode(name, type, ptr);

    if (outputParameterExpectations_ == nullptr)
        outputParameterExpectations_ = newNode;
    else {
        MockOutputParametersListNode* lastNode = outputParameterExpectations_;
        while (lastNode->next_)
            lastNode = lastNode->next_;
        lastNode->next_ = newNode;
    }
}

void MockCheckedActualCall::cleanUpOutputParameterList()
{
    MockOutputParametersListNode* current = outputParameterExpectations_;
    MockOutputParametersListNode* toBeDeleted = nullptr;

    while (current) {
        toBeDeleted = current;
        outputParameterExpectations_ = current = current->next_;
        delete toBeDeleted;
    }
}

MockActualCallTrace::MockActualCallTrace()
{
}

MockActualCallTrace::~MockActualCallTrace()
{
}

MockActualCall& MockActualCallTrace::withName(const std::string& name)
{
    traceBuffer_ += "\nFunction name:";
    traceBuffer_ += name;
    return *this;
}

MockActualCall& MockActualCallTrace::withCallOrder(unsigned int callOrder)
{
    traceBuffer_ += " withCallOrder:";
    traceBuffer_ += StringFrom(callOrder);
    return *this;
}

void MockActualCallTrace::addParameterName(const std::string& name)
{
    traceBuffer_ += " ";
    traceBuffer_ += name;
    traceBuffer_ += ":";
}

MockActualCall& MockActualCallTrace::withBoolParameter(const std::string& name, bool value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withUnsignedIntParameter(const std::string& name, unsigned int value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value) + " " + BracketsFormattedHexStringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withIntParameter(const std::string& name, int value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value) + " " + BracketsFormattedHexStringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withUnsignedLongIntParameter(const std::string& name, unsigned long int value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value) + " " + BracketsFormattedHexStringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withLongIntParameter(const std::string& name, long int value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value) + " " + BracketsFormattedHexStringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withUnsignedLongLongIntParameter(const std::string& name, unsigned long long value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value) + " " + BracketsFormattedHexStringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withLongLongIntParameter(const std::string& name, long long value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value) + " " + BracketsFormattedHexStringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withDoubleParameter(const std::string& name, double value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withStringParameter(const std::string& name, const char* value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withPointerParameter(const std::string& name, void* value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withConstPointerParameter(const std::string& name, const void* value)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withFunctionPointerParameter(const std::string& name, void (*value)())
{
    addParameterName(name);
    traceBuffer_ += StringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withMemoryBufferParameter(const std::string& name, const unsigned char* value, size_t size)
{
    addParameterName(name);
    traceBuffer_ += StringFromBinaryWithSizeOrNull(value, size);
    return *this;
}

MockActualCall& MockActualCallTrace::withParameterOfType(const std::string& typeName, const std::string& name, const void* value)
{
    traceBuffer_ += " ";
    traceBuffer_ += typeName;
    addParameterName(name);
    traceBuffer_ += StringFrom(value);
    return *this;
}

MockActualCall& MockActualCallTrace::withOutputParameter(const std::string& name, void* output)
{
    addParameterName(name);
    traceBuffer_ += StringFrom(output);
    return *this;
}

MockActualCall& MockActualCallTrace::withOutputParameterOfType(const std::string& typeName, const std::string& name, void* output)
{
    traceBuffer_ += " ";
    traceBuffer_ += typeName;
    addParameterName(name);
    traceBuffer_ += StringFrom(output);
    return *this;
}

bool MockActualCallTrace::hasReturnValue()
{
    return false;
}

MockNamedValue MockActualCallTrace::returnValue()
{
    return MockNamedValue("");
}

long int MockActualCallTrace::returnLongIntValue()
{
    return 0;
}

unsigned long int MockActualCallTrace::returnUnsignedLongIntValue()
{
    return 0;
}

unsigned long int MockActualCallTrace::returnUnsignedLongIntValueOrDefault(unsigned long)
{
    return 0;
}

long int MockActualCallTrace::returnLongIntValueOrDefault(long int)
{
    return 0;
}

long long MockActualCallTrace::returnLongLongIntValue()
{
    return 0;
}

unsigned long long MockActualCallTrace::returnUnsignedLongLongIntValue()
{
    return 0;
}

unsigned long long MockActualCallTrace::returnUnsignedLongLongIntValueOrDefault(unsigned long long)
{
    return 0;
}

long long MockActualCallTrace::returnLongLongIntValueOrDefault(long long)
{
    return 0;
}

bool MockActualCallTrace::returnBoolValue()
{
    return false;
}

bool MockActualCallTrace::returnBoolValueOrDefault(bool)
{
    return false;
}

int MockActualCallTrace::returnIntValue()
{
    return 0;
}

double MockActualCallTrace::returnDoubleValue()
{
    return 0.0;
}

double MockActualCallTrace::returnDoubleValueOrDefault(double)
{
    return returnDoubleValue();
}

unsigned int MockActualCallTrace::returnUnsignedIntValue()
{
    return 0;
}

void* MockActualCallTrace::returnPointerValue()
{
    return nullptr;
}

const void* MockActualCallTrace::returnConstPointerValue()
{
    return nullptr;
}

MockActualCall::FunctionPointer MockActualCallTrace::returnFunctionPointerValue()
{
    return nullptr;
}

const void* MockActualCallTrace::returnConstPointerValueOrDefault(const void*)
{
    return returnConstPointerValue();
}

void* MockActualCallTrace::returnPointerValueOrDefault(void*)
{
    return returnPointerValue();
}

MockActualCall::FunctionPointer MockActualCallTrace::returnFunctionPointerValueOrDefault(FunctionPointer)
{
    return returnFunctionPointerValue();
}

const char* MockActualCallTrace::returnStringValue()
{
    return "";
}

const char* MockActualCallTrace::returnStringValueOrDefault(const char*)
{
    return returnStringValue();
}

int MockActualCallTrace::returnIntValueOrDefault(int)
{
    return 0;
}

unsigned int MockActualCallTrace::returnUnsignedIntValueOrDefault(unsigned int)
{
    return returnUnsignedIntValue();
}

MockActualCall& MockActualCallTrace::onObject(const void* objectPtr)
{
    traceBuffer_ += " onObject:";
    traceBuffer_ += StringFrom(objectPtr);
    return *this;
}

void MockActualCallTrace::clear()
{
    traceBuffer_ = "";
}

const char* MockActualCallTrace::getTraceOutput()
{
    return traceBuffer_.c_str();
}

MockActualCallTrace* MockActualCallTrace::instance_ = nullptr;

MockActualCallTrace& MockActualCallTrace::instance()
{
    if (instance_ == nullptr)
        instance_ = new MockActualCallTrace;
    return *instance_;
}

void MockActualCallTrace::clearInstance()
{
    delete instance_;
    instance_ = nullptr;
}

MockIgnoredActualCall& MockIgnoredActualCall::instance()
{
    static MockIgnoredActualCall call;
    return call;
}
