#include "libadt/dynamic_array.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static bool EnsureCapacity(DynamicArray_t *array, size_t requiredCapacity)
{
    if (array == NULL || array->elementSize == 0)
    {
        return false;
    }

    if (requiredCapacity <= array->capacity)
    {
        return true;
    }

    size_t newCapacity = array->capacity == 0 ? INITIAL_CAPACITY : array->capacity;
    while (newCapacity < requiredCapacity)
    {
        if (newCapacity > SIZE_MAX / 2)
        {
            return false;
        }

        newCapacity *= 2;
    }

    if (newCapacity > SIZE_MAX / array->elementSize)
    {
        return false;
    }

    void *newData = realloc(array->data, newCapacity * array->elementSize);

    if (newData == NULL)
    {
        return false;
    }

    array->data = newData;
    array->capacity = newCapacity;

    return true;
}

size_t da_Size(DynamicArray_t *array);

bool da_IsEmpty(DynamicArray_t *array);

size_t da_IndexOf(DynamicArray_t *array, const void *element);

bool da_Contain(DynamicArray_t *array, const void *element);

void da_Set(DynamicArray_t *array, size_t index, const void *element);

void da_Get(DynamicArray_t *array, size_t index, void *out_element);

void da_Clear(DynamicArray_t *array);

void da_Remove(DynamicArray_t *array, size_t index);

bool da_Insert(DynamicArray_t *array, size_t index, const void *element)
{
    if (array == NULL || element == NULL || index > array->size || !EnsureCapacity(array, array->size + 1))
    {
        return false;
    }

    unsigned char *data = (unsigned char *)array->data;
    unsigned char *dest = data + (index + 1) * array->elementSize;
    size_t elementsToMove = array->size - index;

    if (elementsToMove > 0)
    {
        memmove(dest, data + index * array->elementSize, elementsToMove * array->elementSize);
    }

    memcpy(data + index * array->elementSize, element, array->elementSize);
    array->size++;

    return true;
}

bool da_Prepend(DynamicArray_t *array, const void *element)
{
    if (array == NULL || element == NULL)
    {
        return false;
    }

    return da_Insert(array, 0, element);
}

bool da_Append(DynamicArray_t *array, const void *element)
{
    if (array == NULL || element == NULL)
    {
        return false;
    }

    return da_Insert(array, array->size, element);
}

void da_Destroy(DynamicArray_t *array)
{
    if (array == NULL)
    {
        return;
    }

    free(array->data);

    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
    array->elementSize = 0;
}

bool da_InitFrom(DynamicArray_t *array, const void *elements, size_t initialCount, size_t element_size)
{
    if (array == NULL || element_size == 0)
    {
        return false;
    }

    if (elements == NULL)
    {
        return initialCount > 0 ? false : da_Init(array, element_size);
    }

    size_t capacity = INITIAL_CAPACITY;
    while (capacity < initialCount)
    {
        capacity *= 2;
    }

    array->data = malloc(capacity * element_size);

    if (array->data == NULL)
    {
        return false;
    }

    if (initialCount > 0)
    {
        memcpy(array->data, elements, initialCount * element_size);
    }

    array->size = initialCount;
    array->capacity = capacity;
    array->elementSize = element_size;

    return true;
}

bool da_Init(DynamicArray_t *array, size_t elementSize)
{
    if (array == NULL || elementSize == 0)
    {
        return false;
    }

    array->data = malloc(INITIAL_CAPACITY * elementSize);

    if (array->data == NULL)
    {
        return false;
    }

    array->size = 0;
    array->capacity = INITIAL_CAPACITY;
    array->elementSize = elementSize;

    return true;
}