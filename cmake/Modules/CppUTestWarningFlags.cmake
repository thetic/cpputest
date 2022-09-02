if (MSVC)
    set(CPPUTEST_C_WARNING_FLAGS "/WX")
    set(CPPUTEST_CXX_WARNING_FLAGS "/WX /wd4290")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "OpenWatcom")
    # Disable W303 unreferenced parameter - this cannot be suppressed in C.
    set(CPPUTEST_C_WARNING_FLAGS "-we -w3 -wcd=303")
    # Disable W013 unreachable code - it overreacts to CHECK_EQUAL macros
    # Disable W367 conditional expression in if statement is always true - same
    # Disable W368 conditional expression in if statement is always false - same
    # Disable W391 assignment found in boolean expression - we don't care
    set(CPPUTEST_CXX_WARNING_FLAGS "-we -w3 -wcd=13 -wcd=367 -wcd=368 -wcd391 -wcd=472")
else()
    include(CheckCCompilerFlag)
    include(CheckCXXCompilerFlag)

    macro(check_and_append_c_warning_flags)
      foreach (flag ${ARGN})
        check_c_compiler_flag("-${flag}" WARNING_C_FLAG_${flag})
        if (WARNING_C_FLAG_${flag})
            set(CPPUTEST_C_WARNING_FLAGS "${CPPUTEST_C_WARNING_FLAGS} -${flag}")
        endif (WARNING_C_FLAG_${flag})
      endforeach (flag)
    endmacro(check_and_append_c_warning_flags)

    macro(check_and_append_cxx_warning_flags)
      foreach (flag ${ARGN})
        string(REPLACE "++" "xx" WARNING_CXX_FLAG_VAR "WARNING_CXX_FLAG_${flag}")
        check_cxx_compiler_flag("-${flag}" ${WARNING_CXX_FLAG_VAR})
        if (${WARNING_CXX_FLAG_VAR})
            set(CPPUTEST_CXX_WARNING_FLAGS "${CPPUTEST_CXX_WARNING_FLAGS} -${flag}")
        endif (${WARNING_CXX_FLAG_VAR})
      endforeach (flag)
    endmacro(check_and_append_cxx_warning_flags)

    if (NOT GMOCK AND NOT REAL_GTEST)
      list(APPEND WARNING_C_FLAGS Werror pedantic-errors)
    endif (NOT GMOCK AND NOT REAL_GTEST)

    set(WARNING_C_FLAGS
        Wall
        Wextra
        pedantic
        Wshadow
        Wswitch-default
        Wswitch-enum
        Wconversion
        Wsign-conversion
        Wno-padded
        Wno-disabled-macro-expansion
        Wno-reserved-id-macro
        Wno-keyword-macro
        Wno-long-long
        )

    if (CPPUTEST_WERROR)
        list(APPEND WARNING_C_FLAGS Werror)
    endif ()


    set(WARNING_C_ONLY_FLAGS
        Wstrict-prototypes
        )

    set(WARNING_CXX_FLAGS
        ${WARNING_C_FLAGS}
        Woverloaded-virtual
        Wno-global-constructors
        Wno-exit-time-destructors
        Wno-weak-vtables
        Wno-old-style-cast
        )

    if (DEFINED CMAKE_CXX_STANDARD AND NOT CMAKE_CXX_STANDARD EQUAL 98)
        set(WARNING_CXX_FLAGS
            ${WARNING_CXX_FLAGS}
            Wno-c++98-compat
            Wno-c++98-compat-pedantic
            Wno-c++14-compat
            Wno-inconsistent-missing-destructor-override
            Wsuggest-override
        )
    endif ()

    check_and_append_c_warning_flags(${WARNING_C_FLAGS})
    check_and_append_c_warning_flags(${WARNING_C_ONLY_FLAGS})
    check_and_append_cxx_warning_flags(${WARNING_CXX_FLAGS})

endif()
