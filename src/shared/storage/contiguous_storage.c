#include "libadt/internal/storage/contiguous_storage.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CONTIGUOUS_STORAGE_INITIAL_CAPACITY 8

static void shiftElementsAfterIndexLeft(ContiguousStorage_t *storage, size_t count, size_t index, size_t elementSize)
{
    size_t elementsToMove = count - index - 1;
    if (elementsToMove > 0)
    {
        memmove(
            contiguousStorage_At(storage, index, elementSize),
            contiguousStorage_At(storage, index + 1, elementSize),
            elementsToMove * elementSize);
    }
}

static void ShiftElementsAfterIndexRight(ContiguousStorage_t *storage, size_t count, size_t index, size_t elementSize)
{
    size_t elementsToMove = count - index;
    if (elementsToMove > 0)
    {
        memmove(
            contiguousStorage_At(storage, index + 1, elementSize),
            contiguousStorage_At(storage, index, elementSize),
            elementsToMove * elementSize);
    }
}

bool contiguousStorage_Init(ContiguousStorage_t *storage, size_t elementSize)
{
    if (storage == NULL ||
        elementSize == 0 ||
        CONTIGUOUS_STORAGE_INITIAL_CAPACITY > SIZE_MAX / elementSize)
    {
        return false;
    }

    void *data = malloc(CONTIGUOUS_STORAGE_INITIAL_CAPACITY * elementSize);
    if (data == NULL)
    {
        return false;
    }

    *storage = (ContiguousStorage_t){
        .data = data,
        .capacity = CONTIGUOUS_STORAGE_INITIAL_CAPACITY};

    return true;
}

bool contiguousStorage_InitFrom(ContiguousStorage_t *storage, const void *elements, size_t count, size_t elementSize)
{
    if (storage == NULL ||
        elementSize == 0 ||
        (elements == NULL && count > 0) ||
        count > SIZE_MAX / elementSize)
    {
        return false;
    }

    if (count == 0)
    {
        *storage = (ContiguousStorage_t){0};
        return true;
    }

    void *data = malloc(count * elementSize);
    if (data == NULL)
    {
        return false;
    }

    memcpy(data, elements, count * elementSize);

    *storage = (ContiguousStorage_t){
        .data = data,
        .capacity = count};

    return true;
}

bool contiguousStorage_Reserve(ContiguousStorage_t *storage, size_t requiredCapacity, size_t elementSize)
{
    if (storage == NULL ||
        elementSize == 0 ||
        (storage->data == NULL && storage->capacity != 0) ||
        (storage->data != NULL && storage->capacity == 0))
    {
        return false;
    }

    if (requiredCapacity <= storage->capacity)
    {
        return true;
    }

    size_t newCapacity = storage->capacity == 0
                             ? CONTIGUOUS_STORAGE_INITIAL_CAPACITY
                             : storage->capacity;

    while (newCapacity < requiredCapacity)
    {
        if (newCapacity > SIZE_MAX / 2)
        {
            return false;
        }

        newCapacity *= 2;
    }

    if (newCapacity > SIZE_MAX / elementSize)
    {
        return false;
    }

    void *data = realloc(storage->data, newCapacity * elementSize);
    if (data == NULL)
    {
        return false;
    }

    storage->data = data;
    storage->capacity = newCapacity;
    return true;
}

void *contiguousStorage_At(ContiguousStorage_t *storage, size_t index, size_t elementSize)
{
    if (storage == NULL ||
        storage->data == NULL ||
        elementSize == 0 ||
        index >= storage->capacity)
    {
        return NULL;
    }

    return (unsigned char *)storage->data + index * elementSize;
}

const void *contiguousStorage_AtConst(const ContiguousStorage_t *storage, size_t index, size_t elementSize)
{
    if (storage == NULL ||
        storage->data == NULL ||
        elementSize == 0 ||
        index >= storage->capacity)
    {
        return NULL;
    }

    return (const unsigned char *)storage->data + index * elementSize;
}

bool contiguousStorage_Insert(ContiguousStorage_t *storage, size_t count, size_t index, const void *element, size_t elementSize)
{
    if (storage == NULL ||
        element == NULL ||
        elementSize == 0 ||
        (storage->data == NULL && storage->capacity != 0) ||
        (storage->data != NULL && storage->capacity == 0) ||
        count > storage->capacity ||
        index > count ||
        count == SIZE_MAX)
    {
        return false;
    }

    void *elementCopy = NULL;
    const void *source = element;

    if (contiguousStorage_ContainsAddress(storage, element, elementSize))
    {
        elementCopy = malloc(elementSize);
        if (elementCopy == NULL)
        {
            return false;
        }

        memcpy(elementCopy, element, elementSize);
        source = elementCopy;
    }

    if (!contiguousStorage_Reserve(storage, count + 1, elementSize))
    {
        free(elementCopy);
        return false;
    }

    ShiftElementsAfterIndexRight(storage, count, index, elementSize);
    memcpy(contiguousStorage_At(storage, index, elementSize), source, elementSize);
    free(elementCopy);

    return true;
}

bool contiguousStorage_Erase(ContiguousStorage_t *storage, size_t count, size_t index, size_t elementSize)
{
    if (storage == NULL ||
        storage->data == NULL ||
        elementSize == 0 ||
        count == 0 ||
        count > storage->capacity ||
        index >= count)
    {
        return false;
    }

    shiftElementsAfterIndexLeft(storage, count, index, elementSize);

    return true;
}

bool contiguousStorage_ContainsAddress(const ContiguousStorage_t *storage, const void *address, size_t elementSize)
{
    if (storage == NULL ||
        storage->data == NULL ||
        address == NULL ||
        elementSize == 0 ||
        storage->capacity > SIZE_MAX / elementSize)
    {
        return false;
    }

    const uintptr_t storageAddress = (uintptr_t)storage->data;
    const uintptr_t inspectedAddress = (uintptr_t)address;
    const size_t storageSize = storage->capacity * elementSize;

    return inspectedAddress >= storageAddress && inspectedAddress - storageAddress < storageSize;
}

bool contiguousStorage_BinarySearchBy(const ContiguousStorage_t *storage, size_t count, size_t elementSize, CompareFn_t compare, const void *target, size_t *outIndex)
{
    if (storage == NULL ||
        elementSize == 0 ||
        compare == NULL ||
        target == NULL ||
        outIndex == NULL ||
        (storage->data == NULL && storage->capacity != 0) ||
        (storage->data != NULL && storage->capacity == 0) ||
        storage->capacity > SIZE_MAX / elementSize ||
        count > storage->capacity ||
        (count > 0 && storage->data == NULL))
    {
        return false;
    }

    size_t left = 0;
    size_t right = count;

    while (left < right)
    {
        size_t middle = left + (right - left) / 2;

        const void *element = contiguousStorage_AtConst(storage, middle, elementSize);

        if (compare(element, target) < 0)
        {
            left = middle + 1;
        }
        else
        {
            right = middle;
        }
    }

    if (left < count)
    {
        const void *element = contiguousStorage_AtConst(storage, left, elementSize);

        if (compare(element, target) == 0)
        {
            *outIndex = left;
            return true;
        }
    }

    return false;
}

void contiguousStorage_Destroy(ContiguousStorage_t *storage)
{
    if (storage == NULL)
    {
        return;
    }

    free(storage->data);
    storage->data = NULL;
    storage->capacity = 0;
}
