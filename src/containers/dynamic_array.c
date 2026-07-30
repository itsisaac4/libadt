#include "libadt/dynamic_array.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DYNAMIC_ARRAY_INITIAL_CAPACITY 8

#define ELEMENT_AT(array, index) \
    ((unsigned char *)(array)->data + (index) * (array)->super.type.elementSize)

#define CONST_ELEMENT_AT(array, index) \
    ((const unsigned char *)(array)->data + (index) * (array)->super.type.elementSize)

static void VisitElements(const ADT_Super_t *adt, ADT_ConstVisitFn_t visitor, void *context)
{
    if (adt == NULL || visitor == NULL)
    {
        return;
    }

    const DynamicArray_t *array = (const DynamicArray_t *)adt;

    for (size_t i = 0; i < adt->size; i++)
    {
        visitor(CONST_ELEMENT_AT(array, i), i, context);
    }
}

static void VisitMutableElements(ADT_Super_t *adt, ADT_MutableVisitFn_t visitor, void *context)
{
    if (adt == NULL || visitor == NULL)
    {
        return;
    }

    DynamicArray_t *array = (DynamicArray_t *)adt;

    for (size_t i = 0; i < adt->size; i++)
    {
        visitor(ELEMENT_AT(array, i), i, context);
    }
}

static const ADT_VTable_t DA_VTABLE = {
    .containerName = "DynamicArray",
    .visit = VisitElements,
    .visitMutable = VisitMutableElements};

static void DestroyElement(DynamicArray_t *array, size_t index)
{
    if (array->super.type.destroy != NULL)
    {
        array->super.type.destroy(ELEMENT_AT(array, index));
    }
}

static bool PointsIntoStorage(const DynamicArray_t *array, const void *pointer)
{
    if (array->data == NULL || pointer == NULL)
    {
        return false;
    }

    const uintptr_t storage = (uintptr_t)array->data;
    const uintptr_t address = (uintptr_t)pointer;
    const size_t storageSize = array->capacity * array->super.type.elementSize;

    return address >= storage && address - storage < storageSize;
}

static void CloseGap(DynamicArray_t *array, size_t index)
{
    size_t elementsToMove = array->super.size - index - 1;

    if (elementsToMove > 0)
    {
        memmove(
            ELEMENT_AT(array, index),
            ELEMENT_AT(array, index + 1),
            elementsToMove * array->super.type.elementSize);
    }

    array->super.size--;
}

static bool EnsureCapacity(DynamicArray_t *array, size_t requiredCapacity)
{
    if (array == NULL || array->super.type.elementSize == 0)
    {
        return false;
    }

    if (requiredCapacity <= array->capacity)
    {
        return true;
    }

    size_t newCapacity = array->capacity == 0 ? DYNAMIC_ARRAY_INITIAL_CAPACITY : array->capacity;
    while (newCapacity < requiredCapacity)
    {
        if (newCapacity > SIZE_MAX / 2)
        {
            return false;
        }

        newCapacity *= 2;
    }

    if (newCapacity > SIZE_MAX / array->super.type.elementSize)
    {
        return false;
    }

    void *newData = realloc(array->data, newCapacity * array->super.type.elementSize);

    if (newData == NULL)
    {
        return false;
    }

    array->data = newData;
    array->capacity = newCapacity;

    return true;
}

bool da_Init(DynamicArray_t *array, ADT_TypeInfo_t typeInfo)
{
    if (array == NULL || typeInfo.elementSize == 0)
    {
        return false;
    }

    if (DYNAMIC_ARRAY_INITIAL_CAPACITY > SIZE_MAX / typeInfo.elementSize)
    {
        return false;
    }

    void *data = malloc(DYNAMIC_ARRAY_INITIAL_CAPACITY * typeInfo.elementSize);

    if (data == NULL)
    {
        return false;
    }

    array->super = (ADT_Super_t){
        .vtable = &DA_VTABLE,
        .size = 0,
        .type = typeInfo};
    array->data = data;
    array->capacity = DYNAMIC_ARRAY_INITIAL_CAPACITY;

    return true;
}

bool da_InitFrom(DynamicArray_t *array, const void *elements, size_t initialCount, ADT_TypeInfo_t typeInfo)
{
    if (array == NULL || typeInfo.elementSize == 0)
    {
        return false;
    }

    if (elements == NULL)
    {
        return initialCount > 0 ? false : da_Init(array, typeInfo);
    }

    size_t capacity = DYNAMIC_ARRAY_INITIAL_CAPACITY;
    while (capacity < initialCount)
    {
        if (capacity > SIZE_MAX / 2)
        {
            return false;
        }

        capacity *= 2;
    }

    if (capacity > SIZE_MAX / typeInfo.elementSize)
    {
        return false;
    }

    void *data = malloc(capacity * typeInfo.elementSize);

    if (data == NULL)
    {
        return false;
    }

    if (initialCount > 0)
    {
        memcpy(data, elements, initialCount * typeInfo.elementSize);
    }

    array->super = (ADT_Super_t){
        .vtable = &DA_VTABLE,
        .size = initialCount,
        .type = typeInfo};
    array->data = data;
    array->capacity = capacity;

    return true;
}

bool da_Get(const DynamicArray_t *array, size_t index, void *outElement)
{
    if (array == NULL || outElement == NULL || index >= array->super.size)
    {
        return false;
    }

    memcpy(outElement, ELEMENT_AT(array, index), array->super.type.elementSize);
    return true;
}

bool da_SetRef(DynamicArray_t *array, size_t index, const void *element)
{
    if (array == NULL || element == NULL || index >= array->super.size)
    {
        return false;
    }

    void *destination = ELEMENT_AT(array, index);
    if (destination == element)
    {
        return true;
    }

    DestroyElement(array, index);
    memmove(destination, element, array->super.type.elementSize);
    return true;
}

bool da_IndexOfRef(const DynamicArray_t *array, const void *element, size_t *outIndex)
{
    if (array == NULL || element == NULL || outIndex == NULL)
    {
        return false;
    }

    for (size_t i = 0; i < array->super.size; i++)
    {
        const void *currentElement = ELEMENT_AT(array, i);
        const bool matches =
            array->super.type.compare != NULL
                ? array->super.type.compare(currentElement, element) == 0
                : memcmp(currentElement, element, array->super.type.elementSize) == 0;

        if (matches)
        {
            *outIndex = i;
            return true;
        }
    }

    return false;
}

bool da_ContainsRef(const DynamicArray_t *array, const void *element)
{
    if (array == NULL || element == NULL)
    {
        return false;
    }

    size_t index = 0;
    return da_IndexOfRef(array, element, &index);
}

bool da_InsertRef(DynamicArray_t *array, size_t index, const void *element)
{
    if (array == NULL ||
        element == NULL ||
        index > array->super.size ||
        array->super.size == SIZE_MAX)
    {
        return false;
    }

    void *elementCopy = NULL;
    const void *source = element;

    if (PointsIntoStorage(array, element))
    {
        elementCopy = malloc(array->super.type.elementSize);
        if (elementCopy == NULL)
        {
            return false;
        }

        memcpy(elementCopy, element, array->super.type.elementSize);
        source = elementCopy;
    }

    if (!EnsureCapacity(array, array->super.size + 1))
    {
        free(elementCopy);
        return false;
    }

    unsigned char *dest = ELEMENT_AT(array, index + 1);
    size_t elementsToMove = array->super.size - index;

    if (elementsToMove > 0)
    {
        memmove(dest,
                ELEMENT_AT(array, index),
                elementsToMove * array->super.type.elementSize);
    }

    memcpy(ELEMENT_AT(array, index), source, array->super.type.elementSize);
    array->super.size++;

    free(elementCopy);
    return true;
}

bool da_PrependRef(DynamicArray_t *array, const void *element)
{
    if (array == NULL || element == NULL)
    {
        return false;
    }

    return da_InsertRef(array, 0, element);
}

bool da_AppendRef(DynamicArray_t *array, const void *element)
{
    if (array == NULL || element == NULL)
    {
        return false;
    }

    return da_InsertRef(array, array->super.size, element);
}

bool da_Remove(DynamicArray_t *array, size_t index)
{
    if (array == NULL || index >= array->super.size)
    {
        return false;
    }

    DestroyElement(array, index);
    CloseGap(array, index);
    return true;
}

bool da_Take(DynamicArray_t *array, size_t index, void *outElement)
{
    if (array == NULL ||
        outElement == NULL ||
        index >= array->super.size ||
        PointsIntoStorage(array, outElement))
    {
        return false;
    }

    memmove(outElement, ELEMENT_AT(array, index), array->super.type.elementSize);
    CloseGap(array, index);
    return true;
}

void da_Clear(DynamicArray_t *array)
{
    if (array == NULL)
    {
        return;
    }

    for (size_t i = 0; i < array->super.size; i++)
    {
        DestroyElement(array, i);
    }

    array->super.size = 0;
}

void da_Destroy(DynamicArray_t *array)
{
    if (array == NULL)
    {
        return;
    }

    da_Clear(array);

    free(array->data);

    array->super = (ADT_Super_t){0};
    array->data = NULL;
    array->capacity = 0;
}
