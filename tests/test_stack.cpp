extern "C"
{
#include "libadt/stack.h"
}

#include "CppUTest/TestHarness.h"
#include <cstdlib>
#include <cstring>

extern "C"
{
    bool C23StackInitInfersIntType(void);
    bool C23StackInitFromInfersIntType(void);
}

typedef struct
{
    int value;
    char *resource;
} StackOwnedValue_t;

static size_t stackDestroyCount;

static ADT_ElementTypeInfo_t StackIntType()
{
    return ADT_ElementTypeInfo_t{
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
}

static char *CopyStackString(const char *text)
{
    const size_t size = strlen(text) + 1;
    char *copy = static_cast<char *>(malloc(size));
    CHECK(copy != NULL);
    memcpy(copy, text, size);
    return copy;
}

static void DestroyStackOwnedValue(void *element)
{
    StackOwnedValue_t *value = static_cast<StackOwnedValue_t *>(element);
    free(value->resource);
    value->resource = NULL;
    stackDestroyCount++;
}

TEST_GROUP(Stack)
{
    Stack_t stack;

    void setup()
    {
        stack = Stack_t{};
        stackDestroyCount = 0;
    }

    void teardown()
    {
        st_Destroy(&stack);
    }
};

TEST(Stack, InitCreatesEmptyPolymorphicStack)
{
    CHECK_TRUE(st_Init(&stack, StackIntType()));
    CHECK_TRUE(adt_IsEmpty(&stack));
    POINTERS_EQUAL(static_cast<void *>(&stack), static_cast<void *>(&stack.super));
    STRCMP_EQUAL("Stack", stack.super._private.vtable->containerName);
    CHECK(stack._private.storage.data != NULL);
}

TEST(Stack, InitMacrosInferPrimitiveType)
{
    CHECK_TRUE(C23StackInitInfersIntType());
    CHECK_TRUE(C23StackInitFromInfersIntType());
}

TEST(Stack, InitFromTreatsLastValueAsTop)
{
    int values[] = {10, 20, 30};
    CHECK_TRUE(st_InitFrom(&stack, values, 3, StackIntType()));

    values[2] = 99;
    int top = 0;
    CHECK_TRUE(st_Peek(&stack, &top));
    LONGS_EQUAL(30, top);
    UNSIGNED_LONGS_EQUAL(3, adt_Size(&stack));
}

TEST(Stack, InitAndInitFromRejectInvalidArguments)
{
    int value = 1;
    ADT_ElementTypeInfo_t invalid = {};

    CHECK_FALSE(st_Init(NULL, StackIntType()));
    CHECK_FALSE(st_Init(&stack, invalid));
    CHECK_FALSE(st_InitFrom(NULL, &value, 1, StackIntType()));
    CHECK_FALSE(st_InitFrom(&stack, NULL, 1, StackIntType()));
    CHECK_FALSE(st_InitFrom(&stack, &value, 1, invalid));
}

TEST(Stack, PushPeekAndPopUseLifoOrder)
{
    int first = 1;
    int second = 2;
    int third = 3;
    CHECK_TRUE(st_Init(&stack, StackIntType()));
    CHECK_TRUE(st_detail_PushRef(&stack, &first));
    CHECK_TRUE(st_detail_PushRef(&stack, &second));
    CHECK_TRUE(st_detail_PushRef(&stack, &third));

    int actual = 0;
    CHECK_TRUE(st_Peek(&stack, &actual));
    LONGS_EQUAL(3, actual);
    CHECK_TRUE(st_Pop(&stack, &actual));
    LONGS_EQUAL(3, actual);
    CHECK_TRUE(st_Pop(&stack, &actual));
    LONGS_EQUAL(2, actual);
    CHECK_TRUE(st_Pop(&stack, &actual));
    LONGS_EQUAL(1, actual);
    CHECK_TRUE(adt_IsEmpty(&stack));
}

TEST(Stack, EmptyOperationsAndInvalidPushesFail)
{
    int value = 0;
    CHECK_TRUE(st_Init(&stack, StackIntType()));

    CHECK_FALSE(st_Peek(&stack, &value));
    CHECK_FALSE(st_Pop(&stack, &value));
    CHECK_FALSE(st_Discard(&stack));
    CHECK_FALSE(st_detail_PushRef(NULL, &value));
    CHECK_FALSE(st_detail_PushRef(&stack, NULL));
    CHECK_FALSE(st_Peek(NULL, &value));
    CHECK_FALSE(st_Peek(&stack, NULL));
}

TEST(Stack, PrimitiveWrapperRejectsDifferentSizedType)
{
    CHECK_TRUE(st_Init(&stack, StackIntType()));
    CHECK_FALSE(st_detail_PushDouble(&stack, 4.5));
    CHECK_TRUE(adt_IsEmpty(&stack));
}

TEST(Stack, PopTransfersResourcesWithoutDestroyingThem)
{
    ADT_ElementTypeInfo_t type = {
        sizeof(StackOwnedValue_t),
        NULL,
        NULL,
        NULL,
        DestroyStackOwnedValue};
    StackOwnedValue_t value = {7, CopyStackString("owned")};
    StackOwnedValue_t taken = {};

    CHECK_TRUE(st_Init(&stack, type));
    CHECK_TRUE(st_detail_PushRef(&stack, &value));
    CHECK_TRUE(st_Pop(&stack, &taken));
    UNSIGNED_LONGS_EQUAL(0, stackDestroyCount);
    STRCMP_EQUAL("owned", taken.resource);

    DestroyStackOwnedValue(&taken);
    UNSIGNED_LONGS_EQUAL(1, stackDestroyCount);
}

TEST(Stack, DiscardAndClearDestroyOwnedResources)
{
    ADT_ElementTypeInfo_t type = {
        sizeof(StackOwnedValue_t),
        NULL,
        NULL,
        NULL,
        DestroyStackOwnedValue};
    StackOwnedValue_t first = {1, CopyStackString("first")};
    StackOwnedValue_t second = {2, CopyStackString("second")};

    CHECK_TRUE(st_Init(&stack, type));
    CHECK_TRUE(st_detail_PushRef(&stack, &first));
    CHECK_TRUE(st_detail_PushRef(&stack, &second));
    CHECK_TRUE(st_Discard(&stack));
    UNSIGNED_LONGS_EQUAL(1, stackDestroyCount);

    const size_t capacity = stack._private.storage.capacity;
    st_Clear(&stack);
    UNSIGNED_LONGS_EQUAL(2, stackDestroyCount);
    CHECK_TRUE(adt_IsEmpty(&stack));
    UNSIGNED_LONGS_EQUAL(capacity, stack._private.storage.capacity);
    CHECK(stack.super._private.elementType.destroy == DestroyStackOwnedValue);
}

TEST(Stack, PopRejectsOutputInsideOwnedStorage)
{
    int value = 1;
    CHECK_TRUE(st_Init(&stack, StackIntType()));
    CHECK_TRUE(st_detail_PushRef(&stack, &value));

    void *inside = contiguousStorage_At(
        &stack._private.storage,
        0,
        sizeof(int));
    CHECK_FALSE(st_Pop(&stack, inside));
    UNSIGNED_LONGS_EQUAL(1, adt_Size(&stack));
}

TEST(Stack, SharedStatisticsAndSortingUseBottomToTopTraversal)
{
    int values[] = {4, 1, 2, 2};
    CHECK_TRUE(st_InitFrom(&stack, values, 4, StackIntType()));

    double mean = 0.0;
    double median = 0.0;
    double mode = 0.0;
    CHECK_TRUE(adt_Mean(&stack, &mean));
    CHECK_TRUE(adt_Median(&stack, &median));
    CHECK_TRUE(adt_Mode(&stack, &mode));
    DOUBLES_EQUAL(2.25, mean, 0.000001);
    DOUBLES_EQUAL(2.0, median, 0.000001);
    DOUBLES_EQUAL(2.0, mode, 0.000001);

    CHECK_TRUE(adt_Sort(&stack, ADT_SORT_QUICK));
    int top = 0;
    CHECK_TRUE(st_Peek(&stack, &top));
    LONGS_EQUAL(4, top);
}

TEST(Stack, DestroyResetsStateAndSupportsRepeatedCalls)
{
    CHECK_TRUE(st_Init(&stack, StackIntType()));
    st_Destroy(&stack);
    st_Destroy(&stack);
    st_Destroy(NULL);

    POINTERS_EQUAL(NULL, stack._private.storage.data);
    UNSIGNED_LONGS_EQUAL(0, adt_Size(&stack));
    UNSIGNED_LONGS_EQUAL(0, stack.super._private.elementType.elementSize);
}
