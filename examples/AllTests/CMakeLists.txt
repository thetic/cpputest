add_executable(AllTests
    include/AllTests.h
    include/MockPrinter.h
    src/AllTests.cpp
    src/CircularBufferTest.cpp
    src/EventDispatcherTest.cpp
    src/FEDemoTest.cpp
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

add_test(
    NAME Example
    COMMAND AllTests
)
