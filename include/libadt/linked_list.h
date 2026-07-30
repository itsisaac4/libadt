#ifndef LIBADT_LINKED_LIST_H
#define LIBADT_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "abstract_data_type.h"
#include "primitive_dispatch.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Stores one element and its neighboring links.
     *
     * Members:
     * - `data`: Container-owned element storage.
     * - `previous`: Previous node, or NULL.
     * - `next`: Next node, or NULL.
     */
    typedef struct LinkedListNode
    {
        void *data;
        struct LinkedListNode *previous;
        struct LinkedListNode *next;
    } LinkedListNode_t;

    /**
     * @brief Stores elements in a doubly linked sequence.
     *
     * Members:
     * - `super`: Shared ADT state; must remain first.
     * - `head`: First node, or NULL.
     * - `tail`: Last node, or NULL.
     */
    typedef struct
    {
        ADT_Super_t super;
        LinkedListNode_t *head;
        LinkedListNode_t *tail;
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
#define LL_INIT(list, type)             \
    ll_Init(                            \
        (list),                         \
        (ADT_TypeInfo_t){               \
            .elementSize = sizeof(type), \
            .compare = COMPARATOR(type), \
            .print = PRINTER(type),     \
            .destroy = NULL})

/**
 * @brief Initializes a list from a fixed-size C array.
 * @param list List to initialize.
 * @param values Values to copy.
 * @return true on success; otherwise false.
 */
#define LL_INIT_FROM(list, values)                            \
    ll_InitFrom(                                              \
        (list),                                               \
        (values),                                             \
        ARRAY_COUNT(values),                                  \
        (ADT_TypeInfo_t){                                     \
            .elementSize = sizeof((values)[0]),               \
            .compare = COMPARATOR(typeof_unqual((values)[0])), \
            .print = PRINTER(typeof_unqual((values)[0])),     \
            .destroy = NULL})
#endif

    /**
     * @brief Initializes an empty list.
     * @param[out] list List to initialize.
     * @param typeInfo Runtime element type; elementSize must be nonzero.
     * @return true on success; otherwise false.
     */
    bool ll_Init(LinkedList_t *list, ADT_TypeInfo_t typeInfo);

    /**
     * @brief Initializes a list with shallow copies of contiguous elements.
     * @param[out] list List to initialize.
     * @param elements Elements to copy, or NULL when initialCount is zero.
     * @param initialCount Number of elements.
     * @param typeInfo Runtime element type; elementSize must be nonzero.
     * @return true on success; otherwise false.
     */
    bool ll_InitFrom(LinkedList_t *list, const void *elements, size_t initialCount, ADT_TypeInfo_t typeInfo);

    /**
     * @brief Copies an element into caller storage without transferring ownership.
     * @param list Source list.
     * @param index Element index.
     * @param[out] outElement Destination storage.
     * @return true on success; otherwise false.
     */
    bool ll_Get(const LinkedList_t *list, size_t index, void *outElement);

    /**
     * @brief Replaces an element with a shallow copy.
     *
     * Use ll_SetValue for supported primitives. Use ll_SetRef with the address
     * of element storage for custom or pointer element types.
     *
     * @param[in,out] list Target list.
     * @param index Element index.
     * @param element Address of the replacement element.
     * @return true on success; otherwise false.
     */
    bool ll_SetRef(LinkedList_t *list, size_t index, const void *element);

    /**
     * @brief Finds the first equal element.
     *
     * Use ll_IndexOfValue for supported primitives. Use ll_IndexOfRef with the
     * address of element storage for custom or pointer element types.
     *
     * @param list List to search.
     * @param element Address of the element to find.
     * @param[out] outIndex Matching index.
     * @return true when found; otherwise false.
     */
    bool ll_IndexOfRef(const LinkedList_t *list, const void *element, size_t *outIndex);

    /**
     * @brief Checks whether an equal element exists.
     *
     * Use ll_ContainsValue for supported primitives. Use ll_ContainsRef with
     * the address of element storage for custom or pointer element types.
     *
     * @param list List to search.
     * @param element Address of the element to find.
     * @return true when found; otherwise false.
     */
    bool ll_ContainsRef(const LinkedList_t *list, const void *element);

    /**
     * @brief Inserts a shallow element copy at an index.
     *
     * Use ll_InsertValue for supported primitives. Use ll_InsertRef with the
     * address of element storage for custom or pointer element types.
     *
     * @param[in,out] list Target list.
     * @param index Insertion index from zero through size.
     * @param element Address of the element to insert.
     * @return true on success; otherwise false.
     */
    bool ll_InsertRef(LinkedList_t *list, size_t index, const void *element);

    /**
     * @brief Prepends a shallow element copy.
     *
     * Use ll_PrependValue for supported primitives. Use ll_PrependRef with the
     * address of element storage for custom or pointer element types.
     *
     * @param[in,out] list Target list.
     * @param element Address of the element to prepend.
     * @return true on success; otherwise false.
     */
    bool ll_PrependRef(LinkedList_t *list, const void *element);

    /**
     * @brief Appends a shallow element copy.
     *
     * Use ll_AppendValue for supported primitives. Use ll_AppendRef with the
     * address of element storage for custom or pointer element types.
     *
     * @param[in,out] list Target list.
     * @param element Address of the element to append.
     * @return true on success; otherwise false.
     */
    bool ll_AppendRef(LinkedList_t *list, const void *element);

    /**
     * @brief Declares pass-by-value variants for one primitive type.
     * @param Suffix Function-name suffix.
     * @param Type Primitive parameter type.
     * @return The generated declarations.
     */
#define ADT_PRIMITIVE(Suffix, Type)                                                    \
    bool ll_IndexOf##Suffix(const LinkedList_t *list, Type element, size_t *outIndex); \
    bool ll_Contains##Suffix(const LinkedList_t *list, Type element);                  \
    bool ll_Set##Suffix(LinkedList_t *list, size_t index, Type element);               \
    bool ll_Insert##Suffix(LinkedList_t *list, size_t index, Type element);            \
    bool ll_Prepend##Suffix(LinkedList_t *list, Type element);                         \
    bool ll_Append##Suffix(LinkedList_t *list, Type element);
    ADT_FOR_EACH_PRIMITIVE(ADT_PRIMITIVE)
#undef ADT_PRIMITIVE

#ifndef __cplusplus
/**
 * @brief Finds a primitive value using type-based dispatch.
 * @param list List to search.
 * @param element Primitive value.
 * @param[out] outIndex Matching index.
 * @return true when found; otherwise false.
 */
#define ll_IndexOfValue(list, element, outIndex)          \
    ADT_SELECT_PRIMITIVE_FUNCTION(ll_IndexOf, (element))( \
        (list),                                           \
        (element),                                        \
        (outIndex))

/**
 * @brief Checks for a primitive value using type-based dispatch.
 * @param list List to search.
 * @param element Primitive value.
 * @return true when found; otherwise false.
 */
#define ll_ContainsValue(list, element)                    \
    ADT_SELECT_PRIMITIVE_FUNCTION(ll_Contains, (element))( \
        (list),                                            \
        (element))

/**
 * @brief Replaces an element with a primitive value.
 * @param list Target list.
 * @param index Element index.
 * @param element Primitive replacement value.
 * @return true on success; otherwise false.
 */
#define ll_SetValue(list, index, element)             \
    ADT_SELECT_PRIMITIVE_FUNCTION(ll_Set, (element))( \
        (list),                                       \
        (index),                                      \
        (element))

/**
 * @brief Inserts a primitive value at an index.
 * @param list Target list.
 * @param index Insertion index.
 * @param element Primitive value.
 * @return true on success; otherwise false.
 */
#define ll_InsertValue(list, index, element)             \
    ADT_SELECT_PRIMITIVE_FUNCTION(ll_Insert, (element))( \
        (list),                                          \
        (index),                                         \
        (element))

/**
 * @brief Prepends a primitive value.
 * @param list Target list.
 * @param element Primitive value.
 * @return true on success; otherwise false.
 */
#define ll_PrependValue(list, element)                    \
    ADT_SELECT_PRIMITIVE_FUNCTION(ll_Prepend, (element))( \
        (list),                                           \
        (element))

/**
 * @brief Appends a primitive value.
 * @param list Target list.
 * @param element Primitive value.
 * @return true on success; otherwise false.
 */
#define ll_AppendValue(list, element)                    \
    ADT_SELECT_PRIMITIVE_FUNCTION(ll_Append, (element))( \
        (list),                                          \
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
