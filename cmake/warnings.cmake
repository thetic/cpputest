include(CheckCompilerFlag)

# MSVC and ClangCL require MSVC-style flag syntax.
# CMAKE_CXX_COMPILER_FRONTEND_VARIANT distinguishes ClangCL (frontend=MSVC)
# from plain Clang (frontend=GNU), available since CMake 3.14.
if(
    (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR
    (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
)
    # /Wall on MSVC/ClangCL is equivalent to -Weverything; use /W4 instead.
    set(_warning_c_flags   /W4)
    set(_warning_cxx_flags /W4)
elseif(
    (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
)
    set(_common_flags
        -Wall
        -Wextra
        -pedantic
        -Wshadow
        -Wswitch-default
        -Wswitch-enum
        -Wconversion
        -Wsign-conversion
        -Wmissing-include-dirs
        -Wno-padded
        -Wno-disabled-macro-expansion
        -Wreserved-id-macro
        -Wreserved-identifier
        -Wno-long-long
        -Wno-unsafe-buffer-usage
    )

    set(_warning_c_flags   ${_common_flags} -Wstrict-prototypes)
    set(_warning_cxx_flags ${_common_flags}
        -Woverloaded-virtual
        -Wno-global-constructors
        -Wno-exit-time-destructors
        -Wno-weak-vtables
        -Wno-old-style-cast
    )

    if(DEFINED CMAKE_CXX_STANDARD AND NOT CMAKE_CXX_STANDARD EQUAL 98)
        list(APPEND _warning_cxx_flags
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-c++14-compat
            -Wno-inconsistent-missing-destructor-override
            -Wsuggest-override
        )
    endif()

    # Prevent -Werror (set via CFLAGS/CXXFLAGS in presets) from causing
    # unknown-flag checks to fail during configuration.
    set(CMAKE_REQUIRED_FLAGS "-Wno-error")
endif()

function(_add_checked_warning_flags lang flags)
    get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)
    if(NOT "${lang}" IN_LIST languages)
        return()
    endif()
    set(CMAKE_REQUIRED_QUIET TRUE)
    message(CHECK_START "Checking supported ${lang} warning options")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")

    set(_supported)
    foreach(flag IN LISTS flags)
        string(MAKE_C_IDENTIFIER "WARN_${lang}_${flag}" _var)
        cmake_check_compiler_flag(${lang} "${flag}" ${_var})
        if(${_var})
            list(APPEND _supported "${flag}")
        endif()
    endforeach()

    list(POP_BACK CMAKE_MESSAGE_INDENT)
    message(CHECK_PASS "${_supported}")
    add_compile_options("$<$<COMPILE_LANGUAGE:${lang}>:${_supported}>")
endfunction()

_add_checked_warning_flags(C   "${_warning_c_flags}")
_add_checked_warning_flags(CXX "${_warning_cxx_flags}")
