#include "libadt/linked_list.h"
#include "libadt/detail/linked_list_node.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void VisitElements(const ADT_Super_t *adt, ADT_ConstVisitFn_t visitor, void *context)
{
    if (adt == NULL || visitor == NULL)
    {
        return;
    }

    const LinkedList_t *list = (const LinkedList_t *)adt;
    const LinkedListNode_t *current = list->_private.head;
    size_t index = 0;

    while (current != NULL)
    {
        visitor(current->_private.data, index, context);
        current = current->_private.next;
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
    LinkedListNode_t *current = list->_private.head;
    size_t index = 0;

    while (current != NULL)
    {
        visitor(current->_private.data, index, context);
        current = current->_private.next;
        index++;
    }
}

static const ADT_VTable_t LINKED_LIST_VTABLE = {
    .containerName = "LinkedList",
    .visit = VisitElements,
    .visitMutable = VisitMutableElements};

static LinkedListNode_t *NodeAt(const LinkedList_t *list, size_t index)
{
    if (list == NULL || index >= list->super._private.size)
    {
        return NULL;
    }

    if (index < list->super._private.size / 2)
    {
        LinkedListNode_t *current = list->_private.head;
        for (size_t i = 0; i < index; i++)
        {
            current = current->_private.next;
        }
        return current;
    }

    LinkedListNode_t *current = list->_private.tail;
    for (size_t i = list->super._private.size - 1; i > index; i--)
    {
        current = current->_private.previous;
    }
    return current;
}

static LinkedListNode_t *CreateNode(const LinkedList_t *list, const void *element)
{
    LinkedListNode_t *node = malloc(sizeof(LinkedListNode_t));

    if (node == NULL)
    {
        return NULL;
    }

    node->_private.data = malloc(list->super._private.type.elementSize);

    if (node->_private.data == NULL)
    {
        free(node);
        return NULL;
    }

    memcpy(node->_private.data, element, list->super._private.type.elementSize);
    node->_private.previous = NULL;
    node->_private.next = NULL;

    return node;
}

static void UnlinkNode(LinkedList_t *list, LinkedListNode_t *node)
{
    if (node->_private.previous != NULL)
    {
        node->_private.previous->_private.next = node->_private.next;
    }
    else
    {
        list->_private.head = node->_private.next;
    }

    if (node->_private.next != NULL)
    {
        node->_private.next->_private.previous = node->_private.previous;
    }
    else
    {
        list->_private.tail = node->_private.previous;
    }

    list->super._private.size--;
}

static void ReleaseNodes(LinkedList_t *list, bool destroyElements)
{
    LinkedListNode_t *current = list->_private.head;

    while (current != NULL)
    {
        LinkedListNode_t *next = current->_private.next;

        if (destroyElements && list->super._private.type.destroy != NULL)
        {
            list->super._private.type.destroy(current->_private.data);
        }

        free(current->_private.data);
        free(current);
        current = next;
    }

    list->_private.head = NULL;
    list->_private.tail = NULL;
    list->super._private.size = 0;
}

static bool PointsIntoStorage(const LinkedList_t *list, const void *pointer)
{
    const uintptr_t address = (uintptr_t)pointer;

    for (const LinkedListNode_t *node = list->_private.head; node != NULL; node = node->_private.next)
    {
        const uintptr_t storage = (uintptr_t)node->_private.data;

        if (address >= storage &&
            address - storage < list->super._private.type.elementSize)
        {
            return true;
        }
    }

    return false;
}

bool ll_Init(LinkedList_t *list, ADT_TypeInfo_t typeInfo)
{
    if (list == NULL || typeInfo.elementSize == 0)
    {
        return false;
    }

    list->super = (ADT_Super_t){
        ._private = {
            .vtable = &LINKED_LIST_VTABLE,
            .size = 0,
            .type = typeInfo}};
    list->_private.head = NULL;
    list->_private.tail = NULL;

    return true;
}

bool ll_InitFrom(LinkedList_t *list, const void *elements, size_t initialCount, ADT_TypeInfo_t typeInfo)
{
    if (list == NULL ||
        typeInfo.elementSize == 0 ||
        (elements == NULL && initialCount > 0) ||
        initialCount > SIZE_MAX / typeInfo.elementSize)
    {
        return false;
    }

    LinkedList_t initializedList;
    if (!ll_Init(&initializedList, typeInfo))
    {
        return false;
    }

    for (size_t i = 0; i < initialCount; i++)
    {
        const void *element = (const unsigned char *)elements + i * typeInfo.elementSize;

        if (!ll_detail_AppendRef(&initializedList, element))
        {
            ReleaseNodes(&initializedList, false);
            return false;
        }
    }

    *list = initializedList;
    return true;
}

bool ll_Get(const LinkedList_t *list, size_t index, void *outElement)
{
    if (list == NULL || outElement == NULL)
    {
        return false;
    }

    LinkedListNode_t *node = NodeAt(list, index);
    if (node == NULL)
    {
        return false;
    }

    memmove(outElement, node->_private.data, list->super._private.type.elementSize);
    return true;
}

bool ll_detail_SetRef(LinkedList_t *list, size_t index, const void *element)
{
    if (list == NULL || element == NULL)
    {
        return false;
    }

    LinkedListNode_t *node = NodeAt(list, index);
    if (node == NULL)
    {
        return false;
    }

    if (node->_private.data == element)
    {
        return true;
    }

    if (list->super._private.type.destroy != NULL)
    {
        list->super._private.type.destroy(node->_private.data);
    }

    memmove(node->_private.data, element, list->super._private.type.elementSize);
    return true;
}

bool ll_detail_IndexOfRef(const LinkedList_t *list, const void *element, size_t *outIndex)
{
    if (list == NULL ||
        element == NULL ||
        outIndex == NULL ||
        list->super._private.type.elementSize == 0)
    {
        return false;
    }

    LinkedListNode_t *current = list->_private.head;
    size_t index = 0;

    while (current != NULL)
    {
        const bool matches =
            list->super._private.type.compare != NULL
                ? list->super._private.type.compare(current->_private.data, element) == 0
                : memcmp(
                      current->_private.data,
                      element,
                      list->super._private.type.elementSize) == 0;

        if (matches)
        {
            *outIndex = index;
            return true;
        }

        current = current->_private.next;
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
    if (list == NULL ||
        element == NULL ||
        list->super._private.type.elementSize == 0 ||
        index > list->super._private.size ||
        list->super._private.size == SIZE_MAX)
    {
        return false;
    }

    if (index == list->super._private.size)
    {
        return ll_detail_AppendRef(list, element);
    }

    LinkedListNode_t *next = NodeAt(list, index);
    if (next == NULL)
    {
        return false;
    }

    LinkedListNode_t *node = CreateNode(list, element);
    if (node == NULL)
    {
        return false;
    }

    node->_private.previous = next->_private.previous;
    node->_private.next = next;

    if (next->_private.previous != NULL)
    {
        next->_private.previous->_private.next = node;
    }
    else
    {
        list->_private.head = node;
    }

    next->_private.previous = node;
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
        list->super._private.type.elementSize == 0 ||
        list->super._private.size == SIZE_MAX)
    {
        return false;
    }

    LinkedListNode_t *node = CreateNode(list, element);
    if (node == NULL)
    {
        return false;
    }

    node->_private.previous = list->_private.tail;

    if (list->_private.tail == NULL)
    {
        list->_private.head = node;
    }
    else
    {
        list->_private.tail->_private.next = node;
    }

    list->_private.tail = node;
    list->super._private.size++;

    return true;
}

bool ll_Remove(LinkedList_t *list, size_t index)
{
    if (list == NULL)
    {
        return false;
    }

    LinkedListNode_t *node = NodeAt(list, index);
    if (node == NULL)
    {
        return false;
    }

    if (list->super._private.type.destroy != NULL)
    {
        list->super._private.type.destroy(node->_private.data);
    }

    UnlinkNode(list, node);
    free(node->_private.data);
    free(node);

    return true;
}

bool ll_Take(LinkedList_t *list, size_t index, void *outElement)
{
    if (list == NULL ||
        outElement == NULL ||
        PointsIntoStorage(list, outElement))
    {
        return false;
    }

    LinkedListNode_t *node = NodeAt(list, index);
    if (node == NULL)
    {
        return false;
    }

    memmove(outElement, node->_private.data, list->super._private.type.elementSize);
    UnlinkNode(list, node);
    free(node->_private.data);
    free(node);

    return true;
}

void ll_Clear(LinkedList_t *list)
{
    if (list == NULL)
    {
        return;
    }

    ReleaseNodes(list, true);
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
