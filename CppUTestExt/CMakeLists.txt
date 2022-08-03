add_library(CppUTestExt
    include/CppUTestExt/CodeMemoryReportFormatter.h
    include/CppUTestExt/GMock.h
    include/CppUTestExt/GTest.h
    include/CppUTestExt/GTestConvertor.h
    include/CppUTestExt/GTestSupport.h
    include/CppUTestExt/IEEE754ExceptionsPlugin.h
    include/CppUTestExt/MemoryReportAllocator.h
    include/CppUTestExt/MemoryReportFormatter.h
    include/CppUTestExt/MemoryReporterPlugin.h
    include/CppUTestExt/MockActualCall.h
    include/CppUTestExt/MockCheckedActualCall.h
    include/CppUTestExt/MockCheckedExpectedCall.h
    include/CppUTestExt/MockExpectedCall.h
    include/CppUTestExt/MockExpectedCallsList.h
    include/CppUTestExt/MockFailure.h
    include/CppUTestExt/MockNamedValue.h
    include/CppUTestExt/MockSupport.h
    include/CppUTestExt/MockSupportPlugin.h
    include/CppUTestExt/MockSupport_c.h
    include/CppUTestExt/OrderedTest.h
    src/CodeMemoryReportFormatter.cpp
    src/GTest.cpp
    src/IEEE754ExceptionsPlugin.cpp
    src/MemoryReportAllocator.cpp
    src/MemoryReportFormatter.cpp
    src/MemoryReporterPlugin.cpp
    src/MockActualCall.cpp
    src/MockExpectedCall.cpp
    src/MockExpectedCallsList.cpp
    src/MockFailure.cpp
    src/MockNamedValue.cpp
    src/MockSupport.cpp
    src/MockSupportPlugin.cpp
    src/MockSupport_c.cpp
    src/OrderedTest.cpp
)

target_include_directories(CppUTestExt PUBLIC include)

target_link_libraries(CppUTestExt PUBLIC CppUTest::CppUTest)

add_library(CppUTest::CppUTestExt ALIAS CppUTestExt)

if(CPPUTEST_BUILD_TESTING)
    add_subdirectory(tests)
endif()
