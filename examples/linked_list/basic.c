#include <stdio.h>

#include "libadt/libadt.h"

static void ShowList(const char *operation, const LinkedList_t *list)
{
    printf("%s\n", operation);
    adt_Print(list);
    printf("\n");
}

int main(void)
{
    LinkedList_t list = {0};
    LL_INIT(&list, int);
    ShowList("initialized", &list);

    ll_Append(&list, 20);
    ll_Append(&list, 30);
    ShowList("append 20 and 30", &list);

    ll_Prepend(&list, 10);
    ll_Insert(&list, 2, 25);
    ShowList("prepend 10 and insert 25 at index 2", &list);

    ll_Set(&list, 1, 15);
    ShowList("set index 1 to 15", &list);

    int value = 0;
    ll_Get(&list, 2, &value);
    printf("value at index 2: %d\n", value);

    size_t index = 0;
    ll_IndexOf(&list, 30, &index);
    printf("index of 30: %zu\n", index);
    printf("contains 15: %s\n", ll_Contains(&list, 15) ? "true" : "false");

    ll_Remove(&list, 1);
    ShowList("remove index 1", &list);

    ll_Take(&list, 0, &value);
    printf("taken value: %d\n", value);
    ShowList("take index 0", &list);

    ll_Clear(&list);
    ShowList("clear", &list);

    ll_Destroy(&list);
    return 0;
}
