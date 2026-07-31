#include <stdio.h>
#include <stdlib.h>

#include "libadt/libadt.h"

typedef struct
{
    const char *name;
    ADT_SortAlgorithm_t algorithm;
} SortExample_t;

static bool RunSort(DynamicArray_t array, const SortExample_t *example)
{

    if (!adt_Sort(&array, example->algorithm))
    {
        return false;
    }

    printf("%s: ", example->name);
    bool printed = adt_Print(&array);
    return printed;
}

int main(void)
{
    int values[] = {4, 1, 3, 2};
    DynamicArray_t numbers;

    if (!DA_INIT_FROM(&numbers, values))
    {
        da_Destroy(&numbers);
    }

    printf("Original: ");
    adt_Print(&numbers);

    SortExample_t examples[] = {
        {.name = "Bubble", .algorithm = ADT_SORT_BUBBLE},
        {.name = "Selection", .algorithm = ADT_SORT_SELECTION},
        {.name = "Insertion", .algorithm = ADT_SORT_INSERTION},
        {.name = "Quick", .algorithm = ADT_SORT_QUICK},
        {.name = "Bogo", .algorithm = ADT_SORT_BOGO}};

    for (size_t i = 0; i < ARRAY_COUNT(examples); i++)
    {
        if (!RunSort(numbers, &examples[i]))
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
