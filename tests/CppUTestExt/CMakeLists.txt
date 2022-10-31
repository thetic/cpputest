add_executable(CppUTestExtTests
    AllTests.cpp
    CodeMemoryReporterTest.cpp
    IEEE754PluginTest.cpp
    IEEE754PluginTest_c.c
    MemoryReportAllocatorTest.cpp
    MemoryReporterPluginTest.cpp
    MemoryReportFormatterTest.cpp
    MockActualCallTest.cpp
    MockCheatSheetTest.cpp
    MockCallTest.cpp
    MockComparatorCopierTest.cpp
    MockExpectedCallTest.cpp
    ExpectedFunctionsListTest.cpp
    MockFailureReporterForTest.cpp
    MockFailureTest.cpp
    MockHierarchyTest.cpp
    MockNamedValueTest.cpp
    MockParameterTest.cpp
    MockPluginTest.cpp
    MockSupportTest.cpp
    MockSupport_cTestCFile.c
    MockSupport_cTest.cpp
    MockStrictOrderTest.cpp
    MockReturnValueTest.cpp
    OrderedTestTest_c.c
    OrderedTestTest.cpp
)

target_link_libraries(CppUTestExtTests
    PRIVATE
        CppUTest
        CppUTestExt
)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.13")
    target_link_options(CppUTestExtTests
        PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:LINKER:-Map=$<TARGET_FILE:CppUTestExtTests>.map>
            "$<$<CXX_COMPILER_ID:MSVC>:/MAP>"
            $<$<CXX_COMPILER_ID:IAR>:"SHELL:--map $<TARGET_FILE:CppUTestExtTests>.map">
    )
endif()

include(CppUTestNormalizeTestOutputLocation)
cpputest_normalize_test_output_location(CppUTestExtTests)

if (CPPUTEST_TEST_DISCOVERY)
    include(CppUTestBuildTimeDiscoverTests)
    cpputest_buildtime_discover_tests(CppUTestExtTests)
endif()
