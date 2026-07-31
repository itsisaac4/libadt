#include <stdio.h>

#include "libadt/libadt.h"

static void ShowQueue(const char *operation, const Queue_t *queue)
{
    printf("%s\n", operation);
    adt_Print(queue);
    printf("\n");
}

int main(void)
{
    Queue_t queue = {0};
    QU_INIT(&queue, int);
    ShowQueue("initialized", &queue);

    qu_Enqueue(&queue, 10);
    qu_Enqueue(&queue, 20);
    qu_Enqueue(&queue, 30);
    ShowQueue("enqueue 10, 20, and 30", &queue);

    int front = 0;
    int back = 0;
    qu_Front(&queue, &front);
    qu_Back(&queue, &back);
    printf("front: %d, back: %d\n", front, back);

    qu_Dequeue(&queue, &front);
    printf("dequeued value: %d\n", front);
    ShowQueue("dequeue", &queue);

    qu_Discard(&queue);
    ShowQueue("discard", &queue);

    qu_Clear(&queue);
    ShowQueue("clear", &queue);

    qu_Destroy(&queue);
    return 0;
}
