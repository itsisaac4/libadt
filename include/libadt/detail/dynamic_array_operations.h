#ifndef LIBADT_DETAIL_DYNAMIC_ARRAY_OPERATIONS_H
#define LIBADT_DETAIL_DYNAMIC_ARRAY_OPERATIONS_H

#ifndef LIBADT_DYNAMIC_ARRAY_H
#error "Include libadt/dynamic_array.h instead."
#endif

/** @cond INTERNAL */
bool da_detail_SetRef(DynamicArray_t *array, size_t index, const void *element);
bool da_detail_IndexOfRef(const DynamicArray_t *array, const void *element, size_t *outIndex);
bool da_detail_ContainsRef(const DynamicArray_t *array, const void *element);
bool da_detail_InsertRef(DynamicArray_t *array, size_t index, const void *element);
bool da_detail_PrependRef(DynamicArray_t *array, const void *element);
bool da_detail_AppendRef(DynamicArray_t *array, const void *element);

#define ADT_PRIMITIVE(Suffix, Type)                                                       \
    bool da_detail_IndexOf##Suffix(const DynamicArray_t *array, Type element, size_t *outIndex); \
    bool da_detail_Contains##Suffix(const DynamicArray_t *array, Type element);                  \
    bool da_detail_Set##Suffix(DynamicArray_t *array, size_t index, Type element);               \
    bool da_detail_Insert##Suffix(DynamicArray_t *array, size_t index, Type element);            \
    bool da_detail_Prepend##Suffix(DynamicArray_t *array, Type element);                         \
    bool da_detail_Append##Suffix(DynamicArray_t *array, Type element);
ADT_FOR_EACH_PRIMITIVE(ADT_PRIMITIVE)
#undef ADT_PRIMITIVE
/** @endcond */

#endif
