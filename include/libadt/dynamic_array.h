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
     * - `data`: Container-owned element storage.
     * - `capacity`: Number of allocated element slots.
     */
    typedef struct
    {
        ADT_Super_t super;
        void *data;
        size_t capacity;
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

    /**
     * @brief Replaces an element with a shallow copy.
     *
     * Use da_SetValue for supported primitives. Use da_SetRef with the
     * address of element storage for custom or pointer element types.
     *
     * @param[in,out] array Target array.
     * @param index Element index.
     * @param element Address of the replacement element.
     * @return true on success; otherwise false.
     */
    bool da_SetRef(DynamicArray_t *array, size_t index, const void *element);

    /**
     * @brief Finds the first equal element.
     *
     * Use da_IndexOfValue for supported primitives. Use da_IndexOfRef with
     * the address of element storage for custom or pointer element types.
     *
     * @param array Array to search.
     * @param element Address of the element to find.
     * @param[out] outIndex Matching index.
     * @return true when found; otherwise false.
     */
    bool da_IndexOfRef(const DynamicArray_t *array, const void *element, size_t *outIndex);

    /**
     * @brief Checks whether an equal element exists.
     *
     * Use da_ContainsValue for supported primitives. Use da_ContainsRef with
     * the address of element storage for custom or pointer element types.
     *
     * @param array Array to search.
     * @param element Address of the element to find.
     * @return true when found; otherwise false.
     */
    bool da_ContainsRef(const DynamicArray_t *array, const void *element);

    /**
     * @brief Inserts a shallow element copy at an index.
     *
     * Use da_InsertValue for supported primitives. Use da_InsertRef with the
     * address of element storage for custom or pointer element types.
     *
     * @param[in,out] array Target array.
     * @param index Insertion index from zero through size.
     * @param element Address of the element to insert.
     * @return true on success; otherwise false.
     */
    bool da_InsertRef(DynamicArray_t *array, size_t index, const void *element);

    /**
     * @brief Prepends a shallow element copy.
     *
     * Use da_PrependValue for supported primitives. Use da_PrependRef with
     * the address of element storage for custom or pointer element types.
     *
     * @param[in,out] array Target array.
     * @param element Address of the element to prepend.
     * @return true on success; otherwise false.
     */
    bool da_PrependRef(DynamicArray_t *array, const void *element);

    /**
     * @brief Appends a shallow element copy.
     *
     * Use da_AppendValue for supported primitives. Use da_AppendRef with the
     * address of element storage for custom or pointer element types.
     *
     * @param[in,out] array Target array.
     * @param element Address of the element to append.
     * @return true on success; otherwise false.
     */
    bool da_AppendRef(DynamicArray_t *array, const void *element);

    /**
     * @brief Declares pass-by-value variants for one primitive type.
     * @param Suffix Function-name suffix.
     * @param Type Primitive parameter type.
     * @return The generated declarations.
     */
#define ADT_PRIMITIVE(Suffix, Type)                                                       \
    bool da_IndexOf##Suffix(const DynamicArray_t *array, Type element, size_t *outIndex); \
    bool da_Contains##Suffix(const DynamicArray_t *array, Type element);                  \
    bool da_Set##Suffix(DynamicArray_t *array, size_t index, Type element);               \
    bool da_Insert##Suffix(DynamicArray_t *array, size_t index, Type element);            \
    bool da_Prepend##Suffix(DynamicArray_t *array, Type element);                         \
    bool da_Append##Suffix(DynamicArray_t *array, Type element);
    ADT_FOR_EACH_PRIMITIVE(ADT_PRIMITIVE)
#undef ADT_PRIMITIVE

#ifndef __cplusplus
/**
 * @brief Finds a primitive value using type-based dispatch.
 * @param array Array to search.
 * @param element Primitive value.
 * @param[out] outIndex Matching index.
 * @return true when found; otherwise false.
 */
#define da_IndexOfValue(array, element, outIndex)         \
    ADT_SELECT_PRIMITIVE_FUNCTION(da_IndexOf, (element))( \
        (array),                                          \
        (element),                                        \
        (outIndex))

/**
 * @brief Checks for a primitive value using type-based dispatch.
 * @param array Array to search.
 * @param element Primitive value.
 * @return true when found; otherwise false.
 */
#define da_ContainsValue(array, element)                   \
    ADT_SELECT_PRIMITIVE_FUNCTION(da_Contains, (element))( \
        (array),                                           \
        (element))

/**
 * @brief Replaces an element with a primitive value.
 * @param array Target array.
 * @param index Element index.
 * @param element Primitive replacement value.
 * @return true on success; otherwise false.
 */
#define da_SetValue(array, index, element)            \
    ADT_SELECT_PRIMITIVE_FUNCTION(da_Set, (element))( \
        (array),                                      \
        (index),                                      \
        (element))

/**
 * @brief Inserts a primitive value at an index.
 * @param array Target array.
 * @param index Insertion index.
 * @param element Primitive value.
 * @return true on success; otherwise false.
 */
#define da_InsertValue(array, index, element)            \
    ADT_SELECT_PRIMITIVE_FUNCTION(da_Insert, (element))( \
        (array),                                         \
        (index),                                         \
        (element))

/**
 * @brief Prepends a primitive value.
 * @param array Target array.
 * @param element Primitive value.
 * @return true on success; otherwise false.
 */
#define da_PrependValue(array, element)                   \
    ADT_SELECT_PRIMITIVE_FUNCTION(da_Prepend, (element))( \
        (array),                                          \
        (element))

/**
 * @brief Appends a primitive value.
 * @param array Target array.
 * @param element Primitive value.
 * @return true on success; otherwise false.
 */
#define da_AppendValue(array, element)                   \
    ADT_SELECT_PRIMITIVE_FUNCTION(da_Append, (element))( \
        (array),                                         \
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
