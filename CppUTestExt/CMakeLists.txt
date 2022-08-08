add_library(CppUTestExt
    include/CppUTestExt/CodeMemoryReportFormatter.hpp
    include/CppUTestExt/IEEE754ExceptionsPlugin.hpp
    include/CppUTestExt/MemoryReportAllocator.hpp
    include/CppUTestExt/MemoryReportFormatter.hpp
    include/CppUTestExt/MemoryReporterPlugin.hpp
    include/CppUTestExt/MockActualCall.hpp
    include/CppUTestExt/MockCheckedActualCall.hpp
    include/CppUTestExt/MockCheckedExpectedCall.hpp
    include/CppUTestExt/MockExpectedCall.hpp
    include/CppUTestExt/MockExpectedCallsList.hpp
    include/CppUTestExt/MockFailure.hpp
    include/CppUTestExt/MockNamedValue.hpp
    include/CppUTestExt/MockSupport.hpp
    include/CppUTestExt/MockSupportPlugin.hpp
    include/CppUTestExt/MockSupport.h
    include/CppUTestExt/OrderedTest.hpp
    src/CodeMemoryReportFormatter.cpp
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
