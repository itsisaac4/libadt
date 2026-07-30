#ifndef LIBADT_DYNAMIC_ARRAY_H
#define LIBADT_DYNAMIC_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

#include "abstract_data_type.h"
#include "primitive_dispatch.h"

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
            void *data;
            size_t capacity;
        } _private;
    } DynamicArray_t;

#ifdef __cplusplus
    static_assert(offsetof(DynamicArray_t, super) == 0, "DynamicArray_t.super must be first");
#else
_Static_assert(offsetof(DynamicArray_t, super) == 0, "DynamicArray_t.super must be first");
#endif

#ifndef __cplusplus
/**
 * @brief Initializes an empty array for a C type.
 * @param array Array to initialize.
 * @param type Element type.
 * @return true on success; otherwise false.
 */
#define DA_INIT(array, type)             \
    da_Init(                             \
        (array),                         \
        (ADT_TypeInfo_t){                \
            .elementSize = sizeof(type), \
            .compare = COMPARATOR(type), \
            .print = PRINTER(type),      \
            .destroy = NULL})

/**
 * @brief Initializes an array from a fixed-size C array.
 * @param array Array to initialize.
 * @param values Values to copy.
 * @return true on success; otherwise false.
 */
#define DA_INIT_FROM(array, values)                            \
    da_InitFrom(                                               \
        (array),                                               \
        (values),                                              \
        ARRAY_COUNT(values),                                   \
        (ADT_TypeInfo_t){                                      \
            .elementSize = sizeof((values)[0]),                \
            .compare = COMPARATOR(typeof_unqual((values)[0])), \
            .print = PRINTER(typeof_unqual((values)[0])),      \
            .destroy = NULL})
#endif

    /**
     * @brief Initializes an empty array.
     * @param[out] array Array to initialize.
     * @param typeInfo Runtime element type; elementSize must be nonzero.
     * @return true on success; otherwise false.
     */
    bool da_Init(DynamicArray_t *array, ADT_TypeInfo_t typeInfo);

    /**
     * @brief Initializes an array with shallow copies of contiguous elements.
     * @param[out] array Array to initialize.
     * @param elements Elements to copy, or NULL when initialCount is zero.
     * @param initialCount Number of elements.
     * @param typeInfo Runtime element type; elementSize must be nonzero.
     * @return true on success; otherwise false.
     */
    bool da_InitFrom(DynamicArray_t *array, const void *elements, size_t initialCount, ADT_TypeInfo_t typeInfo);

    /**
     * @brief Copies an element into caller storage without transferring ownership.
     * @param array Source array.
     * @param index Element index.
     * @param[out] outElement Destination storage.
     * @return true on success; otherwise false.
     */
    bool da_Get(const DynamicArray_t *array, size_t index, void *outElement);

#include "detail/dynamic_array_operations.h"

#ifndef __cplusplus
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
#define da_IndexOf(array, element, outIndex)               \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_IndexOf, (element))( \
        (array),                                           \
        (element),                                         \
        (outIndex))

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
#define da_Contains(array, element)                         \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_Contains, (element))( \
        (array),                                            \
        (element))

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
#define da_Set(array, index, element)                     \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_Set, (element))( \
        (array),                                          \
        (index),                                          \
        (element))

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
#define da_Insert(array, index, element)                     \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_Insert, (element))( \
        (array),                                             \
        (index),                                             \
        (element))

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
#define da_Prepend(array, element)                         \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_Prepend, (element))( \
        (array),                                            \
        (element))

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
#define da_Append(array, element)                         \
    ADT_DETAIL_DISPATCH_ELEMENT(da_detail_Append, (element))( \
        (array),                                           \
        (element))
#endif

    /**
     * @brief Removes an element and destroys its owned resources.
     * @param[in,out] array Target array.
     * @param index Element index.
     * @return true on success; otherwise false.
     */
    bool da_Remove(DynamicArray_t *array, size_t index);

    /**
     * @brief Removes an element and transfers its resource ownership.
     * @param[in,out] array Target array.
     * @param index Element index.
     * @param[out] outElement Removed element.
     * @return true on success; otherwise false.
     */
    bool da_Take(DynamicArray_t *array, size_t index, void *outElement);

    /**
     * @brief Removes all elements while retaining storage and type information.
     * @param[in,out] array Array to clear, or NULL.
     * @return Nothing.
     */
    void da_Clear(DynamicArray_t *array);

    /**
     * @brief Releases all elements and resets the array.
     * @param[in,out] array Array to destroy, or NULL.
     * @return Nothing.
     */
    void da_Destroy(DynamicArray_t *array);

#ifdef __cplusplus
}
#endif

#endif
