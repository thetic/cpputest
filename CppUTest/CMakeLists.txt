add_library(CppUTest
    include/CppUTest/CommandLineArguments.hpp
    include/CppUTest/CommandLineTestRunner.hpp
    include/CppUTest/JUnitTestOutput.hpp
    include/CppUTest/PlatformSpecificFunctions.hpp
    include/CppUTest/PlatformSpecificFunctions.h
    include/CppUTest/SimpleMutex.hpp
    include/CppUTest/SimpleString.hpp
    include/CppUTest/TeamCityTestOutput.hpp
    include/CppUTest/TestFailure.hpp
    include/CppUTest/TestFilter.hpp
    include/CppUTest/TestHarness.hpp
    include/CppUTest/TestHarness.h
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
    src/SimpleMutex.cpp
    src/SimpleString.cpp
    src/TeamCityTestOutput.cpp
    src/TestFailure.cpp
    src/TestFilter.cpp
    src/TestHarness_c.cpp
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
    [[#include <cfenv>
    int main() { feclearexcept(FE_ALL_EXCEPT); }]]
    CPPUTEST_HAVE_FENV
)
check_cxx_source_compiles(
    "int main() { throw 42; }"
    CPPUTEST_HAVE_EXCEPTIONS
)

target_compile_definitions(CppUTest
    PUBLIC
        "CPPUTEST_NO_EXCEPTIONS=$<NOT:$<BOOL:${CPPUTEST_HAVE_EXCEPTIONS}>>"
        "CPPUTEST_HAVE_FENV=$<BOOL:${CPPUTEST_HAVE_FENV}>"
)

target_compile_features(CppUTest
    PUBLIC
        cxx_std_11
        c_std_99
)

add_library(CppUTest::CppUTest ALIAS CppUTest)

add_subdirectory(Platforms)

if(CPPUTEST_BUILD_TESTING)
    add_subdirectory(tests)
endif()
