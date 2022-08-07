#include "CppUTest/TestHarness.h"
#include "CppUTest/TestMemoryAllocator.h"
#include "CppUTest/MemoryLeakDetector.h"
#include "CppUTest/TestOutput.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTest/PlatformSpecificFunctions.h"
#include "CppUTest/TestTestingFixture.h"
#include "AllocationInCppFile.h"

#include "CppUTest/TestHarness_c.h"
#include "AllocationInCFile.h"

#if defined(__GNUC__) && __GNUC__ >= 11
# define NEEDS_DISABLE_FREE_NON_HEEP_WARNING
#endif /* GCC >= 11 */


TEST_GROUP(BasicBehavior)
{
};

TEST(BasicBehavior, CanDeleteNullPointers)
{
    delete (char*) NULLPTR;
    delete [] (char*) NULLPTR;
}

TEST_GROUP(MemoryLeakOverridesToBeUsedInProductionCode)
{
    MemoryLeakDetector* memLeakDetector;
    void setup() _override
    {
        memLeakDetector = MemoryLeakWarningPlugin::getGlobalDetector();
    }

};

TEST(MemoryLeakOverridesToBeUsedInProductionCode, UseNativeMallocByTemporarlySwitchingOffMalloc)
{
    size_t memLeaks = memLeakDetector->totalMemoryLeaks(mem_leak_period_checking);
#if CPPUTEST_USE_STD_C_LIB
    void* memory = malloc(10);
    LONGS_EQUAL(memLeaks, memLeakDetector->totalMemoryLeaks(mem_leak_period_checking));
    free (memory);
#else
    void* memory = PlatformSpecificMalloc(10);
    LONGS_EQUAL(memLeaks, memLeakDetector->totalMemoryLeaks(mem_leak_period_checking));
    PlatformSpecificFree (memory);
#endif
}

/* TEST... allowing for a new overload in a class */
class NewDummyClass
{
public:
    static bool overloaded_new_called;

    void* operator new (size_t size)
    {
        overloaded_new_called = true;
        return malloc(size);
    }
    void dummyFunction()
    {
        char* memory = new char;
        delete memory;
    }
};
bool NewDummyClass::overloaded_new_called = false;

TEST(MemoryLeakOverridesToBeUsedInProductionCode, NoSideEffectsFromTurningOffNewMacros)
{
    /*
     * Interesting effect of wrapping the operator new around the macro is
     * that the actual new that is called is a different one than expected.
     *
     * The overloaded operator new doesn't actually ever get called.
     *
     * This might come as a surprise, so it is important to realize!
     */
    NewDummyClass dummy;
    dummy.dummyFunction();
    // CHECK(dummy.overloaded_new_called);
}

TEST(MemoryLeakOverridesToBeUsedInProductionCode, UseNativeNewByTemporarlySwitchingOffNew)
{
    char* memory = new char[10];
    delete [] memory;
}


TEST(MemoryLeakOverridesToBeUsedInProductionCode, MemoryOverridesAreDisabled)
{
    char* leak = newAllocation();
    STRCMP_EQUAL("No memory leaks were detected.", memLeakDetector->report(mem_leak_period_checking));
    delete leak;
}

TEST_GROUP(OutOfMemoryTestsForOperatorNew)
{
    TestMemoryAllocator* no_memory_allocator;
    GlobalMemoryAllocatorStash memoryAllocatorStash;
    void setup() _override
    {
        memoryAllocatorStash.save();
        no_memory_allocator = new NullUnknownAllocator;
        setCurrentNewAllocator(no_memory_allocator);
        setCurrentNewArrayAllocator(no_memory_allocator);
    }

    void teardown() _override
    {
        memoryAllocatorStash.restore();
        delete no_memory_allocator;
    }
};
