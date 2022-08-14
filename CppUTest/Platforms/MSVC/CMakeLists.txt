target_sources(CppUTest PRIVATE src/UTestPlatform.cpp)

target_link_libraries(CppUTest
    PRIVATE
        winmm
)
