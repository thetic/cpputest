# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

if(NOT CMAKE_SCRIPT_MODE_FILE)
    message(FATAL_ERROR
        "Do not use this module directly. Use \"include(CppUTest)\" instead."
    )
endif()

set(prefix "${TEST_PREFIX}")
set(suffix "${TEST_SUFFIX}")
set(extra_args ${TEST_EXTRA_ARGS})
set(properties ${TEST_PROPERTIES})
set(reporter ${TEST_REPORTER})
set(script)
set(suite)
set(tests)

function(add_command NAME)
    set(_args "")
    # use ARGV* instead of ARGN, because ARGN splits arrays into multiple arguments
    math(EXPR _last_arg ${ARGC}-1)
    foreach(_n RANGE 1 ${_last_arg})
        set(_arg "${ARGV${_n}}")
        if(_arg MATCHES "[^-./:a-zA-Z0-9_]")
            set(_args "${_args} [==[${_arg}]==]") # form a bracket_argument
        else()
            set(_args "${_args} ${_arg}")
        endif()
    endforeach()
    set(script "${script}${NAME}(${_args})\n" PARENT_SCOPE)
endfunction()

# Run test executable to get list of available tests
if(NOT EXISTS "${TEST_EXECUTABLE}")
    message(FATAL_ERROR
        "Specified test executable '${TEST_EXECUTABLE}' does not exist"
    )
endif()

execute_process(
    COMMAND ${TEST_EXECUTOR} "${TEST_EXECUTABLE}" -lg
    OUTPUT_VARIABLE output
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${TEST_WORKING_DIR}"
)
if(NOT ${result} EQUAL 0)
    message(FATAL_ERROR
        "Error running test executable '${TEST_EXECUTABLE}':\n"
        "  Result: ${result}\n"
        "  Output: ${output}\n"
    )
endif()

string(REPLACE " " ";" output "${output}")

set(reporters normal eclipse teamcity junit)
if(reporter)
    if(report IN_LIST reporters)
        message(FATAL_ERROR
            "\"${reporter}\" is not a valid reporter!\n"
        )
    endif()
endif()

# Prepare reporter
if(reporter)
    set(reporter_arg "-o${reporter}")
endif()

# Parse output
foreach(test ${output})
    # add to script
    add_command(add_test
        "${prefix}${test}${suffix}"
        ${TEST_EXECUTOR}
        "${TEST_EXECUTABLE}"
        -g "${test}"
        ${extra_args}
        "${reporter_arg}"
        "${output_dir_arg}"
    )
    add_command(set_tests_properties
        "${prefix}${test}${suffix}"
        PROPERTIES
        WORKING_DIRECTORY "${TEST_WORKING_DIR}"
        ${properties}
    )

    if(environment_modifications)
        add_command(set_tests_properties
            "${prefix}${test}${suffix}"
            PROPERTIES
            ENVIRONMENT_MODIFICATION "${environment_modifications}")
    endif()

    list(APPEND tests "${prefix}${test}${suffix}")
endforeach()

# Create a list of all discovered tests, which users may use to e.g. set
# properties on the tests
add_command(set ${TEST_LIST} ${tests})

# Write CTest script
file(WRITE "${CTEST_FILE}" "${script}")
