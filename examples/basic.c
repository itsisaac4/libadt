#include <stdio.h>
#include <stdlib.h>

#include "libadt/dynamic_array.h"

int main(void)
{
    int initialValues[] = {10, 20, 30};
    DynamicArray_t numbers = {0};

    if (!DA_INIT_FROM(&numbers, initialValues))
    {
        fputs("Failed to initialize the array.\n", stderr);
        return EXIT_FAILURE;
    }

    puts("Initial array:");
    da_Print(&numbers);

    int appended = 40;
    int prepended = 5;
    int inserted = 15;

    if (!da_Append(&numbers, &appended) ||
        !da_Prepend(&numbers, &prepended) ||
        !da_Insert(&numbers, 2, &inserted))
    {
        da_Destroy(&numbers);
        return EXIT_FAILURE;
    }

    puts("After prepend, insert, and append:");
    da_Print(&numbers);

    int replacement = 99;
    if (!da_Set(&numbers, 3, &replacement))
    {
        da_Destroy(&numbers);
        return EXIT_FAILURE;
    }

    int retrieved = 0;
    if (da_Get(&numbers, 3, &retrieved))
    {
        printf("Value at index 3: %d\n", retrieved);
    }

    int target = 20;
    size_t index = 0;
    if (da_IndexOf(&numbers, &target, &index))
    {
        printf("Found %d at index %zu.\n", target, index);
    }

    puts("Final array:");
    da_Print(&numbers);

    da_Destroy(&numbers);
    return EXIT_SUCCESS;
}
