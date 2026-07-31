#include <stdio.h>

#include "libadt/libadt.h"

static void ShowStack(const char *operation, const Stack_t *stack)
{
    printf("%s\n", operation);
    adt_Print(stack);
    printf("\n");
}

int main(void)
{
    Stack_t stack = {0};
    ST_INIT(&stack, int);
    ShowStack("initialized", &stack);

    st_Push(&stack, 10);
    st_Push(&stack, 20);
    st_Push(&stack, 30);
    ShowStack("push 10, 20, and 30", &stack);

    int value = 0;
    st_Peek(&stack, &value);
    printf("top value: %d\n", value);

    st_Pop(&stack, &value);
    printf("popped value: %d\n", value);
    ShowStack("pop", &stack);

    st_Discard(&stack);
    ShowStack("discard", &stack);

    st_Clear(&stack);
    ShowStack("clear", &stack);

    st_Destroy(&stack);
    return 0;
}
