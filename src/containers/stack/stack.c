#include "libadt/stack.h"

#include <string.h>

static void *ElementAt(Stack_t *stack, size_t index)
{
    return contiguousStorage_At(
        &stack->_private.storage,
        index,
        stack->super._private.elementType.elementSize);
}

static const void *ElementAtConst(const Stack_t *stack, size_t index)
{
    return contiguousStorage_AtConst(
        &stack->_private.storage,
        index,
        stack->super._private.elementType.elementSize);
}

static void VisitElements(const ADT_Super_t *adt, ADT_ConstVisitFn_t visitor, void *context)
{
    if (adt == NULL || visitor == NULL)
    {
        return;
    }

    const Stack_t *stack = (const Stack_t *)adt;

    for (size_t i = 0; i < adt->_private.size; i++)
    {
        visitor(ElementAtConst(stack, i), i, context);
    }
}

static void VisitMutableElements(ADT_Super_t *adt, ADT_MutableVisitFn_t visitor, void *context)
{
    if (adt == NULL || visitor == NULL)
    {
        return;
    }

    Stack_t *stack = (Stack_t *)adt;

    for (size_t i = 0; i < adt->_private.size; i++)
    {
        visitor(ElementAt(stack, i), i, context);
    }
}

static const ADT_VTable_t STACK_VTABLE = {
    .containerName = "Stack",
    .visit = VisitElements,
    .visitMutable = VisitMutableElements};

static void DestroyElement(Stack_t *stack, size_t index)
{
    if (stack->super._private.elementType.destroy != NULL)
    {
        stack->super._private.elementType.destroy(ElementAt(stack, index));
    }
}

bool st_Init(Stack_t *stack, ADT_ElementTypeInfo_t elementType)
{
    if (stack == NULL || elementType.elementSize == 0)
    {
        return false;
    }

    ContiguousStorage_t storage;
    if (!contiguousStorage_Init(&storage, elementType.elementSize))
    {
        return false;
    }

    stack->super = (ADT_Super_t){
        ._private = {
            .vtable = &STACK_VTABLE,
            .size = 0,
            .elementType = elementType}};
    stack->_private.storage = storage;
    return true;
}

bool st_InitFrom(Stack_t *stack, const void *elements, size_t initialCount, ADT_ElementTypeInfo_t elementType)
{
    if (stack == NULL ||
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

    stack->super = (ADT_Super_t){
        ._private = {
            .vtable = &STACK_VTABLE,
            .size = initialCount,
            .elementType = elementType}};
    stack->_private.storage = storage;
    return true;
}

bool st_Peek(const Stack_t *stack, void *outElement)
{
    if (stack == NULL || outElement == NULL || stack->super._private.size == 0)
    {
        return false;
    }

    memmove(
        outElement,
        ElementAtConst(stack, stack->super._private.size - 1),
        stack->super._private.elementType.elementSize);
    return true;
}

bool st_detail_PushRef(Stack_t *stack, const void *element)
{
    if (stack == NULL || element == NULL)
    {
        return false;
    }

    if (!contiguousStorage_Insert(
            &stack->_private.storage,
            stack->super._private.size,
            stack->super._private.size,
            element,
            stack->super._private.elementType.elementSize))
    {
        return false;
    }

    stack->super._private.size++;
    return true;
}

bool st_Pop(Stack_t *stack, void *outElement)
{
    if (stack == NULL ||
        outElement == NULL ||
        stack->super._private.size == 0 ||
        contiguousStorage_ContainsAddress(
            &stack->_private.storage,
            outElement,
            stack->super._private.elementType.elementSize))
    {
        return false;
    }

    const size_t topIndex = stack->super._private.size - 1;
    memmove(
        outElement,
        ElementAt(stack, topIndex),
        stack->super._private.elementType.elementSize);

    if (!contiguousStorage_Erase(
            &stack->_private.storage,
            stack->super._private.size,
            topIndex,
            stack->super._private.elementType.elementSize))
    {
        return false;
    }

    stack->super._private.size--;
    return true;
}

bool st_BinarySearch(const Stack_t *stack, const void *target, size_t *outIndex)
{
    if (stack == NULL ||
        target == NULL ||
        outIndex == NULL ||
        stack->super._private.elementType.compare == NULL)
    {
        return false;
    }

    return contiguousStorage_BinarySearchBy(
        &stack->_private.storage,
        stack->super._private.size,
        stack->super._private.elementType.elementSize,
        stack->super._private.elementType.compare,
        target,
        outIndex);
}

bool st_BinarySearchBy(const Stack_t *stack, CompareFn_t compare, const void *target, size_t *outIndex)
{
    if (stack == NULL ||
        compare == NULL ||
        target == NULL ||
        outIndex == NULL)
    {
        return false;
    }

    return contiguousStorage_BinarySearchBy(
        &stack->_private.storage,
        stack->super._private.size,
        stack->super._private.elementType.elementSize,
        compare,
        target,
        outIndex);
}

bool st_Discard(Stack_t *stack)
{
    if (stack == NULL || stack->super._private.size == 0)
    {
        return false;
    }

    const size_t topIndex = stack->super._private.size - 1;
    DestroyElement(stack, topIndex);

    if (!contiguousStorage_Erase(
            &stack->_private.storage,
            stack->super._private.size,
            topIndex,
            stack->super._private.elementType.elementSize))
    {
        return false;
    }

    stack->super._private.size--;
    return true;
}

void st_Clear(Stack_t *stack)
{
    if (stack == NULL)
    {
        return;
    }

    for (size_t i = 0; i < stack->super._private.size; i++)
    {
        DestroyElement(stack, i);
    }

    stack->super._private.size = 0;
}

void st_Destroy(Stack_t *stack)
{
    if (stack == NULL)
    {
        return;
    }

    st_Clear(stack);
    contiguousStorage_Destroy(&stack->_private.storage);
    stack->super = (ADT_Super_t){0};
}
