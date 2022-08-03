cmake_minimum_required(VERSION 3.17)

project(CppUTest
    VERSION 5.0
    LANGUAGES CXX C
)

include(CTest)

if(${CMAKE_VERSION} VERSION_LESS 3.21)
    if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
        set(PROJECT_IS_TOP_LEVEL TRUE)
    else()
        set(PROJECT_IS_TOP_LEVEL FALSE)
    endif()
endif()

include(CheckTypeSize)
check_type_size("long long" LONGLONG LANGUAGE CXX)

include(CMakeDependentOption)

option(CPPUTEST_EXTENSIONS "Use the CppUTest extension library" ON)
option(CPPUTEST_STD_C "Use the standard C library" ON)
cmake_dependent_option(CPPUTEST_USE_LONG_LONG
    "Support long long"
    ON "HAVE_LONGLONG" OFF
)
cmake_dependent_option(CPPUTEST_BUILD_TESTING
    "Build CppUTest tests"
    ${PROJECT_IS_TOP_LEVEL} "BUILD_TESTING;CPPUTEST_EXTENSIONS;CPPUTEST_STD_C" OFF
)
cmake_dependent_option(CPPUTEST_STD_CPP
    "Use the standard C++ library"
    ON "CPPUTEST_STD_C" OFF
)
cmake_dependent_option(CPPUTEST_MEMORY_LEAK_DETECTION
    "Enable memory leak detection"
    ON "CPPUTEST_STD_C" OFF
)
cmake_dependent_option(CPPUTEST_EXAMPLES
    "Build examples"
    ${PROJECT_IS_TOP_LEVEL} "CPPUTEST_EXTENSIONS;CPPUTEST_STD_CPP" OFF
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
if(NOT PROJECT_IS_TOP_LEVEL)
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
endif()

add_subdirectory(CppUTest)

if(CPPUTEST_EXTENSIONS)
    add_subdirectory(CppUTestExt)
endif()

if(CPPUTEST_EXAMPLES)
    add_subdirectory(examples)
endif()
