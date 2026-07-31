#include "libadt/queue.h"

#include <stdint.h>
#include <string.h>

static void VisitElements(const ADT_Super_t *adt, ADT_ConstVisitFn_t visitor, void *context)
{
    if (adt == NULL || visitor == NULL)
    {
        return;
    }

    const Queue_t *queue = (const Queue_t *)adt;
    size_t index = 0;

    for (const LinkedStorageNode_t *node = queue->_private.storage.head; node != NULL; node = node->next)
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

    Queue_t *queue = (Queue_t *)adt;
    size_t index = 0;

    for (LinkedStorageNode_t *node = queue->_private.storage.head; node != NULL; node = node->next)
    {
        visitor(node->data, index, context);
        index++;
    }
}

static const ADT_VTable_t QUEUE_VTABLE = {
    .containerName = "Queue",
    .visit = VisitElements,
    .visitMutable = VisitMutableElements};

bool qu_Init(Queue_t *queue, ADT_ElementTypeInfo_t elementType)
{
    if (queue == NULL || elementType.elementSize == 0)
    {
        return false;
    }

    LinkedStorage_t storage;
    if (!linkedStorage_Init(&storage))
    {
        return false;
    }

    queue->super = (ADT_Super_t){
        ._private = {
            .vtable = &QUEUE_VTABLE,
            .size = 0,
            .elementType = elementType}};
    queue->_private.storage = storage;
    return true;
}

bool qu_InitFrom(Queue_t *queue, const void *elements, size_t initialCount, ADT_ElementTypeInfo_t elementType)
{
    if (queue == NULL ||
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

    queue->super = (ADT_Super_t){
        ._private = {
            .vtable = &QUEUE_VTABLE,
            .size = initialCount,
            .elementType = elementType}};
    queue->_private.storage = storage;
    return true;
}

bool qu_Front(const Queue_t *queue, void *outElement)
{
    if (queue == NULL ||
        outElement == NULL ||
        queue->super._private.size == 0 ||
        queue->_private.storage.head == NULL)
    {
        return false;
    }

    memmove(
        outElement,
        queue->_private.storage.head->data,
        queue->super._private.elementType.elementSize);
    return true;
}

bool qu_Back(const Queue_t *queue, void *outElement)
{
    if (queue == NULL ||
        outElement == NULL ||
        queue->super._private.size == 0 ||
        queue->_private.storage.tail == NULL)
    {
        return false;
    }

    memmove(
        outElement,
        queue->_private.storage.tail->data,
        queue->super._private.elementType.elementSize);
    return true;
}

bool qu_detail_EnqueueRef(Queue_t *queue, const void *element)
{
    if (queue == NULL ||
        element == NULL ||
        queue->super._private.elementType.elementSize == 0 ||
        queue->super._private.size == SIZE_MAX)
    {
        return false;
    }

    if (!linkedStorage_Append(
            &queue->_private.storage,
            element,
            queue->super._private.elementType.elementSize))
    {
        return false;
    }

    queue->super._private.size++;
    return true;
}

bool qu_Dequeue(Queue_t *queue, void *outElement)
{
    if (queue == NULL ||
        outElement == NULL ||
        queue->super._private.size == 0 ||
        linkedStorage_ContainsAddress(
            &queue->_private.storage,
            outElement,
            queue->super._private.elementType.elementSize))
    {
        return false;
    }

    memmove(
        outElement,
        queue->_private.storage.head->data,
        queue->super._private.elementType.elementSize);

    if (!linkedStorage_Erase(
            &queue->_private.storage,
            queue->super._private.size,
            0))
    {
        return false;
    }

    queue->super._private.size--;
    return true;
}

bool qu_Discard(Queue_t *queue)
{
    if (queue == NULL ||
        queue->super._private.size == 0 ||
        queue->_private.storage.head == NULL)
    {
        return false;
    }

    if (queue->super._private.elementType.destroy != NULL)
    {
        queue->super._private.elementType.destroy(queue->_private.storage.head->data);
    }

    if (!linkedStorage_Erase(
            &queue->_private.storage,
            queue->super._private.size,
            0))
    {
        return false;
    }

    queue->super._private.size--;
    return true;
}

void qu_Clear(Queue_t *queue)
{
    if (queue == NULL)
    {
        return;
    }

    if (queue->super._private.elementType.destroy != NULL)
    {
        for (LinkedStorageNode_t *node = queue->_private.storage.head; node != NULL; node = node->next)
        {
            queue->super._private.elementType.destroy(node->data);
        }
    }

    linkedStorage_Destroy(&queue->_private.storage);
    queue->super._private.size = 0;
}

void qu_Destroy(Queue_t *queue)
{
    if (queue == NULL)
    {
        return;
    }

    qu_Clear(queue);
    queue->super = (ADT_Super_t){0};
}
