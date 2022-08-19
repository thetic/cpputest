target_sources(CppUTest PRIVATE src/UTestPlatform.cpp)

target_compile_definitions(CppUTest
    PRIVATE
        _CRT_SECURE_NO_WARNINGS
)

target_link_libraries(CppUTest
    PRIVATE
        winmm
)
