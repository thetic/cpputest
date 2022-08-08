#include "AllocLetTestFree.h"

#include "CppUTest/TestHarness.hpp"

/*
 * This test makes sure that memory leak malloc macros are forced into .cpp and .c files
 */
TEST_GROUP(AllocLetTestFree)
{
    AllocLetTestFree allocLetTestFree;

    void setup() override
    {
        allocLetTestFree = AllocLetTestFree_Create();
    }

    void teardown() override
    {
        AllocLetTestFree_Destroy(allocLetTestFree);
    }
};

TEST(AllocLetTestFree, Create)
{
    free(allocLetTestFree);
}
