set(CppUTestExtTestsSourceLists)

if(CPPUTEST_SPLIT_TESTS)
    set(CppUTestExtTestsSources1
        MockFailureReporterForTest.cpp
        ExpectedFunctionsListTest.cpp
        GMockTest.cpp
        GTest1Test.cpp
        GTest2ConvertorTest.cpp
    )
    list(APPEND CppUTestExtTestsSourceLists CppUTestExtTestsSources1)

    set(CppUTestExtTestsSources2
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
    list(APPEND CppUTestExtTestsSourceLists CppUTestExtTestsSources2)

    set(CppUTestExtTestsSources3
        MockFailureReporterForTest.cpp
        CodeMemoryReporterTest.cpp
        OrderedTestTest.cpp
        OrderedTestTest_c.c
    )
    list(APPEND CppUTestExtTestsSourceLists CppUTestExtTestsSources3)

    set(CppUTestExtTestsSources4
        MockFailureReporterForTest.cpp
        MockReturnValueTest.cpp
        MockNamedValueTest.cpp
    )
    list(APPEND CppUTestExtTestsSourceLists CppUTestExtTestsSources4)

    set(CppUTestExtTestsSources5
        MockFailureReporterForTest.cpp
        MockPluginTest.cpp
        MockSupport_cTest.cpp
        MockSupport_cTestCFile.c
    )
    list(APPEND CppUTestExtTestsSourceLists CppUTestExtTestsSources5)

    set(CppUTestExtTestsSources6
        MockFailureReporterForTest.cpp
        ExpectedFunctionsListTest.cpp
        MockCallTest.cpp
    )
    list(APPEND CppUTestExtTestsSourceLists CppUTestExtTestsSources6)

    set(CppUTestExtTestsSources7
        MockFailureReporterForTest.cpp
        MockComparatorCopierTest.cpp
        MockHierarchyTest.cpp
        MockParameterTest.cpp
    )
    list(APPEND CppUTestExtTestsSourceLists CppUTestExtTestsSources7)

    set(CppUTestExtTestsSources8
        MockFailureReporterForTest.cpp
        IEEE754PluginTest.cpp
        IEEE754PluginTest_c.c
        MockComparatorCopierTest.cpp
    )
    list(APPEND CppUTestExtTestsSourceLists CppUTestExtTestsSources8)

    set(CppUTestExtTestsSources9
        MockFailureReporterForTest.cpp
        MockFailureTest.cpp
        MockHierarchyTest.cpp
        MockPluginTest.cpp
        MockReturnValueTest.cpp
        MockStrictOrderTest.cpp
        MockSupportTest.cpp
    )
    list(APPEND CppUTestExtTestsSourceLists CppUTestExtTestsSources9)
else()
    set(CppUTestExtTestsSources
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
    list(APPEND CppUTestExtTestsSourceLists CppUTestExtTestsSources)
endif()

if(MINGW)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads)
endif()

foreach(SourceList IN LISTS CppUTestExtTestsSourceLists)
    string(REGEX REPLACE "Sources" "" TestName ${SourceList})

    add_executable(${TestName}
        AllTests.cpp
        ${${SourceList}}
    )

    target_link_libraries(${TestName}
        PRIVATE
            ${CppUTestLibName}
            ${CppUTestExtLibName}
            $<$<BOOL:${CMAKE_USE_PTHREADS_INIT}>:Threads::Threads>
    )

    if(DOS)
        # Use a shorter file name
        string(REGEX MATCH ".\$" TestNumber ${SourceList})
        set_target_properties(${TestName} PROPERTIES
            OUTPUT_NAME CPPUX${TestNumber}
        )
    endif()

    cpputest_normalize_test_output_location(${TestName})
    if (CPPUTEST_TEST_DISCOVERY)
        cpputest_buildtime_discover_tests(${TestName})
    endif()
endforeach()
