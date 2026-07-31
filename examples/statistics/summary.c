#include <stdio.h>
#include <stdlib.h>

#include "libadt/libadt.h"

static bool PrintSummary(const char *label, const ADT_t *values)
{
    double mean = 0.0;
    double median = 0.0;
    double mode = 0.0;

    if (!adt_Mean(values, &mean) ||
        !adt_Median(values, &median) ||
        !adt_Mode(values, &mode))
    {
        return false;
    }

    printf(
        "%s: mean=%.2f, median=%.2f, mode=%.2f\n",
        label,
        mean,
        median,
        mode);
    return true;
}

int main(void)
{
    int initialValues[] = {1, 2, 2, 4, 6};
    DynamicArray_t array = {0};
    LinkedList_t list = {0};

    if (!DA_INIT_FROM(&array, initialValues) ||
        !LL_INIT_FROM(&list, initialValues) ||
        !PrintSummary("dynamic array", &array) ||
        !PrintSummary("linked list", &list))
    {
        da_Destroy(&array);
        ll_Destroy(&list);
        return EXIT_FAILURE;
    }

    da_Destroy(&array);
    ll_Destroy(&list);
    return EXIT_SUCCESS;
}
