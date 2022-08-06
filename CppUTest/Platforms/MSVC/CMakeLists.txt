include(CMakeDependentOption)
cmake_dependent_option(CPPUTEST_SECURE_LIB
    "Use MSVC safe functions"
    ON "MSVC" OFF
)

target_sources(CppUTest PRIVATE src/UTestPlatform.cpp)

target_compile_definitions(CppUTest
    PRIVATE
        $<$<BOOL:${CPPUTEST_SECURE_LIB}>:STDC_WANT_SECURE_LIB>
)

target_link_libraries(CppUTest
    PRIVATE
        winmm
)
