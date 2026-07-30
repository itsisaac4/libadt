#include <stdbool.h>

#include "libadt/dynamic_array.h"
#include "libadt/linked_list.h"

typedef struct
{
    int value;
} MacroStruct;

bool C23UnknownTypeUsesSafeDefaults(void)
{
    DynamicArray_t array = {0};

    if (!DA_INIT(&array, MacroStruct))
    {
        return false;
    }

    const bool usesSafeDefaults =
        array.super.type.compare == NULL &&
        array.super.type.print == NULL;

    MacroStruct value = {.value = 42};
    MacroStruct target = {.value = 42};
    const bool supportsByteComparison =
        da_AppendRef(&array, &value) &&
        da_ContainsRef(&array, &target);

    da_Destroy(&array);
    return usesSafeDefaults && supportsByteComparison;
}

bool C23InitFromInfersIntType(void)
{
    int values[] = {10, 20, 30};
    DynamicArray_t array = {0};

    if (!DA_INIT_FROM(&array, values))
    {
        return false;
    }

    const bool inferredInt =
        array.super.type.elementSize == sizeof(int) &&
        array.super.type.compare == CompareInt &&
        array.super.type.print == PrintInt &&
        array.super.size == ARRAY_COUNT(values);

    da_Destroy(&array);
    return inferredInt;
}

bool C23LinkedListInitInfersIntType(void)
{
    LinkedList_t list = {0};

    if (!LL_INIT(&list, int))
    {
        return false;
    }

    const bool inferredInt =
        list.super.type.elementSize == sizeof(int) &&
        list.super.type.compare == CompareInt &&
        list.super.type.print == PrintInt &&
        list.super.size == 0;

    ll_Destroy(&list);
    return inferredInt;
}

bool C23LinkedListInitFromInfersIntType(void)
{
    int values[] = {10, 20, 30};
    LinkedList_t list = {0};

    if (!LL_INIT_FROM(&list, values))
    {
        return false;
    }

    const bool inferredInt =
        list.super.type.elementSize == sizeof(int) &&
        list.super.type.compare == CompareInt &&
        list.super.type.print == PrintInt &&
        list.super.size == ARRAY_COUNT(values);

    ll_Destroy(&list);
    return inferredInt;
}

#define DEFINE_PRIMITIVE_DISPATCH_CHECK(Suffix, Type)                  \
    static bool CheckDynamicArray##Suffix(void)                         \
    {                                                                  \
        const ADT_TypeInfo_t type = {                                  \
            .elementSize = sizeof(Type),                               \
            .compare = Compare##Suffix,                                \
            .print = Print##Suffix,                                    \
            .destroy = NULL};                                          \
        DynamicArray_t array = {0};                                    \
        Type value = (Type)7;                                          \
                                                                       \
        if (!da_Init(&array, type))                                    \
        {                                                              \
            return false;                                              \
        }                                                              \
                                                                       \
        const bool works =                                             \
            da_AppendValue(&array, value) &&                           \
            da_ContainsValue(&array, value);                           \
        da_Destroy(&array);                                            \
        return works;                                                  \
    }                                                                  \
                                                                       \
    static bool CheckLinkedList##Suffix(void)                          \
    {                                                                  \
        const ADT_TypeInfo_t type = {                                  \
            .elementSize = sizeof(Type),                               \
            .compare = Compare##Suffix,                                \
            .print = Print##Suffix,                                    \
            .destroy = NULL};                                          \
        LinkedList_t list = {0};                                       \
        Type value = (Type)7;                                          \
                                                                       \
        if (!ll_Init(&list, type))                                     \
        {                                                              \
            return false;                                              \
        }                                                              \
                                                                       \
        const bool works =                                             \
            ll_AppendValue(&list, value) &&                            \
            ll_ContainsValue(&list, value);                            \
        ll_Destroy(&list);                                             \
        return works;                                                  \
    }
DEFINE_PRIMITIVE_DISPATCH_CHECK(Char, char)
DEFINE_PRIMITIVE_DISPATCH_CHECK(Int, int)
DEFINE_PRIMITIVE_DISPATCH_CHECK(UnsignedInt, unsigned int)
DEFINE_PRIMITIVE_DISPATCH_CHECK(Long, long)
DEFINE_PRIMITIVE_DISPATCH_CHECK(Float, float)
DEFINE_PRIMITIVE_DISPATCH_CHECK(Double, double)
#undef DEFINE_PRIMITIVE_DISPATCH_CHECK

bool C23PrimitiveTypesDispatchByValue(void)
{
#define CHECK_PRIMITIVE_DISPATCH(Suffix, Type) \
    if (!CheckDynamicArray##Suffix() || !CheckLinkedList##Suffix()) \
    {                                                               \
        return false;                                               \
    }
    CHECK_PRIMITIVE_DISPATCH(Char, char)
    CHECK_PRIMITIVE_DISPATCH(Int, int)
    CHECK_PRIMITIVE_DISPATCH(UnsignedInt, unsigned int)
    CHECK_PRIMITIVE_DISPATCH(Long, long)
    CHECK_PRIMITIVE_DISPATCH(Float, float)
    CHECK_PRIMITIVE_DISPATCH(Double, double)
#undef CHECK_PRIMITIVE_DISPATCH

    return true;
}

bool C23PrimitiveOperationsDispatchByValue(void)
{
    const ADT_TypeInfo_t type = {
        .elementSize = sizeof(int),
        .compare = CompareInt,
        .print = PrintInt,
        .destroy = NULL};
    DynamicArray_t array = {0};
    LinkedList_t list = {0};
    size_t arrayIndex = 0;
    size_t listIndex = 0;

    if (!da_Init(&array, type) || !ll_Init(&list, type))
    {
        da_Destroy(&array);
        ll_Destroy(&list);
        return false;
    }

    const bool arrayWorks =
        da_AppendValue(&array, 2) &&
        da_PrependValue(&array, 1) &&
        da_InsertValue(&array, 1, 3) &&
        da_SetValue(&array, 2, 4) &&
        da_ContainsValue(&array, 3) &&
        da_IndexOfValue(&array, 4, &arrayIndex) &&
        arrayIndex == 2;

    const bool listWorks =
        ll_AppendValue(&list, 2) &&
        ll_PrependValue(&list, 1) &&
        ll_InsertValue(&list, 1, 3) &&
        ll_SetValue(&list, 2, 4) &&
        ll_ContainsValue(&list, 3) &&
        ll_IndexOfValue(&list, 4, &listIndex) &&
        listIndex == 2;

    da_Destroy(&array);
    ll_Destroy(&list);
    return arrayWorks && listWorks;
}
