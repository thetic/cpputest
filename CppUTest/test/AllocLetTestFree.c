#include "AllocLetTestFree.h"

#include <stddef.h>
#include <stdlib.h>

typedef struct AllocLetTestFreeStruct
{
    int placeHolderForHiddenStructElements;
} AllocLetTestFreeStruct;

AllocLetTestFree AllocLetTestFree_Create(void)
{
    size_t count = 1;
    AllocLetTestFree self = (AllocLetTestFree)calloc(count, sizeof(AllocLetTestFreeStruct));
    return self;
}

void AllocLetTestFree_Destroy(AllocLetTestFree self)
{
    AllocLetTestFree no_use = self;
    self = NULL;
    self = no_use;
    (void)self;

}
