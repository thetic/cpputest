cmake_minimum_required(VERSION 3.17)

project(CppUTestExample
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

if(PROJECT_IS_TOP_LEVEL)
    find_package(CppUTest REQUIRED)
endif()

add_subdirectory(ApplicationLib)
add_subdirectory(AllTests)
