
#include "CppUTest/TestHarness.hpp"

#include <memory>

TEST_GROUP(StandardCppLibrary) {};

#if defined(__cplusplus) && __cplusplus >= 201402L

TEST(StandardCppLibrary, UniquePtrConversationToBool)
{
    auto const aNull = std::unique_ptr<int>(nullptr);
    CHECK_FALSE(aNull);

    auto const notNull = std::make_unique<int>(1);
    CHECK_TRUE(notNull);
}

#endif
