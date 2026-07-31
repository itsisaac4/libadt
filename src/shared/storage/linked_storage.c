#include "libadt/internal/storage/linked_storage.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const LinkedStorageNode_t *NodeAtConst(const LinkedStorage_t *storage, size_t count, size_t index)
{
    if (storage == NULL || index >= count)
    {
        return NULL;
    }

    if (index < count / 2)
    {
        const LinkedStorageNode_t *current = storage->head;
        for (size_t i = 0; current != NULL && i < index; i++)
        {
            current = current->next;
        }
        return current;
    }

    const LinkedStorageNode_t *current = storage->tail;
    for (size_t i = count - 1; current != NULL && i > index; i--)
    {
        current = current->previous;
    }
    return current;
}

static LinkedStorageNode_t *NodeAt(LinkedStorage_t *storage, size_t count, size_t index)
{
    return (LinkedStorageNode_t *)NodeAtConst(storage, count, index);
}

static LinkedStorageNode_t *CreateNode(const void *element, size_t elementSize)
{
    if (element == NULL || elementSize == 0)
    {
        return NULL;
    }

    LinkedStorageNode_t *node = malloc(sizeof(LinkedStorageNode_t));
    if (node == NULL)
    {
        return NULL;
    }

    node->data = malloc(elementSize);
    if (node->data == NULL)
    {
        free(node);
        return NULL;
    }

    memcpy(node->data, element, elementSize);
    node->previous = NULL;
    node->next = NULL;
    return node;
}

static void UnlinkNode(LinkedStorage_t *storage, LinkedStorageNode_t *node)
{
    if (node->previous != NULL)
    {
        node->previous->next = node->next;
    }
    else
    {
        storage->head = node->next;
    }

    if (node->next != NULL)
    {
        node->next->previous = node->previous;
    }
    else
    {
        storage->tail = node->previous;
    }
}

bool linkedStorage_Init(LinkedStorage_t *storage)
{
    if (storage == NULL)
    {
        return false;
    }

    storage->head = NULL;
    storage->tail = NULL;
    return true;
}

bool linkedStorage_InitFrom(LinkedStorage_t *storage, const void *elements, size_t count, size_t elementSize)
{
    if (storage == NULL ||
        elementSize == 0 ||
        (elements == NULL && count > 0) ||
        count > SIZE_MAX / elementSize)
    {
        return false;
    }

    LinkedStorage_t initializedStorage;
    if (!linkedStorage_Init(&initializedStorage))
    {
        return false;
    }

    for (size_t i = 0; i < count; i++)
    {
        const void *element = (const unsigned char *)elements + i * elementSize;

        if (!linkedStorage_Append(&initializedStorage, element, elementSize))
        {
            linkedStorage_Destroy(&initializedStorage);
            return false;
        }
    }

    *storage = initializedStorage;
    return true;
}

void *linkedStorage_At(LinkedStorage_t *storage, size_t count, size_t index)
{
    LinkedStorageNode_t *node = NodeAt(storage, count, index);
    return node == NULL ? NULL : node->data;
}

const void *linkedStorage_AtConst(const LinkedStorage_t *storage, size_t count, size_t index)
{
    const LinkedStorageNode_t *node = NodeAtConst(storage, count, index);
    return node == NULL ? NULL : node->data;
}

bool linkedStorage_Insert(LinkedStorage_t *storage, size_t count, size_t index, const void *element, size_t elementSize)
{
    if (storage == NULL ||
        element == NULL ||
        elementSize == 0 ||
        index > count ||
        count == SIZE_MAX)
    {
        return false;
    }

    if (index == 0)
    {
        return linkedStorage_Prepend(storage, element, elementSize);
    }

    if (index == count)
    {
        return linkedStorage_Append(storage, element, elementSize);
    }

    LinkedStorageNode_t *next = NodeAt(storage, count, index);
    if (next == NULL)
    {
        return false;
    }

    LinkedStorageNode_t *node = CreateNode(element, elementSize);
    if (node == NULL)
    {
        return false;
    }

    node->previous = next->previous;
    node->next = next;
    next->previous->next = node;
    next->previous = node;
    return true;
}

bool linkedStorage_Prepend(LinkedStorage_t *storage, const void *element, size_t elementSize)
{
    if (storage == NULL || element == NULL || elementSize == 0)
    {
        return false;
    }

    LinkedStorageNode_t *node = CreateNode(element, elementSize);
    if (node == NULL)
    {
        return false;
    }

    node->next = storage->head;

    if (storage->head == NULL)
    {
        storage->tail = node;
    }
    else
    {
        storage->head->previous = node;
    }

    storage->head = node;
    return true;
}

bool linkedStorage_Append(LinkedStorage_t *storage, const void *element, size_t elementSize)
{
    if (storage == NULL || element == NULL || elementSize == 0)
    {
        return false;
    }

    LinkedStorageNode_t *node = CreateNode(element, elementSize);
    if (node == NULL)
    {
        return false;
    }

    node->previous = storage->tail;

    if (storage->tail == NULL)
    {
        storage->head = node;
    }
    else
    {
        storage->tail->next = node;
    }

    storage->tail = node;
    return true;
}

bool linkedStorage_Erase(LinkedStorage_t *storage, size_t count, size_t index)
{
    LinkedStorageNode_t *node = NodeAt(storage, count, index);
    if (node == NULL)
    {
        return false;
    }

    UnlinkNode(storage, node);
    free(node->data);
    free(node);
    return true;
}

bool linkedStorage_ContainsAddress(const LinkedStorage_t *storage, const void *address, size_t elementSize)
{
    if (storage == NULL || address == NULL || elementSize == 0)
    {
        return false;
    }

    const uintptr_t inspectedAddress = (uintptr_t)address;

    for (const LinkedStorageNode_t *node = storage->head; node != NULL; node = node->next)
    {
        const uintptr_t elementAddress = (uintptr_t)node->data;

        if (inspectedAddress >= elementAddress &&
            inspectedAddress - elementAddress < elementSize)
        {
            return true;
        }
    }

    return false;
}

void linkedStorage_Destroy(LinkedStorage_t *storage)
{
    if (storage == NULL)
    {
        return;
    }

    LinkedStorageNode_t *node = storage->head;
    while (node != NULL)
    {
        LinkedStorageNode_t *next = node->next;
        free(node->data);
        free(node);
        node = next;
    }

    storage->head = NULL;
    storage->tail = NULL;
}
