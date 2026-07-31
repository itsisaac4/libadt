#include <stdio.h>
#include <stdlib.h>

#include "libadt/dynamic_array.h"
#include "libadt/linked_list.h"

static bool PrintExtrema(const ADT_t *adt)
{
    const ADT_ElementTypeInfo_t *type = adt_ElementType(adt);

    if (type == NULL ||
        type->elementSize == 0 ||
        type->compare == NULL ||
        type->print == NULL)
    {
        return false;
    }

    void *minimum = malloc(type->elementSize);
    void *maximum = malloc(type->elementSize);

    if (minimum == NULL || maximum == NULL)
    {
        free(minimum);
        free(maximum);
        return false;
    }

    bool found = adt_Min(adt, minimum) && adt_Max(adt, maximum);

    if (found)
    {
        printf("minimum: ");
        type->print(minimum);
        printf(", maximum: ");
        type->print(maximum);
        printf("\n");
    }

    free(minimum);
    free(maximum);
    return found;
}

static bool ProcessContainer(ADT_t *adt)
{
    if (!adt_Print(adt) ||
        !PrintExtrema(adt) ||
        !adt_Sort(adt, ADT_SORT_QUICK))
    {
        return false;
    }

    return adt_Print(adt);
}

int main(void)
{
    int arrayValues[] = {8, 2, 6, 4};
    int listValues[] = {7, 1, 5, 3};
    DynamicArray_t array = {0};
    LinkedList_t list = {0};

    if (!DA_INIT_FROM(&array, arrayValues) ||
        !LL_INIT_FROM(&list, listValues))
    {
        da_Destroy(&array);
        ll_Destroy(&list);
        return EXIT_FAILURE;
    }

    ADT_t *containers[] = {&array, &list};

    for (size_t i = 0; i < ARRAY_COUNT(containers); i++)
    {
        if (!ProcessContainer(containers[i]))
        {
            da_Destroy(&array);
            ll_Destroy(&list);
            return EXIT_FAILURE;
        }
    }

    da_Destroy(&array);
    ll_Destroy(&list);
    return EXIT_SUCCESS;
}
