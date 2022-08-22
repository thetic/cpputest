#include "CppUTest/TestHarness.h"

void functionWithUnusedParameter(void* PUNUSED(unlessParamater));

void functionWithUnusedParameter(void* PUNUSED(unlessParamater))
{
}

int setup_teardown_was_called_in_test_group_in_C;
int test_was_called_in_test_group_in_C;

TEST_GROUP_C_SETUP(TestGroupInC)
{
    setup_teardown_was_called_in_test_group_in_C++;
}

TEST_GROUP_C_TEARDOWN(TestGroupInC)
{
    setup_teardown_was_called_in_test_group_in_C--;
    CHECK_C(test_was_called_in_test_group_in_C == 1);
    test_was_called_in_test_group_in_C--;
}

TEST_C(TestGroupInC, checkThatTheTestHasRun)
{
    test_was_called_in_test_group_in_C++;
}

IGNORE_TEST_C(TestGroupInC, ignoreMacroForCFile)
{
    test_was_called_in_test_group_in_C++;
}
