#include "libadt/dynamic_array.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define INITIAL_CAPACITY 8

#define ELEMENT_AT(array, index) \
    ((unsigned char *)(array)->data + (index) * (array)->typeInfo.elementSize)

static bool EnsureCapacity(DynamicArray_t *array, size_t requiredCapacity)
{
    if (array == NULL || array->typeInfo.elementSize == 0)
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

    if (newCapacity > SIZE_MAX / array->typeInfo.elementSize)
    {
        return false;
    }

    void *newData = realloc(array->data, newCapacity * array->typeInfo.elementSize);

    if (newData == NULL)
    {
        return false;
    }

    array->data = newData;
    array->capacity = newCapacity;

    return true;
}

void da_Print(const DynamicArray_t *array)
{
    if (array == NULL || array->data == NULL || array->typeInfo.print == NULL)
    {
        printf("DynamicArray is NULL, uninitialized, or print function is NULL.\n");
        return;
    }

    printf("DynamicArray (size: %zu, capacity: %zu): [", array->size, array->capacity);
    for (size_t i = 0; i < array->size; i++)
    {
        if (i > 0)
        {
            printf(", ");
        }
        array->typeInfo.print(ELEMENT_AT(array, i));
    }
    printf("]\n");
}

void da_PrintDebug(const DynamicArray_t *array, const char *expression, const char *file, int line)
{
    printf("DynamicArray Debug\n");
    printf("  expression: %s\n", expression);
    printf("  location: %s:%d\n", file, line);
    printf("  address: %p\n", (const void *)array);

    if (array == NULL)
    {
        printf("  state: NULL\n");
        return;
    }

    printf("  data: %p\n", array->data);
    printf("  size: %zu\n", array->size);
    printf("  capacity: %zu\n", array->capacity);
    printf("  element size: %zu\n", array->typeInfo.elementSize);

    printf(
        "  comparator: %s\n",
        array->typeInfo.compare != NULL ? "set" : "NULL");

    printf(
        "  printer: %s\n",
        array->typeInfo.print != NULL ? "set" : "NULL");

    printf(
        "  destructor: %s\n",
        array->typeInfo.destroy != NULL ? "set" : "NULL");

    printf("  elements: ");

    if (array->data == NULL)
    {
        printf("<no storage>\n");
        return;
    }

    if (array->typeInfo.print == NULL)
    {
        printf("<no print function>\n");
        return;
    }

    printf("[");

    for (size_t i = 0; i < array->size; i++)
    {
        if (i > 0)
        {
            printf(", ");
        }

        array->typeInfo.print(ELEMENT_AT(array, i));
    }

    printf("]\n");
}

size_t da_Size(DynamicArray_t *array)
{

    if (array == NULL)
    {
        return 0;
    }

    return array->size;
}

bool da_IsEmpty(DynamicArray_t *array)
{
    if (array == NULL)
    {
        return true;
    }

    return array->size == 0;
}

bool da_IndexOf(DynamicArray_t *array, const void *element, size_t *outIndex)
{
    if (array == NULL || element == NULL || outIndex == NULL)
    {
        return false;
    }

    for (size_t i = 0; i < array->size; i++)
    {
        void *currentElement = ELEMENT_AT(array, i);

        if (array->typeInfo.compare != NULL)
        {
            if (array->typeInfo.compare(currentElement, element) == 0)
            {
                *outIndex = i;
                return true;
            }
        }
        else
        {
            if (memcmp(currentElement, element, array->typeInfo.elementSize) == 0)
            {
                *outIndex = i;
                return true;
            }
        }
    }
    return false;
}

bool da_Contains(DynamicArray_t *array, const void *element)
{
    if (array == NULL || element == NULL)
    {
        return false;
    }

    size_t index;
    return da_IndexOf(array, element, &index);
}

bool da_Set(DynamicArray_t *array, size_t index, const void *element)
{
    if (array == NULL || element == NULL || index >= array->size)
    {
        return false;
    }

    memcpy(ELEMENT_AT(array, index), element, array->typeInfo.elementSize);
    return true;
}

bool da_Get(DynamicArray_t *array, size_t index, void *outElement)
{
    if (array == NULL || outElement == NULL || index >= array->size)
    {
        return false;
    }

    memcpy(outElement, ELEMENT_AT(array, index), array->typeInfo.elementSize);
    return true;
}

void da_Clear(DynamicArray_t *array)
{
    if (array == NULL)
    {
        return;
    }

    array->size = 0;
}

bool da_Remove(DynamicArray_t *array, size_t index)
{
    if (array == NULL || index >= array->size)
    {
        return false;
    }

    size_t elementsToMove = array->size - index - 1;

    if (elementsToMove > 0)
    {
        memmove(ELEMENT_AT(array, index),
                ELEMENT_AT(array, index + 1),
                elementsToMove * array->typeInfo.elementSize);
    }

    array->size--;
    return true;
}

bool da_Insert(DynamicArray_t *array, size_t index, const void *element)
{
    if (array == NULL || element == NULL || index > array->size || !EnsureCapacity(array, array->size + 1))
    {
        return false;
    }

    unsigned char *dest = ELEMENT_AT(array, index + 1);
    size_t elementsToMove = array->size - index;

    if (elementsToMove > 0)
    {
        memmove(dest,
                ELEMENT_AT(array, index),
                elementsToMove * array->typeInfo.elementSize);
    }

    memcpy(ELEMENT_AT(array, index), element, array->typeInfo.elementSize);
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

    if (array->typeInfo.destroy != NULL)
    {
        for (size_t i = 0; i < array->size; i++)
        {
            void *element =
                (unsigned char *)array->data +
                i * array->typeInfo.elementSize;

            array->typeInfo.destroy(element);
        }
    }

    free(array->data);

    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
    array->typeInfo = (ADT_TypeInfo_t){0};
}

bool da_InitFrom(DynamicArray_t *array, const void *elements, size_t initialCount, const ADT_TypeInfo_t typeInfo)
{
    if (array == NULL || typeInfo.elementSize == 0)
    {
        return false;
    }

    if (elements == NULL)
    {
        return initialCount > 0 ? false : da_Init(array, typeInfo);
    }

    size_t capacity = INITIAL_CAPACITY;
    while (capacity < initialCount)
    {
        capacity *= 2;
    }

    array->data = malloc(capacity * typeInfo.elementSize);

    if (array->data == NULL)
    {
        return false;
    }

    if (initialCount > 0)
    {
        memcpy(array->data, elements, initialCount * typeInfo.elementSize);
    }

    array->size = initialCount;
    array->capacity = capacity;
    array->typeInfo = typeInfo;

    return true;
}

bool da_Init(DynamicArray_t *array, const ADT_TypeInfo_t typeInfo)
{
    if (array == NULL || typeInfo.elementSize == 0)
    {
        return false;
    }

    array->data = malloc(INITIAL_CAPACITY * typeInfo.elementSize);

    if (array->data == NULL)
    {
        return false;
    }

    array->size = 0;
    array->capacity = INITIAL_CAPACITY;
    array->typeInfo = typeInfo;

    return true;
}