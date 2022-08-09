add_library(CppUTest
    include/CppUTest/CommandLineArguments.hpp
    include/CppUTest/CommandLineTestRunner.hpp
    include/CppUTest/JUnitTestOutput.hpp
    include/CppUTest/MemoryLeakDetector.hpp
    include/CppUTest/MemoryLeakWarningPlugin.hpp
    include/CppUTest/PlatformSpecificFunctions.hpp
    include/CppUTest/PlatformSpecificFunctions.h
    include/CppUTest/SimpleMutex.hpp
    include/CppUTest/SimpleString.hpp
    include/CppUTest/SimpleStringInternalCache.hpp
    include/CppUTest/TeamCityTestOutput.hpp
    include/CppUTest/TestFailure.hpp
    include/CppUTest/TestFilter.hpp
    include/CppUTest/TestHarness.hpp
    include/CppUTest/TestHarness.h
    include/CppUTest/TestMemoryAllocator.hpp
    include/CppUTest/TestOutput.hpp
    include/CppUTest/TestPlugin.hpp
    include/CppUTest/TestRegistry.hpp
    include/CppUTest/TestResult.hpp
    include/CppUTest/TestTestingFixture.hpp
    include/CppUTest/Utest.hpp
    include/CppUTest/UtestMacros.hpp
    src/CommandLineArguments.cpp
    src/CommandLineTestRunner.cpp
    src/JUnitTestOutput.cpp
    src/MemoryLeakDetector.cpp
    src/MemoryLeakWarningPlugin.cpp
    src/SimpleMutex.cpp
    src/SimpleString.cpp
    src/SimpleStringInternalCache.cpp
    src/TeamCityTestOutput.cpp
    src/TestFailure.cpp
    src/TestFilter.cpp
    src/TestHarness_c.cpp
    src/TestMemoryAllocator.cpp
    src/TestOutput.cpp
    src/TestPlugin.cpp
    src/TestRegistry.cpp
    src/TestResult.cpp
    src/TestTestingFixture.cpp
    src/Utest.cpp
)

target_include_directories(CppUTest PUBLIC include)

include(CheckCXXSourceCompiles)
check_cxx_source_compiles(
    [[#include "fenv.h"
    int main() {feclearexcept(FE_ALL_EXCEPT);}]]
    HAVE_FENV
)

target_compile_definitions(CppUTest
    PUBLIC
        "CPPUTEST_NO_EXCEPTIONS=$<BOOL:${CPPUTEST_NO_EXCEPTIONS}>"
        "CPPUTEST_HAVE_FENV=$<BOOL:${HAVE_FENV}>"
)

add_library(CppUTest::CppUTest ALIAS CppUTest)

add_subdirectory(Platforms)

if(CPPUTEST_BUILD_TESTING)
    add_subdirectory(tests)
endif()
