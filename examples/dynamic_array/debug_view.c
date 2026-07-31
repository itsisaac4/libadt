#include <stdlib.h>

#include "libadt/libadt.h"

int main(void)
{
    int values[] = {2, 4, 8, 16};
    DynamicArray_t numbers = {0};

    if (!DA_INIT_FROM(&numbers, values))
    {
        return EXIT_FAILURE;
    }

    ADT_DEBUG(numbers);

    da_Destroy(&numbers);
    return EXIT_SUCCESS;
}
