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

#include "CppUTestExt/CodeMemoryReportFormatter.hpp"

#include "CppUTest/PlatformSpecificFunctions.h"
#include "CppUTest/TestHarness.hpp"
#include "CppUTestExt/MemoryReportAllocator.hpp"

#include <cstring>
#include <string>

#define MAX_VARIABLE_NAME_LINE_PART 10
#define MAX_VARIABLE_NAME_FILE_PART 53
#define MAX_VARIABLE_NAME_SEPERATOR_PART 1
#define MAX_VARIABLE_NAME_LENGTH MAX_VARIABLE_NAME_FILE_PART + MAX_VARIABLE_NAME_SEPERATOR_PART + MAX_VARIABLE_NAME_LINE_PART

struct CodeReportingAllocationNode {
    char variableName_[MAX_VARIABLE_NAME_LENGTH + 1];
    void* memory_;
    CodeReportingAllocationNode* next_;
};

CodeMemoryReportFormatter::CodeMemoryReportFormatter(TestMemoryAllocator* internalAllocator)
    : codeReportingList_(nullptr)
    , internalAllocator_(internalAllocator)
{
}

CodeMemoryReportFormatter::~CodeMemoryReportFormatter()
{
    clearReporting();
}

void CodeMemoryReportFormatter::clearReporting()
{
    while (codeReportingList_) {
        CodeReportingAllocationNode* oldNode = codeReportingList_;
        codeReportingList_ = codeReportingList_->next_;
        internalAllocator_->free_memory((char*)oldNode, 0, __FILE__, __LINE__);
    }
}

void CodeMemoryReportFormatter::addNodeToList(const char* variableName, void* memory, CodeReportingAllocationNode* next)
{
    CodeReportingAllocationNode* newNode = (CodeReportingAllocationNode*)(void*)internalAllocator_->alloc_memory(sizeof(CodeReportingAllocationNode), __FILE__, __LINE__);
    newNode->memory_ = memory;
    newNode->next_ = next;
    std::strncpy(newNode->variableName_, variableName, MAX_VARIABLE_NAME_LENGTH);
    codeReportingList_ = newNode;
}

CodeReportingAllocationNode* CodeMemoryReportFormatter::findNode(void* memory)
{

    CodeReportingAllocationNode* current = codeReportingList_;
    while (current && current->memory_ != memory) {
        current = current->next_;
    }
    return current;
}

static std::string extractFileNameFromPath(const char* file)
{
    const char* fileNameOnly = file + std::strlen(file);
    while (fileNameOnly != file && *fileNameOnly != '/')
        fileNameOnly--;
    if (*fileNameOnly == '/')
        fileNameOnly++;
    return fileNameOnly;
}

std::string CodeMemoryReportFormatter::createVariableNameFromFileLineInfo(const char* file, size_t line)
{
    std::string fileNameOnly = extractFileNameFromPath(file);
    Strings::replace(fileNameOnly, ".", "_");

    for (int i = 1; i < 100; i++) {
        std::string variableName = StringFromFormat("%s_%d_%d", fileNameOnly.c_str(), (int)line, i);
        if (!variableExists(variableName))
            return variableName;
    }
    return "";
}

bool CodeMemoryReportFormatter::isNewAllocator(TestMemoryAllocator* allocator)
{
    return (
        std::strcmp(allocator->alloc_name(), defaultNewAllocator()->alloc_name()) == 0
        || std::strcmp(allocator->alloc_name(), defaultNewArrayAllocator()->alloc_name()) == 0);
}

bool CodeMemoryReportFormatter::variableExists(const std::string& variableName)
{
    CodeReportingAllocationNode* current = codeReportingList_;
    while (current) {
        if (variableName == current->variableName_)
            return true;
        current = current->next_;
    }
    return false;
}

std::string CodeMemoryReportFormatter::getAllocationString(TestMemoryAllocator* allocator, const std::string& variableName, size_t size)
{
    if (isNewAllocator(allocator))
        return StringFromFormat("char* %s = new char[%lu]; /* using %s */", variableName.c_str(), (unsigned long)size, allocator->alloc_name());
    else
        return StringFromFormat("void* %s = malloc(%lu);", variableName.c_str(), (unsigned long)size);
}

std::string CodeMemoryReportFormatter::getDeallocationString(TestMemoryAllocator* allocator, const std::string& variableName, const char* file, size_t line)
{
    if (isNewAllocator(allocator))
        return StringFromFormat("delete [] %s; /* using %s at %s:%d */", variableName.c_str(), allocator->free_name(), file, (int)line);
    else
        return StringFromFormat("free(%s); /* at %s:%d */", variableName.c_str(), file, (int)line);
}

void CodeMemoryReportFormatter::report_test_start(TestResult* result, UtestShell& test)
{
    clearReporting();
    result->print(StringFromFormat("*/\nTEST(%s_memoryReport, %s)\n{ /* at %s:%d */\n",
        test.getGroup().c_str(), test.getName().c_str(), test.getFile().c_str(), (int)test.getLineNumber())
                      .c_str());
}

void CodeMemoryReportFormatter::report_test_end(TestResult* result, UtestShell&)
{
    result->print("}/*");
}

void CodeMemoryReportFormatter::report_testgroup_start(TestResult* result, UtestShell& test)
{
    result->print(StringFromFormat("*/TEST_GROUP(%s_memoryReport)\n{\n};\n/*",
        test.getGroup().c_str())
                      .c_str());
}

void CodeMemoryReportFormatter::report_alloc_memory(TestResult* result, TestMemoryAllocator* allocator, size_t size, char* memory, const char* file, size_t line)
{
    std::string variableName = createVariableNameFromFileLineInfo(file, line);
    result->print(StringFromFormat("\t%s\n", getAllocationString(allocator, variableName, size).c_str()).c_str());
    addNodeToList(variableName.c_str(), memory, codeReportingList_);
}

void CodeMemoryReportFormatter::report_free_memory(TestResult* result, TestMemoryAllocator* allocator, char* memory, const char* file, size_t line)
{
    std::string variableName;
    CodeReportingAllocationNode* node = findNode(memory);

    if (memory == nullptr)
        variableName = "NULL";
    else
        variableName = node->variableName_;

    result->print(StringFromFormat("\t%s\n", getDeallocationString(allocator, variableName, file, line).c_str()).c_str());
}
