add_executable(AllTests
    include/MockPrinter.hpp
    src/AllTests.cpp
    src/CircularBufferTest.cpp
    src/EventDispatcherTest.cpp
    src/HelloTest.cpp
    src/MockDocumentationTest.cpp
    src/PrinterTest.cpp
)

target_include_directories(AllTests PRIVATE include)

target_link_libraries(AllTests
    PRIVATE
        CppUTest::CppUTestExt
        ApplicationLib
)

target_link_options(AllTests
    PRIVATE
        $<$<BOOL:${MINGW}>:-static>
)

if(CPPUTEST_DISCOVER)
    include(CppUTest)
    CppUTest_discover_tests(AllTests
        TEST_PREFIX Example::
        DETAILED
    )
else()
    add_test(
        NAME Example
        COMMAND AllTests
    )
endif()
