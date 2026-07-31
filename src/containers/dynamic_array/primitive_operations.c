#include "libadt/dynamic_array.h"
#include "libadt/internal/primitive_dispatch.h"

#define DEFINE_DYNAMIC_ARRAY_PRIMITIVE(Suffix, Type)                                            \
    bool da_detail_IndexOf##Suffix(const DynamicArray_t *array, Type element, size_t *outIndex) \
    {                                                                                           \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                                   \
               da_detail_IndexOfRef(array, &element, outIndex);                                 \
    }                                                                                           \
                                                                                                \
    bool da_detail_Contains##Suffix(const DynamicArray_t *array, Type element)                   \
    {                                                                                           \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                                   \
               da_detail_ContainsRef(array, &element);                                          \
    }                                                                                           \
                                                                                                \
    bool da_detail_Set##Suffix(DynamicArray_t *array, size_t index, Type element)                \
    {                                                                                           \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                                   \
               da_detail_SetRef(array, index, &element);                                        \
    }                                                                                           \
                                                                                                \
    bool da_detail_Insert##Suffix(DynamicArray_t *array, size_t index, Type element)             \
    {                                                                                           \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                                   \
               da_detail_InsertRef(array, index, &element);                                     \
    }                                                                                           \
                                                                                                \
    bool da_detail_Prepend##Suffix(DynamicArray_t *array, Type element)                          \
    {                                                                                           \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                                   \
               da_detail_PrependRef(array, &element);                                           \
    }                                                                                           \
                                                                                                \
    bool da_detail_Append##Suffix(DynamicArray_t *array, Type element)                           \
    {                                                                                           \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                                   \
               da_detail_AppendRef(array, &element);                                            \
    }
ADT_FOR_EACH_PRIMITIVE(DEFINE_DYNAMIC_ARRAY_PRIMITIVE)
#undef DEFINE_DYNAMIC_ARRAY_PRIMITIVE
