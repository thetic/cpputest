add_library(CppUTestExt
    include/CppUTestExt/IEEE754ExceptionsPlugin.hpp
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
    src/IEEE754ExceptionsPlugin.cpp
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

target_compile_definitions(CppUTestExt
    PRIVATE
        $<$<BOOL:${MSVC}>:_CRT_SECURE_NO_WARNINGS>
)

target_link_libraries(CppUTestExt PUBLIC CppUTest::CppUTest)

add_library(CppUTest::CppUTestExt ALIAS CppUTestExt)

if(CPPUTEST_BUILD_TESTING)
    add_subdirectory(tests)
endif()
