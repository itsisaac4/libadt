#ifndef LIBADT_LINKED_LIST_H
#define LIBADT_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "abstract_data_type.h"
#include "internal/primitive_dispatch.h"
#include "internal/storage/linked_storage.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Stores elements in a doubly linked sequence.
     *
     * Members:
     * - `super`: Shared ADT state; must remain first.
     * - `_private`: Internal list state; do not modify.
     */
    typedef struct
    {
        ADT_Super_t super;

        struct
        {
            LinkedStorage_t storage;
        } _private;
    } LinkedList_t;

#ifdef __cplusplus
    static_assert(offsetof(LinkedList_t, super) == 0, "LinkedList_t.super must be first");
#else
_Static_assert(offsetof(LinkedList_t, super) == 0, "LinkedList_t.super must be first");
#endif

#ifndef __cplusplus
/**
 * @brief Initializes an empty list for a C type.
 * @param list List to initialize.
 * @param type Element type.
 * @return true on success; otherwise false.
 */
#define LL_INIT(list, type)    \
    ll_Init(                   \
        (list),                \
        ADT_ELEMENT_TYPE_INFO( \
            type,              \
            COMPARATOR(type),  \
            PRINTER(type),     \
            TO_NUMBER(type),   \
            NULL))

/**
 * @brief Initializes a list from a fixed-size C array.
 * @param list List to initialize.
 * @param values Values to copy.
 * @return true on success; otherwise false.
 */
#define LL_INIT_FROM(list, values)                  \
    ll_InitFrom(                                    \
        (list),                                     \
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
     * @brief Initializes an empty list.
     * @param[out] list List to initialize.
     * @param elementType Runtime element information; elementSize must be nonzero.
     * @return true on success; otherwise false.
     */
    bool ll_Init(LinkedList_t *list, ADT_ElementTypeInfo_t elementType);

    /**
     * @brief Initializes a list with shallow copies of contiguous elements.
     * @param[out] list List to initialize.
     * @param elements Elements to copy, or NULL when initialCount is zero.
     * @param initialCount Number of elements.
     * @param elementType Runtime element information; elementSize must be nonzero.
     * @return true on success; otherwise false.
     */
    bool ll_InitFrom(LinkedList_t *list, const void *elements, size_t initialCount, ADT_ElementTypeInfo_t elementType);

    /**
     * @brief Copies an element into caller storage without transferring ownership.
     * @param list Source list.
     * @param index Element index.
     * @param[out] outElement Destination storage.
     * @return true on success; otherwise false.
     */
    bool ll_Get(const LinkedList_t *list, size_t index, void *outElement);

#include "internal/linked_list_operations.h"

#ifndef __cplusplus
/**
 * @brief Finds the first equal element using type-based dispatch.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param list List to search.
 * @param element Value or address matching the initialized element type.
 * @param[out] outIndex Matching index.
 * @return true when found; otherwise false.
 */
#define ll_IndexOf(list, element, outIndex)                    \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_IndexOf, (element))( \
        (list),                                                \
        (element),                                             \
        (outIndex))

/**
 * @brief Checks for an equal element using type-based dispatch.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param list List to search.
 * @param element Value or address matching the initialized element type.
 * @return true when found; otherwise false.
 */
#define ll_Contains(list, element)                              \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_Contains, (element))( \
        (list),                                                 \
        (element))

/**
 * @brief Replaces an element with a shallow copy.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param list Target list.
 * @param index Element index.
 * @param element Value or address matching the initialized element type.
 * @return true on success; otherwise false.
 */
#define ll_Set(list, index, element)                       \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_Set, (element))( \
        (list),                                            \
        (index),                                           \
        (element))

/**
 * @brief Inserts a shallow element copy at an index.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param list Target list.
 * @param index Insertion index from zero through size.
 * @param element Value or address matching the initialized element type.
 * @return true on success; otherwise false.
 */
#define ll_Insert(list, index, element)                       \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_Insert, (element))( \
        (list),                                               \
        (index),                                              \
        (element))

/**
 * @brief Prepends a shallow element copy.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param list Target list.
 * @param element Value or address matching the initialized element type.
 * @return true on success; otherwise false.
 */
#define ll_Prepend(list, element)                              \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_Prepend, (element))( \
        (list),                                                \
        (element))

/**
 * @brief Appends a shallow element copy.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param list Target list.
 * @param element Value or address matching the initialized element type.
 * @return true on success; otherwise false.
 */
#define ll_Append(list, element)                              \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_Append, (element))( \
        (list),                                               \
        (element))
#endif

    /**
     * @brief Removes an element and destroys its owned resources.
     * @param[in,out] list Target list.
     * @param index Element index.
     * @return true on success; otherwise false.
     */
    bool ll_Remove(LinkedList_t *list, size_t index);

    /**
     * @brief Removes an element and transfers its resource ownership.
     * @param[in,out] list Target list.
     * @param index Element index.
     * @param[out] outElement Removed element.
     * @return true on success; otherwise false.
     */
    bool ll_Take(LinkedList_t *list, size_t index, void *outElement);

    /**
     * @brief Removes all elements while retaining type information.
     * @param[in,out] list List to clear, or NULL.
     * @return Nothing.
     */
    void ll_Clear(LinkedList_t *list);

    /**
     * @brief Releases all elements and resets the list.
     * @param[in,out] list List to destroy, or NULL.
     * @return Nothing.
     */
    void ll_Destroy(LinkedList_t *list);

#ifdef __cplusplus
}
#endif

#endif
