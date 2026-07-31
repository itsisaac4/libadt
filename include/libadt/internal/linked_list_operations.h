#ifndef LIBADT_INTERNAL_LINKED_LIST_OPERATIONS_H
#define LIBADT_INTERNAL_LINKED_LIST_OPERATIONS_H

#ifndef LIBADT_LINKED_LIST_H
#error "Include libadt/linked_list.h instead."
#endif

/** @cond INTERNAL */
bool ll_detail_SetRef(LinkedList_t *list, size_t index, const void *element);
bool ll_detail_IndexOfRef(const LinkedList_t *list, const void *element, size_t *outIndex);
bool ll_detail_ContainsRef(const LinkedList_t *list, const void *element);
bool ll_detail_InsertRef(LinkedList_t *list, size_t index, const void *element);
bool ll_detail_PrependRef(LinkedList_t *list, const void *element);
bool ll_detail_AppendRef(LinkedList_t *list, const void *element);

#define ADT_PRIMITIVE(Suffix, Type)                                                    \
    bool ll_detail_IndexOf##Suffix(const LinkedList_t *list, Type element, size_t *outIndex); \
    bool ll_detail_Contains##Suffix(const LinkedList_t *list, Type element);                  \
    bool ll_detail_Set##Suffix(LinkedList_t *list, size_t index, Type element);               \
    bool ll_detail_Insert##Suffix(LinkedList_t *list, size_t index, Type element);            \
    bool ll_detail_Prepend##Suffix(LinkedList_t *list, Type element);                         \
    bool ll_detail_Append##Suffix(LinkedList_t *list, Type element);
ADT_FOR_EACH_PRIMITIVE(ADT_PRIMITIVE)
#undef ADT_PRIMITIVE
/** @endcond */

#endif
