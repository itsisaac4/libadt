#include <stdio.h>

#include "libadt/libadt.h"

static void ShowArray(const char *operation, const DynamicArray_t *array)
{
    printf("%s\n", operation);
    adt_Print(array);
    printf("\n");
}

int main(void)
{
    DynamicArray_t array = {0};
    DA_INIT(&array, int);
    ShowArray("initialized", &array);

    da_Append(&array, 20);
    da_Append(&array, 30);
    ShowArray("append 20 and 30", &array);

    da_Prepend(&array, 10);
    da_Insert(&array, 2, 25);
    ShowArray("prepend 10 and insert 25 at index 2", &array);

    da_Set(&array, 1, 15);
    ShowArray("set index 1 to 15", &array);

    int value = 0;
    da_Get(&array, 2, &value);
    printf("value at index 2: %d\n", value);

    size_t index = 0;
    da_IndexOf(&array, 30, &index);
    printf("index of 30: %zu\n", index);
    printf("contains 15: %s\n", da_Contains(&array, 15) ? "true" : "false");

    da_Remove(&array, 1);
    ShowArray("remove index 1", &array);

    da_Take(&array, 0, &value);
    printf("taken value: %d\n", value);
    ShowArray("take index 0", &array);

    da_Clear(&array);
    ShowArray("clear", &array);

    da_Destroy(&array);
    return 0;
}
