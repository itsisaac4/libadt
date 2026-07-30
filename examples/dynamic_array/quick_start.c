#include <stdio.h>
#include <stdlib.h>

#include "libadt/dynamic_array.h"

int main(void)
{
    int values[] = {10, 20, 30};
    DynamicArray_t numbers = {0};

    if (!DA_INIT_FROM(&numbers, values) ||
        !da_Prepend(&numbers, 5) ||
        !da_Append(&numbers, 40))
    {
        da_Destroy(&numbers);
        return EXIT_FAILURE;
    }

    adt_Print(&numbers);

    int target = 20;
    size_t index = 0;

    if (da_IndexOf(&numbers, target, &index))
    {
        printf("%d is at index %zu.\n", target, index);
    }

    da_Destroy(&numbers);
    return EXIT_SUCCESS;
}
