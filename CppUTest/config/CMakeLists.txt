# TODO just use target_compile_definitions
if(NOT CPPUTEST_MEMORY_LEAK_DETECTION)
    unset(CPPUTEST_MEM_LEAK_DETECTION_DISABLED)
else()
    set(CPPUTEST_MEM_LEAK_DETECTION_DISABLED ON)
endif()

if(CPPUTEST_STD_C)
    unset(CPPUTEST_STD_C_LIB_DISABLED)
else()
    set(CPPUTEST_STD_C_LIB_DISABLED ON)
endif()

if(CPPUTEST_STD_CPP)
    unset(CPPUTEST_STD_CPP_LIB_DISABLED)
else()
    set(CPPUTEST_STD_CPP_LIB_DISABLED ON)
endif()

include(CheckCXXSymbolExists)
check_cxx_symbol_exists(INFINITY "math.h" HAS_INF)
if(HAS_INF)
    unset(CPPUTEST_NO_INF)
else()
    set(CPPUTEST_NO_INF ON)
endif()

check_cxx_symbol_exists(NAN "math.h" HAS_NAN)
if(HAS_INF)
    unset(CPPUTEST_NO_NAN)
else()
    set(CPPUTEST_NO_NAN ON)
endif()

include(CheckCXXSourceCompiles)
check_cxx_source_compiles(
    [[#include "fenv.h"
    int main() {feclearexcept(FE_ALL_EXCEPT);}]]
    CPPUTEST_HAVE_FENV
)

configure_file(CppUTestGeneratedConfig.h.cmake generated/CppUTestGeneratedConfig.h)

target_sources(CppUTest
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/CppUTestGeneratedConfig.h"
)

target_compile_definitions(CppUTest PUBLIC HAVE_CONFIG_H)

target_include_directories(CppUTest PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
