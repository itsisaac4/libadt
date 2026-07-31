#include "libadt/linked_list.h"

#include <stdint.h>
#include <string.h>

static void VisitElements(const ADT_Super_t *adt, ADT_ConstVisitFn_t visitor, void *context)
{
    if (adt == NULL || visitor == NULL)
    {
        return;
    }

    const LinkedList_t *list = (const LinkedList_t *)adt;
    size_t index = 0;
    for (const LinkedStorageNode_t *node = list->_private.storage.head; node != NULL; node = node->next)
    {
        visitor(node->data, index, context);
        index++;
    }
}

static void VisitMutableElements(ADT_Super_t *adt, ADT_MutableVisitFn_t visitor, void *context)
{
    if (adt == NULL || visitor == NULL)
    {
        return;
    }

    LinkedList_t *list = (LinkedList_t *)adt;
    size_t index = 0;
    for (LinkedStorageNode_t *node = list->_private.storage.head; node != NULL; node = node->next)
    {
        visitor(node->data, index, context);
        index++;
    }
}

static const ADT_VTable_t LINKED_LIST_VTABLE = {
    .containerName = "LinkedList",
    .visit = VisitElements,
    .visitMutable = VisitMutableElements};

bool ll_Init(LinkedList_t *list, ADT_ElementTypeInfo_t elementType)
{
    if (list == NULL || elementType.elementSize == 0)
    {
        return false;
    }

    LinkedStorage_t storage;
    if (!linkedStorage_Init(&storage))
    {
        return false;
    }

    list->super = (ADT_Super_t){
        ._private = {
            .vtable = &LINKED_LIST_VTABLE,
            .size = 0,
            .elementType = elementType}};
    list->_private.storage = storage;

    return true;
}

bool ll_InitFrom(LinkedList_t *list, const void *elements, size_t initialCount, ADT_ElementTypeInfo_t elementType)
{
    if (list == NULL ||
        elementType.elementSize == 0 ||
        (elements == NULL && initialCount > 0) ||
        initialCount > SIZE_MAX / elementType.elementSize)
    {
        return false;
    }

    LinkedStorage_t storage;
    if (!linkedStorage_InitFrom(
            &storage,
            elements,
            initialCount,
            elementType.elementSize))
    {
        return false;
    }

    list->super = (ADT_Super_t){
        ._private = {
            .vtable = &LINKED_LIST_VTABLE,
            .size = initialCount,
            .elementType = elementType}};
    list->_private.storage = storage;
    return true;
}

bool ll_Get(const LinkedList_t *list, size_t index, void *outElement)
{
    if (list == NULL || outElement == NULL)
    {
        return false;
    }

    const void *element = linkedStorage_AtConst(
        &list->_private.storage,
        list->super._private.size,
        index);
    if (element == NULL)
    {
        return false;
    }

    memmove(outElement, element, list->super._private.elementType.elementSize);
    return true;
}

bool ll_detail_SetRef(LinkedList_t *list, size_t index, const void *element)
{
    if (list == NULL || element == NULL)
    {
        return false;
    }

    void *destination = linkedStorage_At(
        &list->_private.storage,
        list->super._private.size,
        index);
    if (destination == NULL)
    {
        return false;
    }

    if (destination == element)
    {
        return true;
    }

    if (list->super._private.elementType.destroy != NULL)
    {
        list->super._private.elementType.destroy(destination);
    }

    memmove(destination, element, list->super._private.elementType.elementSize);
    return true;
}

bool ll_detail_IndexOfRef(const LinkedList_t *list, const void *element, size_t *outIndex)
{
    if (list == NULL ||
        element == NULL ||
        outIndex == NULL ||
        list->super._private.elementType.elementSize == 0)
    {
        return false;
    }

    const CompareFn_t compare = list->super._private.elementType.compare;
    const size_t elementSize = list->super._private.elementType.elementSize;
    size_t index = 0;

    for (const LinkedStorageNode_t *node = list->_private.storage.head; node != NULL; node = node->next)
    {
        const bool matches =
            compare != NULL
                ? compare(node->data, element) == 0
                : memcmp(node->data, element, elementSize) == 0;

        if (matches)
        {
            *outIndex = index;
            return true;
        }

        index++;
    }

    return false;
}

bool ll_detail_ContainsRef(const LinkedList_t *list, const void *element)
{
    size_t index = 0;
    return ll_detail_IndexOfRef(list, element, &index);
}

bool ll_detail_InsertRef(LinkedList_t *list, size_t index, const void *element)
{
    if (list == NULL || element == NULL)
    {
        return false;
    }

    if (!linkedStorage_Insert(
            &list->_private.storage,
            list->super._private.size,
            index,
            element,
            list->super._private.elementType.elementSize))
    {
        return false;
    }

    list->super._private.size++;
    return true;
}

bool ll_detail_PrependRef(LinkedList_t *list, const void *element)
{
    return ll_detail_InsertRef(list, 0, element);
}

bool ll_detail_AppendRef(LinkedList_t *list, const void *element)
{
    if (list == NULL ||
        element == NULL ||
        list->super._private.elementType.elementSize == 0 ||
        list->super._private.size == SIZE_MAX)
    {
        return false;
    }

    if (!linkedStorage_Append(
            &list->_private.storage,
            element,
            list->super._private.elementType.elementSize))
    {
        return false;
    }

    list->super._private.size++;
    return true;
}

bool ll_Remove(LinkedList_t *list, size_t index)
{
    if (list == NULL)
    {
        return false;
    }

    void *element = linkedStorage_At(
        &list->_private.storage,
        list->super._private.size,
        index);
    if (element == NULL)
    {
        return false;
    }

    if (list->super._private.elementType.destroy != NULL)
    {
        list->super._private.elementType.destroy(element);
    }

    if (!linkedStorage_Erase(
            &list->_private.storage,
            list->super._private.size,
            index))
    {
        return false;
    }

    list->super._private.size--;
    return true;
}

bool ll_Take(LinkedList_t *list, size_t index, void *outElement)
{
    if (list == NULL ||
        outElement == NULL ||
        linkedStorage_ContainsAddress(
            &list->_private.storage,
            outElement,
            list->super._private.elementType.elementSize))
    {
        return false;
    }

    const void *element = linkedStorage_AtConst(
        &list->_private.storage,
        list->super._private.size,
        index);
    if (element == NULL)
    {
        return false;
    }

    memmove(outElement, element, list->super._private.elementType.elementSize);

    if (!linkedStorage_Erase(
            &list->_private.storage,
            list->super._private.size,
            index))
    {
        return false;
    }

    list->super._private.size--;
    return true;
}

void ll_Clear(LinkedList_t *list)
{
    if (list == NULL)
    {
        return;
    }

    if (list->super._private.elementType.destroy != NULL)
    {
        for (LinkedStorageNode_t *node = list->_private.storage.head; node != NULL; node = node->next)
        {
            list->super._private.elementType.destroy(node->data);
        }
    }

    linkedStorage_Destroy(&list->_private.storage);
    list->super._private.size = 0;
}

void ll_Destroy(LinkedList_t *list)
{
    if (list == NULL)
    {
        return;
    }

    ll_Clear(list);
    list->super = (ADT_Super_t){0};
}
