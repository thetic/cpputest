cmake_minimum_required(VERSION 3.17)

project(CppUTest
    VERSION 5.0
    LANGUAGES CXX C
)

option(CPPUTEST_MEMORY_LEAK_DETECTION "Enable memory leak detection")

include(CTest)

add_subdirectory(CppUTest)
add_subdirectory(CppUTestExt)
add_subdirectory(examples)
