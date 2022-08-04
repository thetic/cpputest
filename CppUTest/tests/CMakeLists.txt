add_executable(CppUTestTests
    src/AllocationInCFile.c
    src/AllocationInCppFile.cpp
    src/AllocLetTestFree.c
    src/AllocLetTestFreeTest.cpp
    src/AllTests.cpp
    src/CheatSheetTest.cpp
    src/CommandLineArgumentsTest.cpp
    src/CommandLineTestRunnerTest.cpp
    src/CompatabilityTests.cpp
    src/DummyMemoryLeakDetector.cpp
    src/JUnitOutputTest.cpp
    src/MemoryLeakDetectorTest.cpp
    src/MemoryLeakWarningTest.cpp
    src/MemoryOperatorOverloadTest.cpp
    src/PluginTest.cpp
    src/PreprocessorTest.cpp
    src/SetPluginTest.cpp
    src/SimpleMutexTest.cpp
    src/SimpleStringCacheTest.cpp
    src/SimpleStringTest.cpp
    src/TeamCityOutputTest.cpp
    src/TestFailureNaNTest.cpp
    src/TestFailureTest.cpp
    src/TestFilterTest.cpp
    src/TestHarness_cTestCFile.c
    src/TestHarness_cTest.cpp
    src/TestInstallerTest.cpp
    src/TestMemoryAllocatorTest.cpp
    src/TestOutputTest.cpp
    src/TestRegistryTest.cpp
    src/TestResultTest.cpp
    src/TestUTestMacro.cpp
    src/TestUTestStringMacro.cpp
    src/UtestPlatformTest.cpp
    src/UtestTest.cpp
    include/AllocationInCFile.h
    include/AllocationInCppFile.h
    include/AllocLetTestFree.h
    include/AllTests.h
    include/DummyMemoryLeakDetector.h
)

target_include_directories(CppUTestTests PUBLIC include)

target_compile_definitions(CppUTestTests
    PRIVATE
        $<$<BOOL:${CPPUTEST_HAVE_FORK}>:CPPUTEST_HAVE_FORK>
        $<$<BOOL:${CPPUTEST_HAVE_WAITPID}>:CPPUTEST_HAVE_WAITPID>
)

target_link_libraries(CppUTestTests
    PRIVATE
        CppUTest::CppUTest
        CppUTest::CppUTestExt
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
