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
if(HAVE_LONGLONG)
    set(CPPUTEST_HAVE_LONG_LONG_INT ON)
else()
    unset(CPPUTEST_HAVE_LONG_LONG_INT)
endif()

include(CMakeDependentOption)
option(CPPUTEST_MEMORY_LEAK_DETECTION "Enable memory leak detection")
cmake_dependent_option(CPPUTEST_USE_LONG_LONG "Support long long"
    ON "CPPUTEST_HAVE_LONG_LONG_INT" OFF)
option(CPPUTEST_EXAMPLES "Build examples" ${PROJECT_IS_TOP_LEVEL})
option(CPPUTEST_EXTENSIONS "Use the CppUTest extension library" ON)
cmake_dependent_option(CPPUTEST_BUILD_TESTING "Build CppUTest tests"
    ${PROJECT_IS_TOP_LEVEL} "BUILD_TESTING" OFF)
option(CPPUTEST_STD_C "Use the standard C library" ON)
option(CPPUTEST_STD_CPP "Use the standard C++ library" ON)

add_subdirectory(CppUTest)

if(CPPUTEST_EXTENSIONS)
    add_subdirectory(CppUTestExt)
endif()

if(CPPUTEST_EXAMPLES)
    add_subdirectory(examples)
endif()
