#include <stdio.h>
#include <stdlib.h>

#include "libadt/dynamic_array.h"

typedef struct
{
    const char *name;
    ADT_SortAlgorithm_t algorithm;
} SortExample_t;

static bool RunSort(const SortExample_t *example)
{
    int values[] = {4, 1, 3, 2};
    DynamicArray_t numbers = {0};

    if (!DA_INIT_FROM(&numbers, values) ||
        !adt_Sort(&numbers, example->algorithm))
    {
        da_Destroy(&numbers);
        return false;
    }

    printf("%s: ", example->name);
    bool printed = adt_Print(&numbers);
    da_Destroy(&numbers);
    return printed;
}

int main(void)
{
    SortExample_t examples[] = {
        {.name = "bubble", .algorithm = ADT_SORT_BUBBLE},
        {.name = "selection", .algorithm = ADT_SORT_SELECTION},
        {.name = "insertion", .algorithm = ADT_SORT_INSERTION},
        {.name = "quick", .algorithm = ADT_SORT_QUICK},
        {.name = "bogo", .algorithm = ADT_SORT_BOGO}};

    for (size_t i = 0; i < ARRAY_COUNT(examples); i++)
    {
        if (!RunSort(&examples[i]))
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
