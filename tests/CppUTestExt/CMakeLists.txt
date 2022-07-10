set(CppUTestExtTests_src1
    ExpectedFunctionsListTest.cpp
    GMockTest.cpp
    GTest1Test.cpp
    GTest2ConvertorTest.cpp
)

set(CppUTestExtTests_src2
    MemoryReportAllocatorTest.cpp
    MemoryReportFormatterTest.cpp
    MemoryReporterPluginTest.cpp
    MockActualCallTest.cpp
    MockCheatSheetTest.cpp
    MockComparatorCopierTest.cpp
    MockExpectedCallTest.cpp
    MockHierarchyTest.cpp
)

set(CppUTestExtTests_src3
    CodeMemoryReporterTest.cpp
    OrderedTestTest.cpp
    OrderedTestTest_c.c
)

set(CppUTestExtTests_src4
    MockReturnValueTest.cpp
    MockNamedValueTest.cpp
)

set(CppUTestExtTests_src5
    MockPluginTest.cpp
    MockSupport_cTest.cpp
    MockSupport_cTestCFile.c
)

set(CppUTestExtTests_src6
    ExpectedFunctionsListTest.cpp
    MockCallTest.cpp
)

set(CppUTestExtTests_src7
    MockComparatorCopierTest.cpp
    MockHierarchyTest.cpp
    MockParameterTest.cpp
)

set(CppUTestExtTests_src8
    IEEE754PluginTest.cpp
    IEEE754PluginTest_c.c
    MockComparatorCopierTest.cpp
)

set(CppUTestExtTests_src9
    MockFailureTest.cpp
    MockHierarchyTest.cpp
    MockPluginTest.cpp
    MockReturnValueTest.cpp
    MockStrictOrderTest.cpp
    MockSupportTest.cpp
)

if (MINGW)
    find_package (Threads REQUIRED)
    set(THREAD_LIB "pthread")
endif (MINGW)

add_library(CppUTestExtTests_main OBJECT
    AllTests.cpp
    MockFailureReporterForTest.cpp
)

target_link_libraries(CppUTestExtTests_main
    PUBLIC
        ${CppUTestLibName}
        ${CppUTestExtLibName}
        ${THREAD_LIB} ${CPPUNIT_EXTERNAL_LIBRARIES}
)

set(cpputestext_test_executables)
if(CPPUTEST_SPLIT_TESTS)
    foreach(i RANGE 1 9)
        add_executable(CppUTestExtTests${i} ${CppUTestExtTests_src${i}})

        set_target_properties(CppUTestExtTests${i} PROPERTIES
            OUTPUT_NAME CPPUX${i}
        )

        list(APPEND cpputestext_test_executables CppUTestExtTests${i})
    endforeach()
else()
    add_executable(CppUTestExtTests
        ${CppUTestExtTests_src1}
        ${CppUTestExtTests_src2}
        ${CppUTestExtTests_src3}
        ${CppUTestExtTests_src4}
        ${CppUTestExtTests_src5}
        ${CppUTestExtTests_src6}
        ${CppUTestExtTests_src7}
        ${CppUTestExtTests_src8}
        ${CppUTestExtTests_src9}
    )
    list(APPEND cpputestext_test_executables CppUTestExtTests)
endif()

foreach(tgt IN LISTS cpputestext_test_executables)
    target_link_libraries(${tgt} PUBLIC CppUTestExtTests_main)

    cpputest_normalize_test_output_location(${tgt})
    if (TESTS_BUILD_DISCOVER)
        cpputest_buildtime_discover_tests(${tgt})
    endif()
endforeach()
