#include <stdio.h>
#include <stdlib.h>

#include "libadt/libadt.h"

typedef struct
{
    const char *name;
    ADT_SortAlgorithm_t algorithm;
} SortExample_t;

static bool RunSort(DynamicArray_t *array, const SortExample_t *example)
{
    if (!adt_Sort(array, example->algorithm))
    {
        return false;
    }

    printf("%s: ", example->name);
    return adt_Print(array);
}

int main(void)
{
    int values[] = {4, 1, 3, 2};
    DynamicArray_t numbers = {0};

    if (!DA_INIT_FROM(&numbers, values))
    {
        return EXIT_FAILURE;
    }

    printf("Original: ");
    if (!adt_Print(&numbers))
    {
        da_Destroy(&numbers);
        return EXIT_FAILURE;
    }
    da_Destroy(&numbers);

    SortExample_t examples[] = {
        {.name = "Bubble", .algorithm = ADT_SORT_BUBBLE},
        {.name = "Selection", .algorithm = ADT_SORT_SELECTION},
        {.name = "Insertion", .algorithm = ADT_SORT_INSERTION},
        {.name = "Quick", .algorithm = ADT_SORT_QUICK},
        {.name = "Bogo", .algorithm = ADT_SORT_BOGO}};

    for (size_t i = 0; i < ARRAY_COUNT(examples); i++)
    {
        if (!DA_INIT_FROM(&numbers, values))
        {
            return EXIT_FAILURE;
        }

        if (!RunSort(&numbers, &examples[i]))
        {
            da_Destroy(&numbers);
            return EXIT_FAILURE;
        }

        da_Destroy(&numbers);
    }

    return EXIT_SUCCESS;
}
