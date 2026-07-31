#include <stdio.h>
#include <stdlib.h>

#include "libadt/libadt.h"

int main(void)
{
    Stack_t stack = {0};

    if (!ST_INIT(&stack, int) ||
        !st_Push(&stack, 10) ||
        !st_Push(&stack, 20) ||
        !st_Push(&stack, 30))
    {
        st_Destroy(&stack);
        return EXIT_FAILURE;
    }

    int top = 0;
    if (!st_Peek(&stack, &top))
    {
        st_Destroy(&stack);
        return EXIT_FAILURE;
    }

    printf("top: %d\n", top);

    while (st_Pop(&stack, &top))
    {
        printf("popped: %d\n", top);
    }

    st_Destroy(&stack);
    return EXIT_SUCCESS;
}
