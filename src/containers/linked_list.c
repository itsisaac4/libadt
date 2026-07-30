#include "libadt/linked_list.h"

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
    const LinkedListNode_t *current = list->head;
    size_t index = 0;

    while (current != NULL)
    {
        visitor(current->data, index, context);
        current = current->next;
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
    LinkedListNode_t *current = list->head;
    size_t index = 0;

    while (current != NULL)
    {
        visitor(current->data, index, context);
        current = current->next;
        index++;
    }
}

static const ADT_VTable_t LINKED_LIST_VTABLE = {
    .containerName = "LinkedList",
    .visit = VisitElements,
    .visitMutable = VisitMutableElements};

static LinkedListNode_t *NodeAt(const LinkedList_t *list, size_t index)
{
    if (list == NULL || index >= list->super.size)
    {
        return NULL;
    }

    if (index < list->super.size / 2)
    {
        LinkedListNode_t *current = list->head;
        for (size_t i = 0; i < index; i++)
        {
            current = current->next;
        }
        return current;
    }

    LinkedListNode_t *current = list->tail;
    for (size_t i = list->super.size - 1; i > index; i--)
    {
        current = current->previous;
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

    node->data = malloc(list->super.type.elementSize);

    if (node->data == NULL)
    {
        free(node);
        return NULL;
    }

    memcpy(node->data, element, list->super.type.elementSize);
    node->previous = NULL;
    node->next = NULL;

    return node;
}

static void UnlinkNode(LinkedList_t *list, LinkedListNode_t *node)
{
    if (node->previous != NULL)
    {
        node->previous->next = node->next;
    }
    else
    {
        list->head = node->next;
    }

    if (node->next != NULL)
    {
        node->next->previous = node->previous;
    }
    else
    {
        list->tail = node->previous;
    }

    list->super.size--;
}

static void ReleaseNodes(LinkedList_t *list, bool destroyElements)
{
    LinkedListNode_t *current = list->head;

    while (current != NULL)
    {
        LinkedListNode_t *next = current->next;

        if (destroyElements && list->super.type.destroy != NULL)
        {
            list->super.type.destroy(current->data);
        }

        free(current->data);
        free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->super.size = 0;
}

static bool PointsIntoStorage(const LinkedList_t *list, const void *pointer)
{
    const uintptr_t address = (uintptr_t)pointer;

    for (const LinkedListNode_t *node = list->head; node != NULL; node = node->next)
    {
        const uintptr_t storage = (uintptr_t)node->data;

        if (address >= storage &&
            address - storage < list->super.type.elementSize)
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
        .vtable = &LINKED_LIST_VTABLE,
        .size = 0,
        .type = typeInfo};
    list->head = NULL;
    list->tail = NULL;

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

        if (!ll_AppendRef(&initializedList, element))
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

    memmove(outElement, node->data, list->super.type.elementSize);
    return true;
}

bool ll_SetRef(LinkedList_t *list, size_t index, const void *element)
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

    if (node->data == element)
    {
        return true;
    }

    if (list->super.type.destroy != NULL)
    {
        list->super.type.destroy(node->data);
    }

    memmove(node->data, element, list->super.type.elementSize);
    return true;
}

bool ll_IndexOfRef(const LinkedList_t *list, const void *element, size_t *outIndex)
{
    if (list == NULL ||
        element == NULL ||
        outIndex == NULL ||
        list->super.type.elementSize == 0)
    {
        return false;
    }

    LinkedListNode_t *current = list->head;
    size_t index = 0;

    while (current != NULL)
    {
        const bool matches =
            list->super.type.compare != NULL
                ? list->super.type.compare(current->data, element) == 0
                : memcmp(
                      current->data,
                      element,
                      list->super.type.elementSize) == 0;

        if (matches)
        {
            *outIndex = index;
            return true;
        }

        current = current->next;
        index++;
    }

    return false;
}

bool ll_ContainsRef(const LinkedList_t *list, const void *element)
{
    size_t index = 0;
    return ll_IndexOfRef(list, element, &index);
}

bool ll_InsertRef(LinkedList_t *list, size_t index, const void *element)
{
    if (list == NULL ||
        element == NULL ||
        list->super.type.elementSize == 0 ||
        index > list->super.size ||
        list->super.size == SIZE_MAX)
    {
        return false;
    }

    if (index == list->super.size)
    {
        return ll_AppendRef(list, element);
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

    node->previous = next->previous;
    node->next = next;

    if (next->previous != NULL)
    {
        next->previous->next = node;
    }
    else
    {
        list->head = node;
    }

    next->previous = node;
    list->super.size++;

    return true;
}

bool ll_PrependRef(LinkedList_t *list, const void *element)
{
    return ll_InsertRef(list, 0, element);
}

bool ll_AppendRef(LinkedList_t *list, const void *element)
{
    if (list == NULL ||
        element == NULL ||
        list->super.type.elementSize == 0 ||
        list->super.size == SIZE_MAX)
    {
        return false;
    }

    LinkedListNode_t *node = CreateNode(list, element);
    if (node == NULL)
    {
        return false;
    }

    node->previous = list->tail;

    if (list->tail == NULL)
    {
        list->head = node;
    }
    else
    {
        list->tail->next = node;
    }

    list->tail = node;
    list->super.size++;

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

    if (list->super.type.destroy != NULL)
    {
        list->super.type.destroy(node->data);
    }

    UnlinkNode(list, node);
    free(node->data);
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

    memmove(outElement, node->data, list->super.type.elementSize);
    UnlinkNode(list, node);
    free(node->data);
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
