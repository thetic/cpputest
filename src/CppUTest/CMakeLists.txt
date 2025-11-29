add_library(CppUTest
        CommandLineArguments.cpp
        MemoryLeakWarningPlugin.cpp
        TestHarness_c.cpp
        TestRegistry.cpp
        CommandLineTestRunner.cpp
        SimpleString.cpp
        SimpleStringInternalCache.cpp
        TestMemoryAllocator.cpp
        TestResult.cpp
        JUnitTestOutput.cpp
        TeamCityTestOutput.cpp
        TestFailure.cpp
        TestOutput.cpp
        MemoryLeakDetector.cpp
        TestFilter.cpp
        TestPlugin.cpp
        TestTestingFixture.cpp
        SimpleMutex.cpp
        Utest.cpp
        ${PROJECT_SOURCE_DIR}/include/CppUTest/CommandLineArguments.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/PlatformSpecificFunctions.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestMemoryAllocator.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/CommandLineTestRunner.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/PlatformSpecificFunctions_c.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestOutput.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/CppUTestConfig.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/SimpleString.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/SimpleStringInternalCache.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestPlugin.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/JUnitTestOutput.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TeamCityTestOutput.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/StandardCLibrary.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestRegistry.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/MemoryLeakDetector.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestFailure.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestResult.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/MemoryLeakDetectorMallocMacros.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestFilter.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestTestingFixture.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/MemoryLeakDetectorNewMacros.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/MemoryLeakDetectorForceInclude.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestHarness.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/Utest.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/MemoryLeakWarningPlugin.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestHarness_c.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/UtestMacros.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/SimpleMutex.h
)

#[[Set CPPUTEST_PLATFORM in a parent CMakeLists.txt if reusing one of the provided platforms, else supply the missing definitions]]
if(CPPUTEST_PLATFORM)
    target_sources(CppUTest
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/../Platforms/${CPPUTEST_PLATFORM}/UtestPlatform.cpp
    )
endif()

if(CPPUTEST_PLATFORM STREQUAL "Iar")
    target_compile_options(CppUTest
        PRIVATE
            --dlib_config full
    )
endif()

#[[Arrange for the include directory to be added to the include paths of any CMake target depending on CppUTest.]]
target_include_directories(CppUTest
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(CppUTest
    PUBLIC
        CPPUTEST_USE_MEM_LEAK_DETECTION=$<NOT:$<BOOL:${CPPUTEST_MEM_LEAK_DETECTION_DISABLED}>>
        CPPUTEST_USE_LONG_LONG=$<BOOL:${CPPUTEST_USE_LONG_LONG}>
        CPPUTEST_USE_STD_C_LIB=$<NOT:$<BOOL:${CPPUTEST_STD_C_LIB_DISABLED}>>
        CPPUTEST_USE_STD_CPP_LIB=$<NOT:$<BOOL:${CPPUTEST_STD_CPP_LIB_DISABLED}>>
)

# Check for symbols before setting a lot of stuff
set(CMAKE_REQUIRED_FLAGS ${CMAKE_CXX_FLAGS})

if (MINGW)
    # Apply workaround for MinGW timespec redefinition (pthread.h / time.h).
    include(CheckStructHasMember)
    check_struct_has_member("struct timespec" tv_sec time.h HAVE_STRUCT_TIMESPEC LANGUAGE CXX)
    if(HAVE_STRUCT_TIMESPEC)
        target_compile_definitions(CppUTest PRIVATE _TIMESPEC_DEFINED)
    endif()
endif()

include(CheckCXXSymbolExists)
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "IAR")
    check_cxx_symbol_exists(strdup "string.h" CPPUTEST_HAVE_STRDUP)
    if (CPPUTEST_HAVE_STRDUP)
        target_compile_definitions(CppUTest PUBLIC CPPUTEST_HAVE_STRDUP)
    endif()
endif()

check_cxx_symbol_exists(kill "signal.h" CPPUTEST_HAVE_KILL)
if (CPPUTEST_HAVE_KILL)
    target_compile_definitions(CppUTest PUBLIC CPPUTEST_HAVE_KILL)
endif()

check_cxx_symbol_exists(waitpid "sys/wait.h" CPPUTEST_HAVE_WAITPID)
if(CPPUTEST_HAVE_WAITPID)
    target_compile_definitions(CppUTest PUBLIC CPPUTEST_HAVE_WAITPID)
endif()

check_cxx_symbol_exists(gettimeofday "sys/time.h" CPPUTEST_HAVE_GETTIMEOFDAY)
if(CPPUTEST_HAVE_GETTIMEOFDAY)
    target_compile_definitions(CppUTest PRIVATE CPPUTEST_HAVE_GETTIMEOFDAY)
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
set(CPPUTEST_HAVE_PTHREADS ${CMAKE_USE_PTHREADS_INIT} CACHE INTERNAL "Threading support")
if(CPPUTEST_HAVE_PTHREADS)
    target_compile_definitions(CppUTest PUBLIC CPPUTEST_HAVE_PTHREAD_MUTEX_LOCK)
    target_link_libraries(CppUTest PRIVATE Threads::Threads)
endif()

target_link_libraries(CppUTest
    PRIVATE
        $<$<BOOL:${WIN32}>:winmm>
)

add_library(CppUTest::CppUTest ALIAS CppUTest)

if(NOT CPPUTEST_MEM_LEAK_DETECTION_DISABLED)
    if(MSVC)
        set(force_include "/FI")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "IAR")
        set(force_include "--preinclude=")
    else()
        set(force_include "-include")
    endif()
    target_compile_options(CppUTest
        PUBLIC
            $<$<COMPILE_LANGUAGE:C,CXX>:${force_include}CppUTest/MemoryLeakDetectorForceInclude.h>
    )
endif()

# Installation
if(PROJECT_IS_TOP_LEVEL)
    install(
        TARGETS CppUTest
        EXPORT CppUTestTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/include/CppUTest"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif()
