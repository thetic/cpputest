add_library(CppUTest
    include/CppUTest/CommandLineArguments.h
    include/CppUTest/CommandLineTestRunner.h
    include/CppUTest/CppUTestConfig.h
    include/CppUTest/CppUTestGeneratedConfig.h
    include/CppUTest/JUnitTestOutput.h
    include/CppUTest/MemoryLeakDetector.h
    include/CppUTest/MemoryLeakDetectorMallocMacros.h
    include/CppUTest/MemoryLeakDetectorNewMacros.h
    include/CppUTest/MemoryLeakWarningPlugin.h
    include/CppUTest/PlatformSpecificFunctions.h
    include/CppUTest/PlatformSpecificFunctions_c.h
    include/CppUTest/SimpleMutex.h
    include/CppUTest/SimpleString.h
    include/CppUTest/SimpleStringInternalCache.h
    include/CppUTest/StandardCLibrary.h
    include/CppUTest/TeamCityTestOutput.h
    include/CppUTest/TestFailure.h
    include/CppUTest/TestFilter.h
    include/CppUTest/TestHarness.h
    include/CppUTest/TestHarness_c.h
    include/CppUTest/TestMemoryAllocator.h
    include/CppUTest/TestOutput.h
    include/CppUTest/TestPlugin.h
    include/CppUTest/TestRegistry.h
    include/CppUTest/TestResult.h
    include/CppUTest/TestTestingFixture.h
    include/CppUTest/Utest.h
    include/CppUTest/UtestMacros.h
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

include(CheckCXXSymbolExists)
include(CheckCXXSourceCompiles)

check_cxx_symbol_exists(INFINITY "math.h" HAS_INF)
check_cxx_symbol_exists(NAN "math.h" HAS_NAN)
check_cxx_source_compiles(
    [[#include "fenv.h"
    int main() {feclearexcept(FE_ALL_EXCEPT);}]]
    HAVE_FENV
)

target_compile_definitions(CppUTest
    PUBLIC
        $<$<NOT:$<BOOL:${CPPUTEST_MEMORY_LEAK_DETECTION}>>:CPPUTEST_MEM_LEAK_DETECTION_DISABLED>
        $<$<NOT:$<BOOL:${CPPUTEST_STD_C}>>:CPPUTEST_STD_C_LIB_DISABLED>
        $<$<NOT:$<BOOL:${CPPUTEST_STD_CPP}>>:CPPUTEST_STD_CPP_LIB_DISABLED>
        $<$<NOT:$<BOOL:${HAS_INF}>>:CPPUTEST_NO_INF>
        $<$<NOT:$<BOOL:${HAS_NAN}>>:CPPUTEST_NO_NAN>
        $<$<BOOL:${HAVE_FENV}>:CPPUTEST_HAVE_FENV>
        $<$<BOOL:${CPPUTEST_USE_LONG_LONG}>:CPPUTEST_USE_LONG_LONG>
        $<$<BOOL:${HAVE_LONGLONG}>:CPPUTEST_HAVE_LONG_LONG_INT>
)

add_library(CppUTest::CppUTest ALIAS CppUTest)

add_subdirectory(Platforms)

if(CPPUTEST_BUILD_TESTING)
    add_subdirectory(tests)
endif()
