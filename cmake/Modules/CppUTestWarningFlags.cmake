unset(WARNING_COMMON_FLAGS)
unset(WARNING_C_FLAGS)
unset(WARNING_CXX_FLAGS)

if(
    (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
)
    set(WARNING_COMMON_FLAGS
        -Wall
        -Wextra
        -pedantic
        -Wshadow
        -Wswitch-default
        -Wswitch-enum
        -Wconversion
        -Wsign-conversion
    )

    set(WARNING_C_FLAGS
        -Wstrict-prototypes
        -Wno-long-long
    )

    set(WARNING_CXX_FLAGS
        -Wtemplates
        -Woverloaded-virtual
    )

    if(DEFINED CMAKE_CXX_STANDARD AND NOT CMAKE_CXX_STANDARD EQUAL 98)
        list(APPEND WARNING_CXX_FLAGS
            -Wsuggest-override
        )
    endif()

    set(CMAKE_REQUIRED_FLAGS "-Wno-error")
endif()

include(CheckCCompilerFlag)
foreach(flag IN LISTS WARNING_COMMON_FLAGS WARNING_C_FLAGS)
    check_c_compiler_flag("${flag}" WARNING_C_FLAG_${flag})
    if(WARNING_C_FLAG_${flag})
        add_compile_options("$<$<COMPILE_LANGUAGE:C>:${flag}>")
    endif()
endforeach()

include(CheckCXXCompilerFlag)
foreach(flag IN LISTS WARNING_C_FLAGS WARNING_CXX_FLAGS)
    string(REPLACE "++" "xx" WARNING_CXX_FLAG_VAR "WARNING_CXX_FLAG_${flag}")
    check_cxx_compiler_flag("${flag}" ${WARNING_CXX_FLAG_VAR})
    if(${WARNING_CXX_FLAG_VAR})
        add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${flag}>")
    endif()
endforeach()
