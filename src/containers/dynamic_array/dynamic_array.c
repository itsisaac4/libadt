#include "libadt/dynamic_array.h"

#include <string.h>

static void *ElementAt(DynamicArray_t *array, size_t index)
{
    return contiguousStorage_At(
        &array->_private.storage,
        index,
        array->super._private.elementType.elementSize);
}

static const void *ElementAtConst(const DynamicArray_t *array, size_t index)
{
    return contiguousStorage_AtConst(
        &array->_private.storage,
        index,
        array->super._private.elementType.elementSize);
}

static void VisitElements(const ADT_Super_t *adt, ADT_ConstVisitFn_t visitor, void *context)
{
    if (adt == NULL || visitor == NULL)
    {
        return;
    }

    const DynamicArray_t *array = (const DynamicArray_t *)adt;

    for (size_t i = 0; i < adt->_private.size; i++)
    {
        visitor(ElementAtConst(array, i), i, context);
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
        visitor(ElementAt(array, i), i, context);
    }
}

static const ADT_VTable_t DA_VTABLE = {
    .containerName = "DynamicArray",
    .visit = VisitElements,
    .visitMutable = VisitMutableElements};

static void DestroyElement(DynamicArray_t *array, size_t index)
{
    if (array->super._private.elementType.destroy != NULL)
    {
        array->super._private.elementType.destroy(ElementAt(array, index));
    }
}

bool da_Init(DynamicArray_t *array, ADT_ElementTypeInfo_t elementType)
{
    if (array == NULL || elementType.elementSize == 0)
    {
        return false;
    }

    ContiguousStorage_t storage;
    if (!contiguousStorage_Init(&storage, elementType.elementSize))
    {
        return false;
    }

    array->super = (ADT_Super_t){
        ._private = {
            .vtable = &DA_VTABLE,
            .size = 0,
            .elementType = elementType}};
    array->_private.storage = storage;

    return true;
}

bool da_InitFrom(DynamicArray_t *array, const void *elements, size_t initialCount, ADT_ElementTypeInfo_t elementType)
{
    if (array == NULL ||
        elementType.elementSize == 0 ||
        (elements == NULL && initialCount > 0))
    {
        return false;
    }

    ContiguousStorage_t storage;
    if (!contiguousStorage_InitFrom(
            &storage,
            elements,
            initialCount,
            elementType.elementSize))
    {
        return false;
    }

    array->super = (ADT_Super_t){
        ._private = {
            .vtable = &DA_VTABLE,
            .size = initialCount,
            .elementType = elementType}};
    array->_private.storage = storage;

    return true;
}

bool da_Get(const DynamicArray_t *array, size_t index, void *outElement)
{
    if (array == NULL || outElement == NULL || index >= array->super._private.size)
    {
        return false;
    }

    memcpy(outElement, ElementAtConst(array, index), array->super._private.elementType.elementSize);
    return true;
}

bool da_detail_SetRef(DynamicArray_t *array, size_t index, const void *element)
{
    if (array == NULL || element == NULL || index >= array->super._private.size)
    {
        return false;
    }

    void *destination = ElementAt(array, index);
    if (destination == element)
    {
        return true;
    }

    DestroyElement(array, index);
    memmove(destination, element, array->super._private.elementType.elementSize);
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
        const void *currentElement = ElementAtConst(array, i);
        const bool matches = array->super._private.elementType.compare != NULL
                                 ? array->super._private.elementType.compare(currentElement, element) == 0
                                 : memcmp(currentElement, element, array->super._private.elementType.elementSize) == 0;

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
    if (array == NULL || element == NULL)
    {
        return false;
    }

    if (!contiguousStorage_Insert(
            &array->_private.storage,
            array->super._private.size,
            index,
            element,
            array->super._private.elementType.elementSize))
    {
        return false;
    }

    array->super._private.size++;
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
    if (!contiguousStorage_Erase(
            &array->_private.storage,
            array->super._private.size,
            index,
            array->super._private.elementType.elementSize))
    {
        return false;
    }

    array->super._private.size--;
    return true;
}

bool da_Take(DynamicArray_t *array, size_t index, void *outElement)
{
    if (array == NULL ||
        outElement == NULL ||
        index >= array->super._private.size ||
        contiguousStorage_ContainsAddress(
            &array->_private.storage,
            outElement,
            array->super._private.elementType.elementSize))
    {
        return false;
    }

    memmove(outElement, ElementAt(array, index), array->super._private.elementType.elementSize);

    if (!contiguousStorage_Erase(
            &array->_private.storage,
            array->super._private.size,
            index,
            array->super._private.elementType.elementSize))
    {
        return false;
    }

    array->super._private.size--;
    return true;
}

bool da_BinarySearch(const DynamicArray_t *array, const void *target, size_t *outIndex)
{
    if (array == NULL ||
        target == NULL ||
        outIndex == NULL ||
        array->super._private.elementType.compare == NULL)
    {
        return false;
    }

    return contiguousStorage_BinarySearchBy(
        &array->_private.storage,
        array->super._private.size,
        array->super._private.elementType.elementSize,
        array->super._private.elementType.compare,
        target,
        outIndex);
}

bool da_BinarySearchBy(const DynamicArray_t *array, CompareFn_t compare, const void *target, size_t *outIndex)
{
    if (array == NULL ||
        compare == NULL ||
        target == NULL ||
        outIndex == NULL)
    {
        return false;
    }

    return contiguousStorage_BinarySearchBy(
        &array->_private.storage,
        array->super._private.size,
        array->super._private.elementType.elementSize,
        compare,
        target,
        outIndex);
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
    contiguousStorage_Destroy(&array->_private.storage);
    array->super = (ADT_Super_t){0};
}
