#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define INITIAL_CAPACITY 8
    typedef struct

    {
        void *data;
        size_t size;
        size_t capacity;
        size_t elementSize;

        void (*Compare)(const void *a, const void *b);
    } DynamicArray_t;

#define ARRAY_COUNT(array) \
    (sizeof(array) / sizeof((array)[0]))

#define DA_INIT_FROM(array, values) \
    da_InitFrom(                    \
        (array),                    \
        (values),                   \
        ARRAY_COUNT(values),        \
        sizeof((values)[0]))

    size_t da_Size(DynamicArray_t *array);

    bool da_IsEmpty(DynamicArray_t *array);

    size_t da_IndexOf(DynamicArray_t *array, const void *element);

    bool da_Contain(DynamicArray_t *array, const void *element);

    void da_Set(DynamicArray_t *array, size_t index, const void *element);

    void da_Get(DynamicArray_t *array, size_t index, void *out_element);

    void da_Clear(DynamicArray_t *array);

    void da_Remove(DynamicArray_t *array, size_t index);

    bool da_Insert(DynamicArray_t *array, size_t index, const void *element);

    bool da_Prepend(DynamicArray_t *array, const void *element);

    bool da_Append(DynamicArray_t *array, const void *element);

    void da_Destroy(DynamicArray_t *array);

    bool da_InitFrom(DynamicArray_t *array, const void *elements, size_t initialCount, size_t element_size);

    bool da_Init(DynamicArray_t *array, size_t element_size);

#ifdef __cplusplus
}
#endif

#endif