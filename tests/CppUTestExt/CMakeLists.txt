if(CPPUTEST_SPLIT_TESTS)
    add_executable(CppUTestExtTests1
        AllTests.cpp
        MockFailureReporterForTest.cpp
        ExpectedFunctionsListTest.cpp
        GMockTest.cpp
        GTest1Test.cpp
        GTest2ConvertorTest.cpp
    )
    target_link_libraries(CppUTestExtTests1 PRIVATE ${CppUTestExtLibName})
    set_target_properties(CppUTestExtTests1 PROPERTIES OUTPUT_NAME CPPUX1)

    add_executable(CppUTestExtTests2
        AllTests.cpp
        MockFailureReporterForTest.cpp
        MemoryReportAllocatorTest.cpp
        MemoryReportFormatterTest.cpp
        MemoryReporterPluginTest.cpp
        MockActualCallTest.cpp
        MockCheatSheetTest.cpp
        MockComparatorCopierTest.cpp
        MockExpectedCallTest.cpp
        MockHierarchyTest.cpp
    )
    target_link_libraries(CppUTestExtTests2 PRIVATE ${CppUTestExtLibName})
    set_target_properties(CppUTestExtTests2 PROPERTIES OUTPUT_NAME CPPUX2)

    add_executable(CppUTestExtTests3
        AllTests.cpp
        MockFailureReporterForTest.cpp
        CodeMemoryReporterTest.cpp
        OrderedTestTest.cpp
        OrderedTestTest_c.c
    )
    target_link_libraries(CppUTestExtTests3 PRIVATE ${CppUTestExtLibName})
    set_target_properties(CppUTestExtTests3 PROPERTIES OUTPUT_NAME CPPUX3)

    add_executable(CppUTestExtTests4
        AllTests.cpp
        MockFailureReporterForTest.cpp
        MockReturnValueTest.cpp
        MockNamedValueTest.cpp
    )
    target_link_libraries(CppUTestExtTests4 PRIVATE ${CppUTestExtLibName})
    set_target_properties(CppUTestExtTests4 PROPERTIES OUTPUT_NAME CPPUX4)

    add_executable(CppUTestExtTests5
        AllTests.cpp
        MockFailureReporterForTest.cpp
        MockPluginTest.cpp
        MockSupport_cTest.cpp
        MockSupport_cTestCFile.c
    )
    target_link_libraries(CppUTestExtTests5 PRIVATE ${CppUTestExtLibName})
    set_target_properties(CppUTestExtTests5 PROPERTIES OUTPUT_NAME CPPUX5)

    add_executable(CppUTestExtTests6
        AllTests.cpp
        MockFailureReporterForTest.cpp
        ExpectedFunctionsListTest.cpp
        MockCallTest.cpp
    )
    target_link_libraries(CppUTestExtTests6 PRIVATE ${CppUTestExtLibName})
    set_target_properties(CppUTestExtTests6 PROPERTIES OUTPUT_NAME CPPUX6)

    add_executable(CppUTestExtTests7
        AllTests.cpp
        MockFailureReporterForTest.cpp
        MockComparatorCopierTest.cpp
        MockHierarchyTest.cpp
        MockParameterTest.cpp
    )
    target_link_libraries(CppUTestExtTests7 PRIVATE ${CppUTestExtLibName})
    set_target_properties(CppUTestExtTests7 PROPERTIES OUTPUT_NAME CPPUX7)

    add_executable(CppUTestExtTests8
        AllTests.cpp
        MockFailureReporterForTest.cpp
        IEEE754PluginTest.cpp
        IEEE754PluginTest_c.c
        MockComparatorCopierTest.cpp
    )
    target_link_libraries(CppUTestExtTests8 PRIVATE ${CppUTestExtLibName})
    set_target_properties(CppUTestExtTests8 PROPERTIES OUTPUT_NAME CPPUX8)

    add_executable(CppUTestExtTests9
        AllTests.cpp
        MockFailureReporterForTest.cpp
        MockFailureTest.cpp
        MockHierarchyTest.cpp
        MockPluginTest.cpp
        MockReturnValueTest.cpp
        MockStrictOrderTest.cpp
        MockSupportTest.cpp
    )
    target_link_libraries(CppUTestExtTests9 PRIVATE ${CppUTestExtLibName})
    set_target_properties(CppUTestExtTests9 PROPERTIES OUTPUT_NAME CPPUX9)
else()
    add_executable(CppUTestExtTests
        AllTests.cpp
        CodeMemoryReporterTest.cpp
        GMockTest.cpp
        GTest1Test.cpp
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

    if(MINGW)
        set(THREADS_PREFER_PTHREAD_FLAG ON)
        find_package(Threads)
        if(CMAKE_USE_PTHREADS_INIT)
            target_link_libraries(CppUTestExtTests PRIVATE Threads::Threads)
        endif()
    endif()

    cpputest_normalize_test_output_location(CppUTestExtTests)
    target_link_libraries(CppUTestExtTests
        PRIVATE
            ${CppUTestLibName}
            ${CppUTestExtLibName}
    )

    if (CPPUTEST_TEST_DISCOVERY)
        cpputest_buildtime_discover_tests(CppUTestExtTests)
    endif()
endif()
