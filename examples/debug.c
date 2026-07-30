#include <stdio.h>
#include <stdlib.h>

#include "libadt/dynamic_array.h"

int main(void)
{
    DynamicArray_t numbers = {0};

    if (!DA_INIT(&numbers, int))
    {
        fputs("Failed to initialize the array.\n", stderr);
        return EXIT_FAILURE;
    }

    int values[] = {2, 4, 8, 16};
    for (size_t i = 0; i < ARRAY_COUNT(values); ++i)
    {
        if (!da_Append(&numbers, &values[i]))
        {
            da_Destroy(&numbers);
            return EXIT_FAILURE;
        }
    }

    puts("Normal print:");
    da_Print(&numbers);

    puts("\nDebug print:");
    DA_DEBUG(numbers);

    da_Destroy(&numbers);
    return EXIT_SUCCESS;
}
