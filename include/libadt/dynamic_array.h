#ifndef LIBADT_DYNAMIC_ARRAY_H
#define LIBADT_DYNAMIC_ARRAY_H

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
 * @brief Stores elements in resizable contiguous storage.
 *
 * Members:
 * - `super`: Shared ADT state; must remain first.
 * - `_private`: Internal array state; do not modify.
 */
typedef struct
{
    ADT_Super_t super;

    struct
    {
        ContiguousStorage_t storage;
    } _private;
} DynamicArray_t;

#ifdef __cplusplus
static_assert(offsetof(DynamicArray_t, super) == 0, "DynamicArray_t.super must be first");
#else
_Static_assert(offsetof(DynamicArray_t, super) == 0, "DynamicArray_t.super must be first");
#endif

#ifndef __cplusplus
/* --- DA_INIT / DYNAMIC_ARRAY_INIT ---------------------------------------- */

/**
 * @brief Initializes an empty array of primitive elements and automatically constructs its ADT_ElementTypeInfo_t.
 * @param array Array to initialize.
 * @param type Element type.
 * @return true on success; otherwise false.
 */
#define DYNAMIC_ARRAY_INIT DA_INIT
#define DA_INIT(array, type)   \
    da_Init(                   \
        (array),               \
        ADT_ELEMENT_TYPE_INFO( \
            type,              \
            COMPARATOR(type),  \
            PRINTER(type),     \
            TO_NUMBER(type),   \
            NULL))

/* --- DA_INIT_FROM / DYNAMIC_ARRAY_INIT_FROM ------------------------------ */

/**
 * @brief Initializes an array from primitive elements and automatically constructs its ADT_ElementTypeInfo_t.
 * @param array Array to initialize.
 * @param values Values to copy.
 * @return true on success; otherwise false.
 */
#define DYNAMIC_ARRAY_INIT_FROM DA_INIT_FROM
#define DA_INIT_FROM(array, values)                 \
    da_InitFrom(                                    \
        (array),                                    \
        (values),                                   \
        ARRAY_COUNT(values),                        \
        ADT_ELEMENT_TYPE_INFO(                      \
            typeof_unqual((values)[0]),             \
            COMPARATOR(typeof_unqual((values)[0])), \
            PRINTER(typeof_unqual((values)[0])),    \
            TO_NUMBER(typeof_unqual((values)[0])),  \
            NULL))
#endif

/* --- da_Init / dynamicArray_Init ----------------------------------------- */

/**
 * @brief Initializes an empty array.
 * @param[out] array Array to initialize.
 * @param elementType Runtime element information; elementSize must be nonzero.
 * @return true on success; otherwise false.
 */
#define dynamicArray_Init da_Init
bool da_Init(DynamicArray_t *array, ADT_ElementTypeInfo_t elementType);

/* --- da_InitFrom / dynamicArray_InitFrom --------------------------------- */

/**
 * @brief Initializes an array with shallow copies of contiguous elements.
 * @param[out] array Array to initialize.
 * @param elements Elements to copy, or NULL when initialCount is zero.
 * @param initialCount Number of elements.
 * @param elementType Runtime element information; elementSize must be nonzero.
 * @return true on success; otherwise false.
 */
#define dynamicArray_InitFrom da_InitFrom
bool da_InitFrom(DynamicArray_t *array, const void *elements, size_t initialCount, ADT_ElementTypeInfo_t elementType);

/* --- da_Get / dynamicArray_Get ------------------------------------------- */

/**
 * @brief Copies an element into caller storage without transferring ownership.
 * @param array Source array.
 * @param index Element index.
 * @param[out] outElement Destination storage.
 * @return true on success; otherwise false.
 */
#define dynamicArray_Get da_Get
bool da_Get(const DynamicArray_t *array, size_t index, void *outElement);

#include "internal/dynamic_array_operations.h"

#ifndef __cplusplus
/* --- da_IndexOf / dynamicArray_IndexOf ----------------------------------- */

/**
 * @brief Finds the first equal element using type-based dispatch.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param array Array to search.
 * @param element Value or address matching the initialized element type.
 * @param[out] outIndex Matching index.
 * @return true when found; otherwise false.
 */
#define dynamicArray_IndexOf da_IndexOf
#define da_IndexOf(array, element, outIndex)                   \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_IndexOf, (element))( \
        (array),                                               \
        (element),                                             \
        (outIndex))

/* --- da_Contains / dynamicArray_Contains --------------------------------- */

/**
 * @brief Checks for an equal element using type-based dispatch.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param array Array to search.
 * @param element Value or address matching the initialized element type.
 * @return true when found; otherwise false.
 */
#define dynamicArray_Contains da_Contains
#define da_Contains(array, element)                             \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_Contains, (element))( \
        (array),                                                \
        (element))

/* --- da_Set / dynamicArray_Set ------------------------------------------- */

/**
 * @brief Replaces an element with a shallow copy.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param array Target array.
 * @param index Element index.
 * @param element Value or address matching the initialized element type.
 * @return true on success; otherwise false.
 */
#define dynamicArray_Set da_Set
#define da_Set(array, index, element)                      \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_Set, (element))( \
        (array),                                           \
        (index),                                           \
        (element))

/* --- da_Insert / dynamicArray_Insert ------------------------------------- */

/**
 * @brief Inserts a shallow element copy at an index.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param array Target array.
 * @param index Insertion index from zero through size.
 * @param element Value or address matching the initialized element type.
 * @return true on success; otherwise false.
 */
#define dynamicArray_Insert da_Insert
#define da_Insert(array, index, element)                      \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_Insert, (element))( \
        (array),                                              \
        (index),                                              \
        (element))

/* --- da_Prepend / dynamicArray_Prepend ----------------------------------- */

/**
 * @brief Prepends a shallow element copy.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param array Target array.
 * @param element Value or address matching the initialized element type.
 * @return true on success; otherwise false.
 */
#define dynamicArray_Prepend da_Prepend
#define da_Prepend(array, element)                             \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_Prepend, (element))( \
        (array),                                               \
        (element))

/* --- da_Append / dynamicArray_Append ------------------------------------- */

/**
 * @brief Appends a shallow element copy.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param array Target array.
 * @param element Value or address matching the initialized element type.
 * @return true on success; otherwise false.
 */
#define dynamicArray_Append da_Append
#define da_Append(array, element)                             \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_Append, (element))( \
        (array),                                              \
        (element))
#endif

/* --- da_Remove / dynamicArray_Remove ------------------------------------- */

/**
 * @brief Removes an element and destroys its owned resources.
 * @param[in,out] array Target array.
 * @param index Element index.
 * @return true on success; otherwise false.
 */
#define dynamicArray_Remove da_Remove
bool da_Remove(DynamicArray_t *array, size_t index);

/* --- da_Take / dynamicArray_Take ----------------------------------------- */

/**
 * @brief Removes an element and transfers its resource ownership.
 * @param[in,out] array Target array.
 * @param index Element index.
 * @param[out] outElement Removed element.
 * @return true on success; otherwise false.
 */
#define dynamicArray_Take da_Take
bool da_Take(DynamicArray_t *array, size_t index, void *outElement);

/* --- da_BinarySearch / dynamicArray_BinarySearch ------------------------- */

/**
 * @brief Finds the first matching element in a sorted array.
 *
 * The array must already be ordered by its configured comparator.
 * Use adt_isSorted when the current ordering is uncertain.
 *
 * @param array Sorted array to search.
 * @param target Element value to find.
 * @param[out] outIndex Index of the first matching element when found.
 * @return true when a match is found; otherwise false.
 */
#define dynamicArray_BinarySearch da_BinarySearch
bool da_BinarySearch(const DynamicArray_t *array, const void *target, size_t *outIndex);

/* --- da_BinarySearchBy / dynamicArray_BinarySearchBy --------------------- */

/**
 * @brief Finds the first matching element in an array sorted by a comparator.
 *
 * The array must already be ordered by @p compare.
 * Use adt_isSortedBy with the same comparator when the ordering is uncertain.
 *
 * @param array Sorted array to search.
 * @param compare Comparator defining the array order.
 * @param target Element value to find.
 * @param[out] outIndex Index of the first matching element when found.
 * @return true when a match is found; otherwise false.
 */
#define dynamicArray_BinarySearchBy da_BinarySearchBy
bool da_BinarySearchBy(const DynamicArray_t *array, CompareFn_t compare, const void *target, size_t *outIndex);

/* --- da_Clear / dynamicArray_Clear --------------------------------------- */

/**
 * @brief Removes all elements while retaining storage and type information.
 * @param[in,out] array Array to clear, or NULL.
 * @return Nothing.
 */
#define dynamicArray_Clear da_Clear
void da_Clear(DynamicArray_t *array);

/* --- da_Destroy / dynamicArray_Destroy ----------------------------------- */

/**
 * @brief Releases all elements and resets the array.
 * @param[in,out] array Array to destroy, or NULL.
 * @return Nothing.
 */
#define dynamicArray_Destroy da_Destroy
void da_Destroy(DynamicArray_t *array);

#ifdef __cplusplus
}
#endif

#endif
