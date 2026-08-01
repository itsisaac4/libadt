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
/* --- LL_INIT / LINKED_LIST_INIT ------------------------------------------ */

/**
 * @brief Initializes an empty list of primitive elements and automatically constructs its ADT_ElementTypeInfo_t.
 * @param list List to initialize.
 * @param type Element type.
 * @return true on success; otherwise false.
 */
#define LINKED_LIST_INIT LL_INIT
#define LL_INIT(list, type)    \
    ll_Init(                   \
        (list),                \
        ADT_ELEMENT_TYPE_INFO( \
            type,              \
            COMPARATOR(type),  \
            PRINTER(type),     \
            TO_NUMBER(type),   \
            NULL))

/* --- LL_INIT_FROM / LINKED_LIST_INIT_FROM -------------------------------- */

/**
 * @brief Initializes a list from primitive elements and automatically constructs its ADT_ElementTypeInfo_t.
 * @param list List to initialize.
 * @param values Values to copy.
 * @return true on success; otherwise false.
 */
#define LINKED_LIST_INIT_FROM LL_INIT_FROM
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

/* --- ll_Init / linkedList_Init ------------------------------------------- */

/**
 * @brief Initializes an empty list.
 * @param[out] list List to initialize.
 * @param elementType Runtime element information; elementSize must be nonzero.
 * @return true on success; otherwise false.
 */
#define linkedList_Init ll_Init
bool ll_Init(LinkedList_t *list, ADT_ElementTypeInfo_t elementType);

/* --- ll_InitFrom / linkedList_InitFrom ----------------------------------- */

/**
 * @brief Initializes a list with shallow copies of contiguous elements.
 * @param[out] list List to initialize.
 * @param elements Elements to copy, or NULL when initialCount is zero.
 * @param initialCount Number of elements.
 * @param elementType Runtime element information; elementSize must be nonzero.
 * @return true on success; otherwise false.
 */
#define linkedList_InitFrom ll_InitFrom
bool ll_InitFrom(LinkedList_t *list, const void *elements, size_t initialCount, ADT_ElementTypeInfo_t elementType);

/* --- ll_Get / linkedList_Get --------------------------------------------- */

/**
 * @brief Copies an element into caller storage without transferring ownership.
 * @param list Source list.
 * @param index Element index.
 * @param[out] outElement Destination storage.
 * @return true on success; otherwise false.
 */
#define linkedList_Get ll_Get
bool ll_Get(const LinkedList_t *list, size_t index, void *outElement);

#include "internal/linked_list_operations.h"

#ifndef __cplusplus
/* --- ll_IndexOf / linkedList_IndexOf ------------------------------------- */

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
#define linkedList_IndexOf ll_IndexOf
#define ll_IndexOf(list, element, outIndex)                    \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_IndexOf, (element))( \
        (list),                                                \
        (element),                                             \
        (outIndex))

/* --- ll_Contains / linkedList_Contains ----------------------------------- */

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
#define linkedList_Contains ll_Contains
#define ll_Contains(list, element)                              \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_Contains, (element))( \
        (list),                                                 \
        (element))

/* --- ll_Set / linkedList_Set --------------------------------------------- */

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
#define linkedList_Set ll_Set
#define ll_Set(list, index, element)                       \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_Set, (element))( \
        (list),                                            \
        (index),                                           \
        (element))

/* --- ll_Insert / linkedList_Insert --------------------------------------- */

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
#define linkedList_Insert ll_Insert
#define ll_Insert(list, index, element)                       \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_Insert, (element))( \
        (list),                                               \
        (index),                                              \
        (element))

/* --- ll_Prepend / linkedList_Prepend ------------------------------------- */

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
#define linkedList_Prepend ll_Prepend
#define ll_Prepend(list, element)                              \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_Prepend, (element))( \
        (list),                                                \
        (element))

/* --- ll_Append / linkedList_Append --------------------------------------- */

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
#define linkedList_Append ll_Append
#define ll_Append(list, element)                              \
    ADT_DETAIL_DISPATCH_ELEMENT(ll_detail_Append, (element))( \
        (list),                                               \
        (element))
#endif

/* --- ll_Remove / linkedList_Remove --------------------------------------- */

/**
 * @brief Removes an element and destroys its owned resources.
 * @param[in,out] list Target list.
 * @param index Element index.
 * @return true on success; otherwise false.
 */
#define linkedList_Remove ll_Remove
bool ll_Remove(LinkedList_t *list, size_t index);

/* --- ll_Take / linkedList_Take ------------------------------------------- */

/**
 * @brief Removes an element and transfers its resource ownership.
 * @param[in,out] list Target list.
 * @param index Element index.
 * @param[out] outElement Removed element.
 * @return true on success; otherwise false.
 */
#define linkedList_Take ll_Take
bool ll_Take(LinkedList_t *list, size_t index, void *outElement);

/* --- ll_Clear / linkedList_Clear ----------------------------------------- */

/**
 * @brief Removes all elements while retaining type information.
 * @param[in,out] list List to clear, or NULL.
 * @return Nothing.
 */
#define linkedList_Clear ll_Clear
void ll_Clear(LinkedList_t *list);

/* --- ll_Destroy / linkedList_Destroy ------------------------------------- */

/**
 * @brief Releases all elements and resets the list.
 * @param[in,out] list List to destroy, or NULL.
 * @return Nothing.
 */
#define linkedList_Destroy ll_Destroy
void ll_Destroy(LinkedList_t *list);

#ifdef __cplusplus
}
#endif

#endif
