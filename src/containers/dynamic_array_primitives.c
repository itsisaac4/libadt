#include "libadt/dynamic_array.h"
#include "libadt/primitive_dispatch.h"

#define DEFINE_DYNAMIC_ARRAY_PRIMITIVE(Suffix, Type)                                     \
    bool da_IndexOf##Suffix(const DynamicArray_t *array, Type element, size_t *outIndex) \
    {                                                                                    \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                            \
               da_IndexOfRef(array, &element, outIndex);                                 \
    }                                                                                    \
                                                                                         \
    bool da_Contains##Suffix(const DynamicArray_t *array, Type element)                  \
    {                                                                                    \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                            \
               da_ContainsRef(array, &element);                                          \
    }                                                                                    \
                                                                                         \
    bool da_Set##Suffix(DynamicArray_t *array, size_t index, Type element)               \
    {                                                                                    \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                            \
               da_SetRef(array, index, &element);                                        \
    }                                                                                    \
                                                                                         \
    bool da_Insert##Suffix(DynamicArray_t *array, size_t index, Type element)            \
    {                                                                                    \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                            \
               da_InsertRef(array, index, &element);                                     \
    }                                                                                    \
                                                                                         \
    bool da_Prepend##Suffix(DynamicArray_t *array, Type element)                         \
    {                                                                                    \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                            \
               da_PrependRef(array, &element);                                           \
    }                                                                                    \
                                                                                         \
    bool da_Append##Suffix(DynamicArray_t *array, Type element)                          \
    {                                                                                    \
        return adt_AcceptsElementSize(array, sizeof(Type)) &&                            \
               da_AppendRef(array, &element);                                            \
    }
ADT_FOR_EACH_PRIMITIVE(DEFINE_DYNAMIC_ARRAY_PRIMITIVE)
#undef DEFINE_DYNAMIC_ARRAY_PRIMITIVE
