#include <stdio.h>
#include <stdlib.h>

#include "libadt/libadt.h"

int main(void)
{
    int values[] = {20, 30, 40};
    LinkedList_t numbers = {0};

    if (!LL_INIT_FROM(&numbers, values) ||
        !ll_Prepend(&numbers, 10) ||
        !ll_Append(&numbers, 50))
    {
        ll_Destroy(&numbers);
        return EXIT_FAILURE;
    }

    adt_Print(&numbers);

    int taken = 0;
    if (!ll_Take(&numbers, 2, &taken))
    {
        ll_Destroy(&numbers);
        return EXIT_FAILURE;
    }

    printf("taken: %d\n", taken);
    adt_Print(&numbers);

    ll_Destroy(&numbers);
    return EXIT_SUCCESS;
}
