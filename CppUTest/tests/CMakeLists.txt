add_executable(CppUTestTests
    src/AllTests.cpp
    src/CheatSheetTest.cpp
    src/CommandLineArgumentsTest.cpp
    src/CommandLineTestRunnerTest.cpp
    src/CompatabilityTests.cpp
    src/JUnitOutputTest.cpp
    src/PluginTest.cpp
    src/PreprocessorTest.cpp
    src/SetPluginTest.cpp
    src/SimpleMutexTest.cpp
    src/SimpleStringTest.cpp
    src/TeamCityOutputTest.cpp
    src/TestFailureNaNTest.cpp
    src/TestFailureTest.cpp
    src/TestFilterTest.cpp
    src/TestHarness_cTestCFile.c
    src/TestHarness_cTest.cpp
    src/TestInstallerTest.cpp
    src/TestOutputTest.cpp
    src/TestRegistryTest.cpp
    src/TestResultTest.cpp
    src/TestUTestMacro.cpp
    src/TestUTestStringMacro.cpp
    src/UtestPlatformTest.cpp
    src/UtestTest.cpp
)

target_include_directories(CppUTestTests PUBLIC include)

include(CheckCXXSymbolExists)
check_cxx_symbol_exists(NAN "math.h" HAS_NAN)
check_cxx_symbol_exists(INFINITY "math.h" HAS_INF)

target_compile_definitions(CppUTestTests
    PRIVATE
        $<$<BOOL:${CPPUTEST_HAVE_FORK}>:CPPUTEST_HAVE_FORK>
        $<$<BOOL:${CPPUTEST_HAVE_WAITPID}>:CPPUTEST_HAVE_WAITPID>
        CPPUTEST_HAS_NAN=$<BOOL:${HAS_NAN}>
        CPPUTEST_HAS_INF=$<BOOL:${HAS_INF}>
)

target_link_libraries(CppUTestTests
    PRIVATE
        CppUTest::CppUTest
)

target_link_options(CppUTestTests
    PRIVATE
        $<$<BOOL:${MINGW}>:-static>
)

if(CPPUTEST_DISCOVER)
    include(CppUTest)
    CppUTest_discover_tests(CppUTestTests
        TEST_PREFIX CppUTest.
    )
else()
    add_test(
        NAME CppUTest
        COMMAND CppUTestTests
    )
endif()
