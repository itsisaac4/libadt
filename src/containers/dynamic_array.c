#include "libadt/dynamic_array.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DYNAMIC_ARRAY_INITIAL_CAPACITY 8

#define ELEMENT_AT(array, index) \
    ((unsigned char *)(array)->_private.data + (index) * (array)->super._private.type.elementSize)

#define CONST_ELEMENT_AT(array, index) \
    ((const unsigned char *)(array)->_private.data + (index) * (array)->super._private.type.elementSize)

static void VisitElements(const ADT_Super_t *adt, ADT_ConstVisitFn_t visitor, void *context)
{
    if (adt == NULL || visitor == NULL)
    {
        return;
    }

    const DynamicArray_t *array = (const DynamicArray_t *)adt;

    for (size_t i = 0; i < adt->_private.size; i++)
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

    for (size_t i = 0; i < adt->_private.size; i++)
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
    if (array->super._private.type.destroy != NULL)
    {
        array->super._private.type.destroy(ELEMENT_AT(array, index));
    }
}

static bool PointsIntoStorage(const DynamicArray_t *array, const void *pointer)
{
    if (array->_private.data == NULL || pointer == NULL)
    {
        return false;
    }

    const uintptr_t storage = (uintptr_t)array->_private.data;
    const uintptr_t address = (uintptr_t)pointer;
    const size_t storageSize = array->_private.capacity * array->super._private.type.elementSize;

    return address >= storage && address - storage < storageSize;
}

static void CloseGap(DynamicArray_t *array, size_t index)
{
    size_t elementsToMove = array->super._private.size - index - 1;

    if (elementsToMove > 0)
    {
        memmove(
            ELEMENT_AT(array, index),
            ELEMENT_AT(array, index + 1),
            elementsToMove * array->super._private.type.elementSize);
    }

    array->super._private.size--;
}

static bool EnsureCapacity(DynamicArray_t *array, size_t requiredCapacity)
{
    if (array == NULL || array->super._private.type.elementSize == 0)
    {
        return false;
    }

    if (requiredCapacity <= array->_private.capacity)
    {
        return true;
    }

    size_t newCapacity = array->_private.capacity == 0 ? DYNAMIC_ARRAY_INITIAL_CAPACITY : array->_private.capacity;
    while (newCapacity < requiredCapacity)
    {
        if (newCapacity > SIZE_MAX / 2)
        {
            return false;
        }

        newCapacity *= 2;
    }

    if (newCapacity > SIZE_MAX / array->super._private.type.elementSize)
    {
        return false;
    }

    void *newData = realloc(array->_private.data, newCapacity * array->super._private.type.elementSize);

    if (newData == NULL)
    {
        return false;
    }

    array->_private.data = newData;
    array->_private.capacity = newCapacity;

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
        ._private = {
            .vtable = &DA_VTABLE,
            .size = 0,
            .type = typeInfo}};
    array->_private.data = data;
    array->_private.capacity = DYNAMIC_ARRAY_INITIAL_CAPACITY;

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
        ._private = {
            .vtable = &DA_VTABLE,
            .size = initialCount,
            .type = typeInfo}};
    array->_private.data = data;
    array->_private.capacity = capacity;

    return true;
}

bool da_Get(const DynamicArray_t *array, size_t index, void *outElement)
{
    if (array == NULL || outElement == NULL || index >= array->super._private.size)
    {
        return false;
    }

    memcpy(outElement, ELEMENT_AT(array, index), array->super._private.type.elementSize);
    return true;
}

bool da_detail_SetRef(DynamicArray_t *array, size_t index, const void *element)
{
    if (array == NULL || element == NULL || index >= array->super._private.size)
    {
        return false;
    }

    void *destination = ELEMENT_AT(array, index);
    if (destination == element)
    {
        return true;
    }

    DestroyElement(array, index);
    memmove(destination, element, array->super._private.type.elementSize);
    return true;
}

bool da_detail_IndexOfRef(const DynamicArray_t *array, const void *element, size_t *outIndex)
{
    if (array == NULL || element == NULL || outIndex == NULL)
    {
        return false;
    }

    for (size_t i = 0; i < array->super._private.size; i++)
    {
        const void *currentElement = ELEMENT_AT(array, i);
        const bool matches =
            array->super._private.type.compare != NULL
                ? array->super._private.type.compare(currentElement, element) == 0
                : memcmp(currentElement, element, array->super._private.type.elementSize) == 0;

        if (matches)
        {
            *outIndex = i;
            return true;
        }
    }

    return false;
}

bool da_detail_ContainsRef(const DynamicArray_t *array, const void *element)
{
    if (array == NULL || element == NULL)
    {
        return false;
    }

    size_t index = 0;
    return da_detail_IndexOfRef(array, element, &index);
}

bool da_detail_InsertRef(DynamicArray_t *array, size_t index, const void *element)
{
    if (array == NULL ||
        element == NULL ||
        index > array->super._private.size ||
        array->super._private.size == SIZE_MAX)
    {
        return false;
    }

    void *elementCopy = NULL;
    const void *source = element;

    if (PointsIntoStorage(array, element))
    {
        elementCopy = malloc(array->super._private.type.elementSize);
        if (elementCopy == NULL)
        {
            return false;
        }

        memcpy(elementCopy, element, array->super._private.type.elementSize);
        source = elementCopy;
    }

    if (!EnsureCapacity(array, array->super._private.size + 1))
    {
        free(elementCopy);
        return false;
    }

    unsigned char *dest = ELEMENT_AT(array, index + 1);
    size_t elementsToMove = array->super._private.size - index;

    if (elementsToMove > 0)
    {
        memmove(dest,
                ELEMENT_AT(array, index),
                elementsToMove * array->super._private.type.elementSize);
    }

    memcpy(ELEMENT_AT(array, index), source, array->super._private.type.elementSize);
    array->super._private.size++;

    free(elementCopy);
    return true;
}

bool da_detail_PrependRef(DynamicArray_t *array, const void *element)
{
    if (array == NULL || element == NULL)
    {
        return false;
    }

    return da_detail_InsertRef(array, 0, element);
}

bool da_detail_AppendRef(DynamicArray_t *array, const void *element)
{
    if (array == NULL || element == NULL)
    {
        return false;
    }

    return da_detail_InsertRef(array, array->super._private.size, element);
}

bool da_Remove(DynamicArray_t *array, size_t index)
{
    if (array == NULL || index >= array->super._private.size)
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
        index >= array->super._private.size ||
        PointsIntoStorage(array, outElement))
    {
        return false;
    }

    memmove(outElement, ELEMENT_AT(array, index), array->super._private.type.elementSize);
    CloseGap(array, index);
    return true;
}

void da_Clear(DynamicArray_t *array)
{
    if (array == NULL)
    {
        return;
    }

    for (size_t i = 0; i < array->super._private.size; i++)
    {
        DestroyElement(array, i);
    }

    array->super._private.size = 0;
}

void da_Destroy(DynamicArray_t *array)
{
    if (array == NULL)
    {
        return;
    }

    da_Clear(array);

    free(array->_private.data);

    array->super = (ADT_Super_t){0};
    array->_private.data = NULL;
    array->_private.capacity = 0;
}
