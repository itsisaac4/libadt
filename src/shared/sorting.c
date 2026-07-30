#include "libadt/abstract_data_type.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    unsigned char *buffer;
    size_t stride;
} SortBufferContext_t;

typedef bool (*SortBufferFn_t)(unsigned char *buffer, size_t count, size_t elementSize, CompareFn_t compare, void *tmp);

static void CopyToBuffer(const void *element, size_t index, void *context)
{
    SortBufferContext_t *sort = (SortBufferContext_t *)context;
    memcpy(sort->buffer + index * sort->stride, element, sort->stride);
}

static void CopyFromBuffer(void *element, size_t index, void *context)
{
    SortBufferContext_t *sort = (SortBufferContext_t *)context;
    memcpy(element, sort->buffer + index * sort->stride, sort->stride);
}

static unsigned char *BufferElement(unsigned char *buffer, size_t index, size_t elementSize)
{
    return buffer + index * elementSize;
}

static void SwapElements(void *first, void *second, void *tmp, size_t elementSize)
{
    if (first == second)
    {
        return;
    }

    memcpy(tmp, first, elementSize);
    memcpy(first, second, elementSize);
    memcpy(second, tmp, elementSize);
}

static bool BubbleSortBuffer(unsigned char *buffer, size_t count, size_t elementSize, CompareFn_t compare, void *tmp)
{
    for (size_t end = count; end > 1; end--)
    {
        bool swapped = false;

        for (size_t i = 1; i < end; i++)
        {
            void *left = BufferElement(buffer, i - 1, elementSize);
            void *right = BufferElement(buffer, i, elementSize);

            if (compare(left, right) > 0)
            {
                SwapElements(left, right, tmp, elementSize);
                swapped = true;
            }
        }
        if (!swapped)
        {
            return true;
        }
    }
    return true;
}

static bool SelectionSortBuffer(unsigned char *buffer, size_t count, size_t elementSize, CompareFn_t compare, void *tmp)
{
    for (size_t current = 0; current + 1 < count; current++)
    {
        size_t smallest = current;

        for (size_t test = current + 1; test < count; test++)
        {
            void *testElement = BufferElement(buffer, test, elementSize);
            void *smallestElement = BufferElement(buffer, smallest, elementSize);

            if (compare(testElement, smallestElement) < 0)
            {
                smallest = test;
            }
        }

        if (smallest != current)
        {
            void *currentElement = BufferElement(buffer, current, elementSize);
            void *smallestElement = BufferElement(buffer, smallest, elementSize);
            SwapElements(currentElement, smallestElement, tmp, elementSize);
        }
    }

    return true;
}

static bool InsertionSortBuffer(unsigned char *buffer, size_t count, size_t elementSize, CompareFn_t compare, void *tmp)
{
    for (size_t current = 1; current < count; current++)
    {
        memcpy(tmp, BufferElement(buffer, current, elementSize), elementSize);

        size_t insertionIndex = current;

        while (insertionIndex > 0)
        {
            void *previous = BufferElement(
                buffer,
                insertionIndex - 1,
                elementSize);

            if (compare(previous, tmp) <= 0)
            {
                break;
            }

            memcpy(
                BufferElement(buffer, insertionIndex, elementSize),
                previous,
                elementSize);

            insertionIndex--;
        }

        memcpy(
            BufferElement(buffer, insertionIndex, elementSize),
            tmp,
            elementSize);
    }

    return true;
}

static size_t PartitionBuffer(unsigned char *buffer, size_t begin, size_t end, size_t elementSize, CompareFn_t compare, void *tmp)
{
    size_t pivotIndex = begin + (end - begin) / 2;
    void *pivot = BufferElement(buffer, pivotIndex, elementSize);
    void *last = BufferElement(buffer, end - 1, elementSize);

    SwapElements(pivot, last, tmp, elementSize);

    size_t insertionIndex = begin;

    for (size_t current = begin; current < end - 1; current++)
    {
        void *currentElement = BufferElement(buffer, current, elementSize);

        if (compare(currentElement, last) < 0)
        {
            void *insertionElement = BufferElement(buffer, insertionIndex, elementSize);

            SwapElements(currentElement, insertionElement, tmp, elementSize);

            insertionIndex++;
        }
    }

    SwapElements(BufferElement(buffer, insertionIndex, elementSize), last, tmp, elementSize);

    return insertionIndex;
}

static void QuickSortRange(unsigned char *buffer, size_t begin, size_t end, size_t elementSize, CompareFn_t compare, void *tmp)
{
    while (end - begin >= 2)
    {
        size_t pivot = PartitionBuffer(buffer, begin, end, elementSize, compare, tmp);
        size_t rightBegin = pivot + 1;

        if (pivot - begin < end - rightBegin)
        {
            QuickSortRange(buffer, begin, pivot, elementSize, compare, tmp);
            begin = rightBegin;
        }
        else
        {
            QuickSortRange(buffer, rightBegin, end, elementSize, compare, tmp);
            end = pivot;
        }
    }
}

static bool QuickSortBuffer(unsigned char *buffer, size_t count, size_t elementSize, CompareFn_t compare, void *tmp)
{
    QuickSortRange(buffer, 0, count, elementSize, compare, tmp);
    return true;
}

static const size_t BOGO_MAX_ELEMENTS = 8;
static const size_t BOGO_MAX_SHUFFLES = 100000;

static uint64_t NextRandom(uint64_t *state)
{
    *state ^= *state << 13;
    *state ^= *state >> 7;
    *state ^= *state << 17;
    return *state;
}

static bool BufferIsSorted(unsigned char *buffer, size_t count, size_t elementSize, CompareFn_t compare)
{
    for (size_t i = 1; i < count; i++)
    {
        void *previous = BufferElement(buffer, i - 1, elementSize);
        void *current = BufferElement(buffer, i, elementSize);

        if (compare(previous, current) > 0)
        {
            return false;
        }
    }

    return true;
}

static void ShuffleBuffer(unsigned char *buffer, size_t count, size_t elementSize, void *tmp, uint64_t *state)
{
    for (size_t remaining = count; remaining > 1; remaining--)
    {
        size_t randomIndex =
            (size_t)(NextRandom(state) % (uint64_t)remaining);

        SwapElements(
            BufferElement(buffer, remaining - 1, elementSize),
            BufferElement(buffer, randomIndex, elementSize),
            tmp,
            elementSize);
    }
}

static bool BogoSortBuffer(unsigned char *buffer, size_t count, size_t elementSize, CompareFn_t compare, void *tmp)
{
    if (BufferIsSorted(buffer, count, elementSize, compare))
    {
        return true;
    }

    if (count > BOGO_MAX_ELEMENTS)
    {
        return false;
    }

    uint64_t randomState =
        UINT64_C(0x9E3779B97F4A7C15) ^
        (uint64_t)count ^
        ((uint64_t)elementSize << 32);

    for (size_t attempt = 0; attempt < BOGO_MAX_SHUFFLES; attempt++)
    {
        ShuffleBuffer(
            buffer,
            count,
            elementSize,
            tmp,
            &randomState);

        if (BufferIsSorted(buffer, count, elementSize, compare))
        {
            return true;
        }
    }

    return false;
}

bool adt_SortBy(ADT_t *adt, ADT_SortAlgorithm_t algorithm, CompareFn_t compare)
{
    ADT_Super_t *super = (ADT_Super_t *)adt;
    SortBufferFn_t sortBuffer;

    if (super == NULL ||
        super->_private.vtable == NULL ||
        super->_private.vtable->visit == NULL ||
        super->_private.vtable->visitMutable == NULL ||
        super->_private.type.elementSize == 0 ||
        compare == NULL)
    {
        return false;
    }

    switch (algorithm)
    {
    case ADT_SORT_BUBBLE:
        sortBuffer = BubbleSortBuffer;
        break;
    case ADT_SORT_SELECTION:
        sortBuffer = SelectionSortBuffer;
        break;
    case ADT_SORT_INSERTION:
        sortBuffer = InsertionSortBuffer;
        break;
    case ADT_SORT_QUICK:
        sortBuffer = QuickSortBuffer;
        break;
    case ADT_SORT_BOGO:
        sortBuffer = BogoSortBuffer;
        break;
    default:
        return false;
    }

    if (super->_private.size < 2)
    {
        return true;
    }

    if (super->_private.size > SIZE_MAX / super->_private.type.elementSize)
    {
        return false;
    }

    size_t bufferSize = super->_private.size * super->_private.type.elementSize;
    unsigned char *buffer = malloc(bufferSize);
    void *tmp = malloc(super->_private.type.elementSize);

    if (buffer == NULL || tmp == NULL)
    {
        free(buffer);
        free(tmp);
        return false;
    }

    SortBufferContext_t context = {
        .buffer = buffer,
        .stride = super->_private.type.elementSize};

    if (!adt_ForEach(adt, CopyToBuffer, &context))
    {
        free(tmp);
        free(buffer);
        return false;
    }

    bool sorted = sortBuffer(buffer, super->_private.size, super->_private.type.elementSize, compare, tmp);
    bool copied = sorted && adt_ForEachMutable(adt, CopyFromBuffer, &context);

    free(tmp);
    free(buffer);
    return copied;
}

bool adt_Sort(ADT_t *adt, ADT_SortAlgorithm_t algorithm)
{
    ADT_Super_t *super = (ADT_Super_t *)adt;

    if (super == NULL)
    {
        return false;
    }

    return adt_SortBy(adt, algorithm, super->_private.type.compare);
}
