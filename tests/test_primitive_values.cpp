extern "C"
{
#include "libadt/dynamic_array.h"
#include "libadt/linked_list.h"
}

#include "CppUTest/TestHarness.h"

extern "C"
{
    bool C23PrimitiveTypesDispatchByValue(void);
    bool C23PrimitiveOperationsDispatchByValue(void);
}

TEST_GROUP(DynamicArrayPrimitiveValues)
{
};

#define ADT_PRIMITIVE(Suffix, Type)                                  \
    TEST(DynamicArrayPrimitiveValues, Suffix)                         \
    {                                                                \
        ADT_TypeInfo_t type = {                                      \
            sizeof(Type),                                            \
            Compare##Suffix,                                         \
            Print##Suffix,                                           \
            NULL};                                                   \
        DynamicArray_t array = {};                                   \
        Type output = (Type)0;                                       \
        Type expected = (Type)4;                                     \
        size_t index = 0;                                            \
                                                                     \
        CHECK_TRUE(da_Init(&array, type));                            \
        CHECK_TRUE(da_Append##Suffix(&array, (Type)2));               \
        CHECK_TRUE(da_Prepend##Suffix(&array, (Type)1));              \
        CHECK_TRUE(da_Insert##Suffix(&array, 1, (Type)3));            \
        CHECK_TRUE(da_Set##Suffix(&array, 2, (Type)4));               \
        CHECK_TRUE(da_Contains##Suffix(&array, (Type)3));             \
        CHECK_FALSE(da_Contains##Suffix(&array, (Type)2));            \
        CHECK_TRUE(da_IndexOf##Suffix(&array, (Type)4, &index));      \
        UNSIGNED_LONGS_EQUAL(2, index);                               \
        CHECK_TRUE(da_Get(&array, 2, &output));                       \
        CHECK(Compare##Suffix(&output, &expected) == 0);              \
                                                                     \
        da_Destroy(&array);                                          \
    }
ADT_FOR_EACH_PRIMITIVE(ADT_PRIMITIVE)
#undef ADT_PRIMITIVE

TEST(DynamicArrayPrimitiveValues, RejectsDifferentElementSize)
{
    ADT_TypeInfo_t type = {
        sizeof(double),
        CompareDouble,
        PrintDouble,
        NULL};
    DynamicArray_t array = {};
    size_t index = 0;

    CHECK_TRUE(da_Init(&array, type));
    CHECK_FALSE(da_AppendInt(&array, 5));
    CHECK_FALSE(da_PrependInt(&array, 5));
    CHECK_FALSE(da_InsertInt(&array, 0, 5));
    CHECK_FALSE(da_SetInt(&array, 0, 5));
    CHECK_FALSE(da_ContainsInt(&array, 5));
    CHECK_FALSE(da_IndexOfInt(&array, 5, &index));

    da_Destroy(&array);
}

TEST_GROUP(LinkedListPrimitiveValues)
{
};

#define ADT_PRIMITIVE(Suffix, Type)                                  \
    TEST(LinkedListPrimitiveValues, Suffix)                           \
    {                                                                \
        ADT_TypeInfo_t type = {                                      \
            sizeof(Type),                                            \
            Compare##Suffix,                                         \
            Print##Suffix,                                           \
            NULL};                                                   \
        LinkedList_t list = {};                                      \
        Type output = (Type)0;                                       \
        Type expected = (Type)4;                                     \
        size_t index = 0;                                            \
                                                                     \
        CHECK_TRUE(ll_Init(&list, type));                             \
        CHECK_TRUE(ll_Append##Suffix(&list, (Type)2));                \
        CHECK_TRUE(ll_Prepend##Suffix(&list, (Type)1));               \
        CHECK_TRUE(ll_Insert##Suffix(&list, 1, (Type)3));             \
        CHECK_TRUE(ll_Set##Suffix(&list, 2, (Type)4));                \
        CHECK_TRUE(ll_Contains##Suffix(&list, (Type)3));              \
        CHECK_FALSE(ll_Contains##Suffix(&list, (Type)2));             \
        CHECK_TRUE(ll_IndexOf##Suffix(&list, (Type)4, &index));       \
        UNSIGNED_LONGS_EQUAL(2, index);                               \
        CHECK_TRUE(ll_Get(&list, 2, &output));                        \
        CHECK(Compare##Suffix(&output, &expected) == 0);              \
                                                                     \
        ll_Destroy(&list);                                           \
    }
ADT_FOR_EACH_PRIMITIVE(ADT_PRIMITIVE)
#undef ADT_PRIMITIVE

TEST(LinkedListPrimitiveValues, RejectsDifferentElementSize)
{
    ADT_TypeInfo_t type = {
        sizeof(double),
        CompareDouble,
        PrintDouble,
        NULL};
    LinkedList_t list = {};
    size_t index = 0;

    CHECK_TRUE(ll_Init(&list, type));
    CHECK_FALSE(ll_AppendInt(&list, 5));
    CHECK_FALSE(ll_PrependInt(&list, 5));
    CHECK_FALSE(ll_InsertInt(&list, 0, 5));
    CHECK_FALSE(ll_SetInt(&list, 0, 5));
    CHECK_FALSE(ll_ContainsInt(&list, 5));
    CHECK_FALSE(ll_IndexOfInt(&list, 5, &index));

    ll_Destroy(&list);
}

TEST_GROUP(PrimitiveValueGenerics)
{
};

TEST(PrimitiveValueGenerics, DispatchesEverySupportedType)
{
    CHECK_TRUE(C23PrimitiveTypesDispatchByValue());
}

TEST(PrimitiveValueGenerics, DispatchesEveryValueOperation)
{
    CHECK_TRUE(C23PrimitiveOperationsDispatchByValue());
}
