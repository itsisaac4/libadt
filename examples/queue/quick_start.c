#include <stdio.h>
#include <stdlib.h>

#include "libadt/libadt.h"

int main(void)
{
    Queue_t queue = {0};

    if (!QU_INIT(&queue, int) ||
        !qu_Enqueue(&queue, 10) ||
        !qu_Enqueue(&queue, 20) ||
        !qu_Enqueue(&queue, 30))
    {
        qu_Destroy(&queue);
        return EXIT_FAILURE;
    }

    int front = 0;
    int back = 0;
    if (!qu_Front(&queue, &front) ||
        !qu_Back(&queue, &back))
    {
        qu_Destroy(&queue);
        return EXIT_FAILURE;
    }

    printf("front: %d, back: %d\n", front, back);

    while (qu_Dequeue(&queue, &front))
    {
        printf("dequeued: %d\n", front);
    }

    qu_Destroy(&queue);
    return EXIT_SUCCESS;
}
