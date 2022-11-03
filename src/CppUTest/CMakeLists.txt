add_library(${CppUTestLibName}
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
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestHarness.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/Utest.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/MemoryLeakWarningPlugin.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/TestHarness_c.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/UtestMacros.h
        ${PROJECT_SOURCE_DIR}/include/CppUTest/SimpleMutex.h
)

#[[Set CPPUTEST_PLATFORM in a parent CMakeLists.txt if reusing one of the provided platforms, else supply the missing definitions]]
if(CPPUTEST_PLATFORM)
    target_sources(${CppUTestLibName}
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/../Platforms/${CPPUTEST_PLATFORM}/UtestPlatform.cpp
    )
    target_include_directories(${CppUTestLibName}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../include/Platforms/${CPPUTEST_PLATFORM}>
    )
endif()

if (MINGW)
    include(CheckStructHasMember)
    check_struct_has_member("struct timespec" tv_sec time.h HAVE_STRUCT_TIMESPEC LANGUAGE CXX)
endif()

configure_file (
    config.h.cmake
    "${PROJECT_BINARY_DIR}/generated/CppUTestGeneratedConfig.h"
)

#[[Arrange for the include directory to be added to the include paths of any CMake target depending on CppUTest.]]
target_include_directories(${CppUTestLibName}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

check_cxx_symbol_exists(gettimeofday "sys/time.h" HAVE_GETTIMEOFDAY)
target_compile_definitions(${CppUTestLibName}
    PUBLIC
        HAVE_CONFIG_H
    PRIVATE
        $<$<BOOL:${HAVE_GETTIMEOFDAY}>:CPPUTEST_HAVE_GETTIMEOFDAY>
        $<$<BOOL:${CPPUTEST_USE_SECURE_STDLIB}>:STDC_WANT_SECURE_LIB>
        # Apply workaround for MinGW timespec redefinition (pthread.h / time.h).
        $<$<BOOL:${HAVE_STRUCT_TIMESPEC}>:_TIMESPEC_DEFINED>
)

if (WIN32)
    target_link_libraries(${CppUTestLibName} winmm)
endif (WIN32)

add_library(CppUTest::CppUTest ALIAS ${CppUTestLibName})

if(NOT CPPUTEST_MEM_LEAK_DETECTION_DISABLED)
    if(MSVC)
        set(force_include "/FI")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "IAR")
        set(force_include "--preinclude ")
    else()
        set(force_include "-include")
    endif()
    target_compile_options(${CppUTestLibName}
        PUBLIC
            "$<$<COMPILE_LANGUAGE:C,CXX>:${force_include}CppUTest/MemoryLeakDetectorMallocMacros.h>"
            "$<$<COMPILE_LANGUAGE:CXX>:${force_include}CppUTest/MemoryLeakDetectorNewMacros.h>"
    )
endif()

# Installation
if(PROJECT_IS_TOP_LEVEL)
    install(
        TARGETS ${CppUTestLibName}
        EXPORT CppUTestTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/include/CppUTest"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif()
