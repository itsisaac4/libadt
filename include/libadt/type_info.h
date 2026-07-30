#ifndef LIBADT_TYPE_INFO_H
#define LIBADT_TYPE_INFO_H

#include <stddef.h>

#include "comparators.h"
#include "printers.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** Releases resources owned by one element. */
    typedef void (*DestroyFn)(
        void *element);

    /** Describes the size and optional operations for an element type. */
    typedef struct
    {
        size_t elementSize;
        CompareFn compare;
        PrintFn print;
        DestroyFn destroy;
    } ADT_TypeInfo_t;

#ifdef __cplusplus
}
#endif

#endif
