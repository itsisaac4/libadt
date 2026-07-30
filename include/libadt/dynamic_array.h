#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdbool.h>
#include <stddef.h>
#include "type_info.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define INITIAL_CAPACITY 8

#define ARRAY_COUNT(array) \
    (sizeof(array) / sizeof((array)[0]))

#ifndef __cplusplus
#define DA_INIT(array, type)             \
    da_Init(                             \
        (array),                         \
        (ADT_TypeInfo_t){                \
            .elementSize = sizeof(type), \
            .compare = COMPARATOR(type), \
            .print = PRINTER(type),      \
            .destroy = NULL})

#define DA_INIT_FROM(array, values)                            \
    da_InitFrom(                                               \
        (array),                                               \
        (values),                                              \
        ARRAY_COUNT(values),                                   \
        (ADT_TypeInfo_t){                                      \
            .elementSize = sizeof((values)[0]),                \
            .compare = COMPARATOR(typeof_unqual((values)[0])), \
            .print = PRINTER(typeof_unqual((values)[0])),      \
            .destroy = NULL})
#endif

/* Accepts an array variable, not a pointer, so #array is a clean name. */
#define DA_DEBUG(array) \
    da_PrintDebug(&(array), #array, __FILE__, __LINE__)

    typedef struct
    {
        void *data;
        size_t size;
        size_t capacity;
        ADT_TypeInfo_t typeInfo;
    } DynamicArray_t;

    /** Prints the array using its configured print function. */
    void da_Print(const DynamicArray_t *array);

    /** Prints the array's internal state and source location for debugging. */
    void da_PrintDebug(
        const DynamicArray_t *array,
        const char *expression,
        const char *file,
        int line);

    /** Returns the number of elements in the array, or zero for NULL. */
    size_t da_Size(DynamicArray_t *array);

    /** Returns whether the array is NULL or contains no elements. */
    bool da_IsEmpty(DynamicArray_t *array);

    /** Finds the first matching element and writes its index to outIndex. */
    bool da_IndexOf(DynamicArray_t *array, const void *element, size_t *outIndex);

    /** Returns whether the array contains a matching element. */
    bool da_Contains(DynamicArray_t *array, const void *element);

    /** Replaces the element at index by copying the supplied value. */
    bool da_Set(DynamicArray_t *array, size_t index, const void *element);

    /** Copies the element at index into outElement. */
    bool da_Get(DynamicArray_t *array, size_t index, void *outElement);

    /** Removes all elements without releasing the allocated storage. */
    void da_Clear(DynamicArray_t *array);

    /** Removes the element at index without invoking the element destructor. */
    bool da_Remove(DynamicArray_t *array, size_t index);

    /** Inserts a copied element at index. */
    bool da_Insert(DynamicArray_t *array, size_t index, const void *element);

    /** Inserts a copied element at the beginning of the array. */
    bool da_Prepend(DynamicArray_t *array, const void *element);

    /** Adds a copied element to the end of the array. */
    bool da_Append(DynamicArray_t *array, const void *element);

    /** Destroys remaining elements and releases the array's storage. */
    void da_Destroy(DynamicArray_t *array);

    /** Initializes an empty array with the supplied type information. */
    bool da_Init(DynamicArray_t *array, ADT_TypeInfo_t typeInfo);

    /** Initializes an array by copying initialCount elements. */
    bool da_InitFrom(
        DynamicArray_t *array,
        const void *elements,
        size_t initialCount,
        ADT_TypeInfo_t typeInfo);

#ifdef __cplusplus
}
#endif

#endif
