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

include(CMakeDependentOption)

cmake_dependent_option(CPPUTEST_BUILD_TESTING
    "Build CppUTest tests"
    ${PROJECT_IS_TOP_LEVEL} "BUILD_TESTING" OFF
)
cmake_dependent_option(CPPUTEST_EXAMPLES
    "Build examples"
    ${PROJECT_IS_TOP_LEVEL} "CPPUTEST_BUILD_TESTING" OFF
)
cmake_dependent_option(CPPUTEST_DISCOVER
    "Dynamically register tests at build-time"
    # Depends on actually building the tests
    ON "CPPUTEST_BUILD_TESTING" OFF
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
if(NOT PROJECT_IS_TOP_LEVEL)
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
endif()

add_subdirectory(CppUTest)
add_subdirectory(CppUTestExt)

if(CPPUTEST_EXAMPLES)
    add_subdirectory(examples)
endif()
