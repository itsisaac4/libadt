#ifndef LIBADT_STACK_H
#define LIBADT_STACK_H

#include <stdbool.h>
#include <stddef.h>

#include "abstract_data_type.h"
#include "internal/primitive_dispatch.h"
#include "internal/storage/contiguous_storage.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Stores elements in last-in, first-out order.
     *
     * Members:
     * - `super`: Shared ADT state; must remain first.
     * - `_private`: Internal stack state; do not modify.
     */
    typedef struct
    {
        ADT_Super_t super;

        struct
        {
            ContiguousStorage_t storage;
        } _private;
    } Stack_t;

#ifdef __cplusplus
    static_assert(offsetof(Stack_t, super) == 0, "Stack_t.super must be first");
#else
_Static_assert(offsetof(Stack_t, super) == 0, "Stack_t.super must be first");
#endif

#ifndef __cplusplus
/**
 * @brief Initializes an empty stack for a C type.
 * @param stack Stack to initialize.
 * @param type Element type.
 * @return true on success; otherwise false.
 */
#define ST_INIT(stack, type)   \
    st_Init(                   \
        (stack),               \
        ADT_ELEMENT_TYPE_INFO( \
            type,              \
            COMPARATOR(type),  \
            PRINTER(type),     \
            TO_NUMBER(type),   \
            NULL))

/**
 * @brief Initializes a stack from a fixed-size C array.
 * @param stack Stack to initialize.
 * @param values Values ordered from bottom to top.
 * @return true on success; otherwise false.
 */
#define ST_INIT_FROM(stack, values)                 \
    st_InitFrom(                                    \
        (stack),                                    \
        (values),                                   \
        ARRAY_COUNT(values),                        \
        ADT_ELEMENT_TYPE_INFO(                      \
            typeof_unqual((values)[0]),             \
            COMPARATOR(typeof_unqual((values)[0])), \
            PRINTER(typeof_unqual((values)[0])),    \
            TO_NUMBER(typeof_unqual((values)[0])),  \
            NULL))
#endif

    /**
     * @brief Initializes an empty stack.
     * @param[out] stack Stack to initialize.
     * @param elementType Runtime element information; elementSize must be nonzero.
     * @return true on success; otherwise false.
     */
    bool st_Init(Stack_t *stack, ADT_ElementTypeInfo_t elementType);

    /**
     * @brief Initializes a stack with shallow copies ordered bottom to top.
     * @param[out] stack Stack to initialize.
     * @param elements Contiguous elements, or NULL when initialCount is zero.
     * @param initialCount Number of elements.
     * @param elementType Runtime element information; elementSize must be nonzero.
     * @return true on success; otherwise false.
     */
    bool st_InitFrom(Stack_t *stack, const void *elements, size_t initialCount, ADT_ElementTypeInfo_t elementType);

    /**
     * @brief Copies the top element without removing it.
     * @param stack Source stack.
     * @param[out] outElement Destination storage.
     * @return true on success; otherwise false.
     */
    bool st_Peek(const Stack_t *stack, void *outElement);

#include "internal/stack_operations.h"

#ifndef __cplusplus
/**
 * @brief Pushes a shallow element copy onto the stack.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param stack Target stack.
 * @param element Value or address matching the initialized element type.
 * @return true on success; otherwise false.
 */
#define st_Push(stack, element)                             \
    ADT_DETAIL_DISPATCH_ELEMENT(st_detail_Push, (element))( \
        (stack),                                            \
        (element))
#endif

    /**
     * @brief Pops the top element and transfers its resource ownership.
     * @param[in,out] stack Target stack.
     * @param[out] outElement Removed element.
     * @return true on success; otherwise false.
     */
    bool st_Pop(Stack_t *stack, void *outElement);

    /**
     * @brief Removes the top element and destroys its owned resources.
     * @param[in,out] stack Target stack.
     * @return true on success; otherwise false.
     */
    bool st_Discard(Stack_t *stack);

    /**
     * @brief Removes all elements while retaining storage and type information.
     * @param[in,out] stack Stack to clear, or NULL.
     * @return Nothing.
     */
    void st_Clear(Stack_t *stack);

    /**
     * @brief Releases all elements and resets the stack.
     * @param[in,out] stack Stack to destroy, or NULL.
     * @return Nothing.
     */
    void st_Destroy(Stack_t *stack);

#ifdef __cplusplus
}
#endif

#endif
