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

if(PROJECT_IS_TOP_LEVEL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    set(CPPUTEST_CONFIG_DEST "${CMAKE_INSTALL_LIBDIR}/cmake/CppUTest")

    # Package configuration
    configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/CppUTestConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/CppUTestConfig.cmake"
        INSTALL_DESTINATION ${CPPUTEST_CONFIG_DEST}
    )

    # Package version
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/CppUTestConfigVersion.cmake"
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/CppUTestConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/CppUTestConfigVersion.cmake"
            "cmake/CppUTest.cmake"
            "cmake/CppUTestAddTests.cmake"
        DESTINATION ${CPPUTEST_CONFIG_DEST}
    )
    install(
        EXPORT CppUTestTargets
        NAMESPACE CppUTest::
        DESTINATION ${CPPUTEST_CONFIG_DEST}
    )
endif()
