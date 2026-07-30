#include "libadt/linked_list.h"
#include "libadt/primitive_dispatch.h"

#define DEFINE_LINKED_LIST_PRIMITIVE(Suffix, Type)                                        \
    bool ll_detail_IndexOf##Suffix(const LinkedList_t *list, Type element, size_t *outIndex) \
    {                                                                                     \
        return adt_AcceptsElementSize(list, sizeof(Type)) &&                              \
               ll_detail_IndexOfRef(list, &element, outIndex);                            \
    }                                                                                     \
                                                                                          \
    bool ll_detail_Contains##Suffix(const LinkedList_t *list, Type element)                \
    {                                                                                     \
        return adt_AcceptsElementSize(list, sizeof(Type)) &&                              \
               ll_detail_ContainsRef(list, &element);                                     \
    }                                                                                     \
                                                                                          \
    bool ll_detail_Set##Suffix(LinkedList_t *list, size_t index, Type element)             \
    {                                                                                     \
        return adt_AcceptsElementSize(list, sizeof(Type)) &&                              \
               ll_detail_SetRef(list, index, &element);                                   \
    }                                                                                     \
                                                                                          \
    bool ll_detail_Insert##Suffix(LinkedList_t *list, size_t index, Type element)          \
    {                                                                                     \
        return adt_AcceptsElementSize(list, sizeof(Type)) &&                              \
               ll_detail_InsertRef(list, index, &element);                                \
    }                                                                                     \
                                                                                          \
    bool ll_detail_Prepend##Suffix(LinkedList_t *list, Type element)                       \
    {                                                                                     \
        return adt_AcceptsElementSize(list, sizeof(Type)) &&                              \
               ll_detail_PrependRef(list, &element);                                      \
    }                                                                                     \
                                                                                          \
    bool ll_detail_Append##Suffix(LinkedList_t *list, Type element)                        \
    {                                                                                     \
        return adt_AcceptsElementSize(list, sizeof(Type)) &&                              \
               ll_detail_AppendRef(list, &element);                                       \
    }
ADT_FOR_EACH_PRIMITIVE(DEFINE_LINKED_LIST_PRIMITIVE)
#undef DEFINE_LINKED_LIST_PRIMITIVE
