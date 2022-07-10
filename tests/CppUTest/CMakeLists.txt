set(CppUTestTests_src1
    AllocLetTestFree.c
    AllocLetTestFreeTest.cpp
    CheatSheetTest.cpp
    CompatabilityTests.cpp
    CommandLineArgumentsTest.cpp
    CommandLineTestRunnerTest.cpp
    JUnitOutputTest.cpp
)

set(CppUTestTests_src2
    DummyMemoryLeakDetector.cpp
    MemoryLeakWarningTest.cpp
    PluginTest.cpp
    PreprocessorTest.cpp
    SetPluginTest.cpp
    SimpleMutexTest.cpp
    TestFailureNaNTest.cpp
    TestFailureTest.cpp
    TestResultTest.cpp

)

set(CppUTestTests_src3
    MemoryLeakDetectorTest.cpp
    SimpleStringTest.cpp
    SimpleStringCacheTest.cpp
)

set(CppUTestTests_src4
    TestOutputTest.cpp
    TestRegistryTest.cpp
)

set(CppUTestTests_src5
    AllocationInCFile.c
    AllocationInCppFile.cpp
    MemoryOperatorOverloadTest.cpp
    TeamCityOutputTest.cpp
)

set(CppUTestTests_src6
    TestFilterTest.cpp
    TestHarness_cTest.cpp
    TestHarness_cTestCFile.c
    TestInstallerTest.cpp
)

set(CppUTestTests_src7
    TestMemoryAllocatorTest.cpp
    TestUTestMacro.cpp

)
set(CppUTestTests_src8
    UtestPlatformTest.cpp
    UtestTest.cpp
    TestUTestStringMacro.cpp
)

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4723")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4723")
endif (MSVC)

if (MINGW)
    find_package (Threads REQUIRED)
    set(THREAD_LIB "pthread")
endif (MINGW)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "OpenBSD")
   set (THREADS_HAVE_PTHREAD_ARG 1)
   find_package (Threads REQUIRED)
   set (THREAD_LIB "pthread")
endif ()

add_library(CppUTestTests_main OBJECT AllTests.cpp)

target_link_libraries(CppUTestTests_main
    PUBLIC
        ${CppUTestLibName}
        ${THREAD_LIB}
)

set(cpputest_test_executables)
if(CPPUTEST_SPLIT_TESTS)
    foreach(i RANGE 1 8)
        add_executable(CppUTestTests${i} ${CppUTestTests_src${i}})
        list(APPEND cpputest_test_executables CppUTestTests${i})
    endforeach()
else()
    add_executable(CppUTestTests
        ${CppUTestTests_src1}
        ${CppUTestTests_src2}
        ${CppUTestTests_src3}
        ${CppUTestTests_src4}
        ${CppUTestTests_src5}
        ${CppUTestTests_src6}
        ${CppUTestTests_src7}
        ${CppUTestTests_src8}
    )
    list(APPEND cpputest_test_executables CppUTestTests)
endif()

foreach(tgt IN LISTS cpputest_test_executables)
    target_link_libraries(${tgt} PRIVATE CppUTestTests_main)

    cpputest_normalize_test_output_location(${tgt})
    if (TESTS_BUILD_DISCOVER)
        cpputest_buildtime_discover_tests(${tgt})
    endif()
endforeach()
