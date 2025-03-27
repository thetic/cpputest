cmake_minimum_required(VERSION 3.21)

project(CppUTest
    VERSION 4.0
)

include(CTest)

option(CPPUTEST_STD_CPP_LIB_DISABLED "Use the standard C++ library")
option(CPPUTEST_EXTENSIONS "Use the CppUTest extension library" ON)

include(CMakeDependentOption)
cmake_dependent_option(CPPUTEST_BUILD_TESTING "Compile and make tests for CppUTest"
    ${PROJECT_IS_TOP_LEVEL} "BUILD_TESTING" OFF)
cmake_dependent_option(CPPUTEST_EXAMPLES "Compile and make examples?"
    ${PROJECT_IS_TOP_LEVEL} "CPPUTEST_EXTENSIONS;NOT CPPUTEST_STD_CPP_LIB_DISABLED" OFF)

if(NOT DEFINED CPPUTEST_PLATFORM)
    if(MSVC)
        set(CPPUTEST_PLATFORM VisualCpp)
    else()
        set(CPPUTEST_PLATFORM Gcc)
    endif()
endif()
set(CPPUTEST_PLATFORM "${CPPUTEST_PLATFORM}" CACHE STRING "Platform implementation")
set_property(
    CACHE CPPUTEST_PLATFORM
    PROPERTY STRINGS
        Gcc VisualCpp
)

include(CheckCXXSymbolExists)
check_cxx_symbol_exists(fopen_s "stdio.h" CPPUTEST_HAVE_SECURE_STDLIB)
cmake_dependent_option(CPPUTEST_USE_SECURE_STDLIB "Use MSVC safe functions"
    ON "WIN32;CPPUTEST_HAVE_SECURE_STDLIB" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
if(NOT PROJECT_IS_TOP_LEVEL)
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
endif()

if(PROJECT_IS_TOP_LEVEL)
    include(cmake/warnings.cmake)
endif()
include(GNUInstallDirs)

add_subdirectory(CppUTest)

if(CPPUTEST_EXTENSIONS)
    add_subdirectory(CppUTestExt)
endif()

if (CPPUTEST_EXAMPLES)
    add_subdirectory(examples)
endif()

if(PROJECT_IS_TOP_LEVEL)
    include(cmake/install.cmake)
endif()
