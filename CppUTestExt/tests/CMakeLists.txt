add_executable(CppUTestExtTests
    include/IEEE754PluginTest.h
    include/MockFailureReporterForTest.hpp
    include/MockSupport_cTestCFile.h
    include/OrderedTestTest.h
    src/AllTests.cpp
    src/CodeMemoryReporterTest.cpp
    src/ExpectedFunctionsListTest.cpp
    src/IEEE754PluginTest.cpp
    src/IEEE754PluginTest_c.c
    src/MemoryReportAllocatorTest.cpp
    src/MemoryReportFormatterTest.cpp
    src/MemoryReporterPluginTest.cpp
    src/MockActualCallTest.cpp
    src/MockCallTest.cpp
    src/MockCheatSheetTest.cpp
    src/MockComparatorCopierTest.cpp
    src/MockExpectedCallTest.cpp
    src/MockFailureReporterForTest.cpp
    src/MockFailureTest.cpp
    src/MockFakeLongLong.cpp
    src/MockHierarchyTest.cpp
    src/MockNamedValueTest.cpp
    src/MockParameterTest.cpp
    src/MockPluginTest.cpp
    src/MockReturnValueTest.cpp
    src/MockStrictOrderTest.cpp
    src/MockSupportTest.cpp
    src/MockSupport_cTest.cpp
    src/MockSupport_cTestCFile.c
    src/OrderedTestTest.cpp
    src/OrderedTestTest_c.c
)

target_include_directories(CppUTestExtTests PRIVATE include)

target_link_libraries(CppUTestExtTests
    PRIVATE
        CppUTest::CppUTestExt
)

target_link_options(CppUTestExtTests
    PRIVATE
        $<$<BOOL:${MINGW}>:-static>
)

if(CPPUTEST_DISCOVER)
    include(CppUTest)
    CppUTest_discover_tests(CppUTestExtTests
        TEST_PREFIX CppUTestExt.
    )
else()
    add_test(
        NAME CppUTestExt
        COMMAND CppUTestExtTests
    )
endif()
