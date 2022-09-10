set(CppUTestTestsSourceLists)

if(CPPUTEST_SPLIT_TESTS)
    set(CppUTestTestsSources1
        AllocLetTestFree.c
        AllocLetTestFreeTest.cpp
        CheatSheetTest.cpp
        CompatabilityTests.cpp
        CommandLineArgumentsTest.cpp
        CommandLineTestRunnerTest.cpp
        JUnitOutputTest.cpp
    )
    list(APPEND CppUTestTestsSourceLists CppUTestTestSources1)

    set(CppUTestTestsSources2
        DummyMemoryLeakDetector.cpp
        MemoryLeakWarningTest.cpp
        PluginTest.cpp
        PreprocessorTest.cpp
        SetPluginTest.cpp
        SimpleMutexTest.cpp
        TeamCityOutputTest.cpp
        TestFailureNaNTest.cpp
        TestFailureTest.cpp
        TestResultTest.cpp
    )
    list(APPEND CppUTestTestsSourceLists CppUTestTestSources2)

    set(CppUTestTestsSources3
        MemoryLeakDetectorTest.cpp
        SimpleStringTest.cpp
        SimpleStringCacheTest.cpp
    )
    list(APPEND CppUTestTestsSourceLists CppUTestTestSources3)

    set(CppUTestTestsSources4
        TestOutputTest.cpp
        TestRegistryTest.cpp
    )
    list(APPEND CppUTestTestsSourceLists CppUTestTestSources4)

    set(CppUTestTestsSources5
        AllocationInCFile.c
        AllocationInCppFile.cpp
        MemoryOperatorOverloadTest.cpp
        TeamCityOutputTest.cpp
    )
    list(APPEND CppUTestTestsSourceLists CppUTestTestSources5)

    set(CppUTestTestsSources6
        TestFilterTest.cpp
        TestHarness_cTest.cpp
        TestHarness_cTestCFile.c
        TestInstallerTest.cpp
    )
    list(APPEND CppUTestTestsSourceLists CppUTestTestSources6)

    set(CppUTestTestsSources7
        TestMemoryAllocatorTest.cpp
        TestUTestMacro.cpp
    )
    list(APPEND CppUTestTestsSourceLists CppUTestTestSources7)

    set(CppUTestTestsSources8
        UtestPlatformTest.cpp
        UtestTest.cpp
        TestUTestStringMacro.cpp
    )
    list(APPEND CppUTestTestsSourceLists CppUTestTestSources8)
else()
    set(CppUTestTestsSources
        SetPluginTest.cpp
        CheatSheetTest.cpp
        SimpleStringTest.cpp
        SimpleStringCacheTest.cpp
        CompatabilityTests.cpp
        CommandLineArgumentsTest.cpp
        TestFailureTest.cpp
        TestFailureNaNTest.cpp
        CommandLineTestRunnerTest.cpp
        TestFilterTest.cpp
        TestHarness_cTest.cpp
        JUnitOutputTest.cpp
        TestHarness_cTestCFile.c
        DummyMemoryLeakDetector.cpp
        MemoryLeakDetectorTest.cpp
        TestInstallerTest.cpp
        AllocLetTestFree.c
        MemoryOperatorOverloadTest.cpp
        TestMemoryAllocatorTest.cpp
        MemoryLeakWarningTest.cpp
        TestOutputTest.cpp
        AllocLetTestFreeTest.cpp
        TestRegistryTest.cpp
        AllocationInCFile.c
        PluginTest.cpp
        TestResultTest.cpp
        PreprocessorTest.cpp
        TestUTestMacro.cpp
        TestUTestStringMacro.cpp
        AllocationInCppFile.cpp
        UtestTest.cpp
        SimpleMutexTest.cpp
        UtestPlatformTest.cpp
        TeamCityOutputTest.cpp
    )
    list(APPEND CppUTestTestsSourceLists CppUTestTestsSources)
endif()

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4723")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4723")
endif (MSVC)

if(MINGW OR (${CMAKE_HOST_SYSTEM_NAME} MATCHES "OpenBSD"))
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads)
endif()

foreach(SourceList IN LISTS CppUTestTestsSourceLists)
    string(REGEX REPLACE "Sources" "" TestName ${SourceList})

    add_executable(${TestName}
        AllTests.cpp
        ${${SourceList}}
    )

    target_link_libraries(${TestName}
        PRIVATE
            ${CppUTestLibName}
            $<$<BOOL:${CMAKE_USE_PTHREADS_INIT}>:Threads::Threads>
    )

    if(DOS)
        # Use a shorter file name
        string(REGEX MATCH "\\d+\$" TestNumber ${SourceList})
        set_target_properties(${TestName} PROPERTIES
            OUTPUT_NAME CPPU${TestNumber}
        )
    endif()

    cpputest_normalize_test_output_location(${TestName})
    if (CPPUTEST_TEST_DISCOVERY)
        cpputest_buildtime_discover_tests(${TestName})
    endif()
endforeach()
