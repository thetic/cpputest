if(CPPUTEST_SPLIT_TESTS)
    add_executable(CppUTestTests1
        AllTests.cpp
        AllocLetTestFree.c
        AllocLetTestFreeTest.cpp
        CheatSheetTest.cpp
        CompatabilityTests.cpp
        CommandLineArgumentsTest.cpp
        CommandLineTestRunnerTest.cpp
        JUnitOutputTest.cpp
    )
    target_link_libraries(CppUTestTests1 PRIVATE ${CppUTestLibName})

    add_executable(CppUTestTests2
        AllTests.cpp
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
    target_link_libraries(CppUTestTests2 PRIVATE ${CppUTestLibName})

    add_executable(CppUTestTests3
        AllTests.cpp
        MemoryLeakDetectorTest.cpp
        SimpleStringTest.cpp
        SimpleStringCacheTest.cpp
    )
    target_link_libraries(CppUTestTests3 PRIVATE ${CppUTestLibName})

    add_executable(CppUTestTests4
        AllTests.cpp
        TestOutputTest.cpp
        TestRegistryTest.cpp
    )
    target_link_libraries(CppUTestTests4 PRIVATE ${CppUTestLibName})

    add_executable(CppUTestTests5
        AllTests.cpp
        AllocationInCFile.c
        AllocationInCppFile.cpp
        MemoryOperatorOverloadTest.cpp
        TeamCityOutputTest.cpp
    )
    target_link_libraries(CppUTestTests5 PRIVATE ${CppUTestLibName})

    add_executable(CppUTestTests6
        AllTests.cpp
        TestFilterTest.cpp
        TestHarness_cTest.cpp
        TestHarness_cTestCFile.c
        TestInstallerTest.cpp
    )
    target_link_libraries(CppUTestTests6 PRIVATE ${CppUTestLibName})

    add_executable(CppUTestTests7
        AllTests.cpp
        TestMemoryAllocatorTest.cpp
        TestUTestMacro.cpp
    )
    target_link_libraries(CppUTestTests7 PRIVATE ${CppUTestLibName})

    add_executable(CppUTestTests8
        AllTests.cpp
        UtestPlatformTest.cpp
        UtestTest.cpp
        TestUTestStringMacro.cpp
    )
    target_link_libraries(CppUTestTests8 PRIVATE ${CppUTestLibName})
else()
    add_executable(CppUTestTests
        AllTests.cpp
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

    if (MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4723")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4723")
    endif (MSVC)

    if(MINGW OR (${CMAKE_HOST_SYSTEM_NAME} MATCHES "OpenBSD"))
        set(THREADS_PREFER_PTHREAD_FLAG ON)
        find_package(Threads)
        if(CMAKE_USE_PTHREADS_INIT)
            target_link_libraries(CppUTestTests PRIVATE Threads::Threads)
        endif()
    endif()

    cpputest_normalize_test_output_location(CppUTestTests)
    target_link_libraries(CppUTestTests PRIVATE ${CppUTestLibName})

    if (CPPUTEST_TEST_DISCOVERY)
        cpputest_buildtime_discover_tests(CppUTestTests)
    endif()
endif()
