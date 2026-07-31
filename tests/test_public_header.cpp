extern "C"
{
#include "libadt/libadt.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(PublicHeader)
{
};

TEST(PublicHeader, ExposesEveryContainerAndSharedType)
{
    DynamicArray_t array = {};
    LinkedList_t list = {};
    Stack_t stack = {};
    Queue_t queue = {};
    ADT_ElementTypeInfo_t elementType = {};

    UNSIGNED_LONGS_EQUAL(0, adt_Size(&array));
    UNSIGNED_LONGS_EQUAL(0, adt_Size(&list));
    UNSIGNED_LONGS_EQUAL(0, adt_Size(&stack));
    UNSIGNED_LONGS_EQUAL(0, adt_Size(&queue));
    UNSIGNED_LONGS_EQUAL(0, elementType.elementSize);
}
