extern "C"
{
#include "libadt/dynamic_array.h"
#include "libadt/comparators.h"
}

#include "CppUTest/TestHarness.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>

#define INITIAL_CAPACITY 8

static ADT_TypeInfo_t IntTypeInfo()
{
    ADT_TypeInfo_t info = {sizeof(int), CompareInt, PrintInt, NULL};
    return info;
}

static ADT_TypeInfo_t DoubleTypeInfo()
{
    ADT_TypeInfo_t info = {sizeof(double), CompareDouble, PrintDouble, NULL};
    return info;
}

static ADT_TypeInfo_t InvalidTypeInfo()
{
    ADT_TypeInfo_t info = {0, NULL, NULL, NULL};
    return info;
}

/* =========================================================
 * Initialization and destruction tests
 * ========================================================= */

TEST_GROUP(DynamicArrayInitialization)
{
    DynamicArray_t array;

    void setup()
    {
        array.data = NULL;
        array.size = 0;
        array.capacity = 0;
        array.typeInfo = InvalidTypeInfo();
    }

    void teardown()
    {
        da_Destroy(&array);
    }
};

TEST(DynamicArrayInitialization, InitCreatesEmptyArray)
{
    CHECK_TRUE(da_Init(&array, IntTypeInfo()));

    CHECK(array.data != NULL);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(INITIAL_CAPACITY, array.capacity);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.typeInfo.elementSize);
}

TEST(DynamicArrayInitialization, InitFromCopiesIntegerElements)
{
    int input[] = {10, 20, 30, 40};

    CHECK_TRUE(da_InitFrom(&array, input, 4, IntTypeInfo()));

    int *stored = static_cast<int *>(array.data);

    UNSIGNED_LONGS_EQUAL(4, array.size);
    CHECK(array.capacity >= array.size);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.typeInfo.elementSize);

    LONGS_EQUAL(10, stored[0]);
    LONGS_EQUAL(20, stored[1]);
    LONGS_EQUAL(30, stored[2]);
    LONGS_EQUAL(40, stored[3]);
}

TEST(DynamicArrayInitialization, InitFromMakesIndependentCopy)
{
    int input[] = {10, 20, 30};

    CHECK_TRUE(da_InitFrom(&array, input, 3, IntTypeInfo()));

    CHECK(array.data != input);

    input[0] = 999;

    int *stored = static_cast<int *>(array.data);

    LONGS_EQUAL(10, stored[0]);
}

TEST(DynamicArrayInitialization, InitFromUsesEnoughCapacity)
{
    int input[20];

    for (int i = 0; i < 20; i++)
    {
        input[i] = i;
    }

    CHECK_TRUE(da_InitFrom(&array, input, 20, IntTypeInfo()));

    UNSIGNED_LONGS_EQUAL(20, array.size);
    CHECK(array.capacity >= array.size);
}

TEST(DynamicArrayInitialization, InitFromPreservesAllElementsAboveInitialCapacity)
{
    const size_t count = INITIAL_CAPACITY + 5;
    int input[count];

    for (size_t i = 0; i < count; i++)
    {
        input[i] = static_cast<int>(i * 10);
    }

    CHECK_TRUE(da_InitFrom(&array, input, count, IntTypeInfo()));

    int *stored = static_cast<int *>(array.data);

    UNSIGNED_LONGS_EQUAL(count, array.size);
    CHECK(array.capacity >= count);

    for (size_t i = 0; i < count; i++)
    {
        LONGS_EQUAL(
            static_cast<long>(i * 10),
            stored[i]);
    }
}

TEST(DynamicArrayInitialization, InitFromAllowsZeroElements)
{
    int input[] = {1, 2, 3};
    CHECK_TRUE(da_InitFrom(&array, input, 0, IntTypeInfo()));

    UNSIGNED_LONGS_EQUAL(0, array.size);
    CHECK(array.capacity >= INITIAL_CAPACITY);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.typeInfo.elementSize);
}

TEST(DynamicArrayInitialization, InitRejectsNullArray)
{
    CHECK_FALSE(da_Init(NULL, IntTypeInfo()));
}

TEST(DynamicArrayInitialization, InitRejectsZeroElementSize)
{
    CHECK_FALSE(da_Init(&array, InvalidTypeInfo()));

    POINTERS_EQUAL(NULL, array.data);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(0, array.capacity);
    UNSIGNED_LONGS_EQUAL(0, array.typeInfo.elementSize);
}

TEST(DynamicArrayInitialization, InitFromRejectsNullArray)
{
    CHECK_FALSE(da_InitFrom(&array, NULL, 3, IntTypeInfo()));
}

TEST(DynamicArrayInitialization, InitFromRejectsNullElementsWithCount)
{
    CHECK_FALSE(da_InitFrom(&array, NULL, 3, IntTypeInfo()));

    POINTERS_EQUAL(NULL, array.data);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(0, array.capacity);
}

TEST(DynamicArrayInitialization, InitFromSupportsDoubleElements)
{
    double input[] = {1.5, 2.5, 3.5};

    CHECK_TRUE(da_InitFrom(&array, input, 3, DoubleTypeInfo()));

    double *stored = static_cast<double *>(array.data);

    UNSIGNED_LONGS_EQUAL(3, array.size);
    UNSIGNED_LONGS_EQUAL(sizeof(double), array.typeInfo.elementSize);

    DOUBLES_EQUAL(1.5, stored[0], 0.000001);
    DOUBLES_EQUAL(2.5, stored[1], 0.000001);
    DOUBLES_EQUAL(3.5, stored[2], 0.000001);
}

TEST(DynamicArrayInitialization, DestroyResetsArray)
{
    CHECK_TRUE(da_Init(&array, IntTypeInfo()));

    da_Destroy(&array);

    POINTERS_EQUAL(NULL, array.data);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(0, array.capacity);
    UNSIGNED_LONGS_EQUAL(0, array.typeInfo.elementSize);
}

TEST(DynamicArrayInitialization, DestroyHandlesNullPointer)
{
    da_Destroy(NULL);
}

TEST(DynamicArrayInitialization, DestroyCanBeCalledTwice)
{
    CHECK_TRUE(da_Init(&array, IntTypeInfo()));

    da_Destroy(&array);
    da_Destroy(&array);

    POINTERS_EQUAL(NULL, array.data);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(0, array.capacity);
    UNSIGNED_LONGS_EQUAL(0, array.typeInfo.elementSize);
}

/* =========================================================
 * Integer insertion tests
 * ========================================================= */

TEST_GROUP(DynamicArrayInsertion)
{
    DynamicArray_t array;

    void setup()
    {
        array.data = NULL;
        array.size = 0;
        array.capacity = 0;
        array.typeInfo = InvalidTypeInfo();

        CHECK_TRUE(da_Init(&array, IntTypeInfo()));
    }

    void teardown()
    {
        da_Destroy(&array);
    }

    int *values()
    {
        return static_cast<int *>(array.data);
    }
};

TEST(DynamicArrayInsertion, AppendAddsElementToEmptyArray)
{
    int value = 10;

    CHECK_TRUE(
        da_Append(&array, &value));

    UNSIGNED_LONGS_EQUAL(1, array.size);
    LONGS_EQUAL(10, values()[0]);
}

TEST(DynamicArrayInsertion, AppendPreservesInsertionOrder)
{
    int first = 10;
    int second = 20;
    int third = 30;

    CHECK_TRUE(da_Append(&array, &first));
    CHECK_TRUE(da_Append(&array, &second));
    CHECK_TRUE(da_Append(&array, &third));

    UNSIGNED_LONGS_EQUAL(3, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
    LONGS_EQUAL(30, values()[2]);
}

TEST(DynamicArrayInsertion, PrependAddsElementToEmptyArray)
{
    int value = 10;

    CHECK_TRUE(
        da_Prepend(&array, &value));

    UNSIGNED_LONGS_EQUAL(1, array.size);
    LONGS_EQUAL(10, values()[0]);
}

TEST(DynamicArrayInsertion, PrependShiftsExistingElementsRight)
{
    int twenty = 20;
    int thirty = 30;
    int ten = 10;

    CHECK_TRUE(da_Append(&array, &twenty));
    CHECK_TRUE(da_Append(&array, &thirty));
    CHECK_TRUE(da_Prepend(&array, &ten));

    UNSIGNED_LONGS_EQUAL(3, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
    LONGS_EQUAL(30, values()[2]);
}

TEST(DynamicArrayInsertion, InsertIntoEmptyArrayAtZeroSucceeds)
{
    int value = 10;

    CHECK_TRUE(
        da_Insert(&array, 0, &value));

    UNSIGNED_LONGS_EQUAL(1, array.size);
    LONGS_EQUAL(10, values()[0]);
}

TEST(DynamicArrayInsertion, InsertAddsElementInMiddle)
{
    int ten = 10;
    int thirty = 30;
    int twenty = 20;

    CHECK_TRUE(da_Append(&array, &ten));
    CHECK_TRUE(da_Append(&array, &thirty));
    CHECK_TRUE(da_Insert(&array, 1, &twenty));

    UNSIGNED_LONGS_EQUAL(3, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
    LONGS_EQUAL(30, values()[2]);
}

TEST(DynamicArrayInsertion, InsertAtZeroBehavesLikePrepend)
{
    int twenty = 20;
    int ten = 10;

    CHECK_TRUE(da_Append(&array, &twenty));
    CHECK_TRUE(da_Insert(&array, 0, &ten));

    UNSIGNED_LONGS_EQUAL(2, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
}

TEST(DynamicArrayInsertion, InsertAtSizeBehavesLikeAppend)
{
    int ten = 10;
    int twenty = 20;

    CHECK_TRUE(da_Append(&array, &ten));
    CHECK_TRUE(da_Insert(&array, array.size, &twenty));

    UNSIGNED_LONGS_EQUAL(2, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
}

TEST(DynamicArrayInsertion, InsertBeyondSizeFails)
{
    int value = 10;

    CHECK_FALSE(
        da_Insert(&array, 4, &value));

    UNSIGNED_LONGS_EQUAL(0, array.size);
}

TEST(DynamicArrayInsertion, AppendRejectsNullArray)
{
    int value = 10;

    CHECK_FALSE(
        da_Append(NULL, &value));
}

TEST(DynamicArrayInsertion, PrependRejectsNullArray)
{
    int value = 10;

    CHECK_FALSE(
        da_Prepend(NULL, &value));
}

TEST(DynamicArrayInsertion, InsertRejectsNullArray)
{
    int value = 10;

    CHECK_FALSE(
        da_Insert(NULL, 0, &value));
}

TEST(DynamicArrayInsertion, AppendRejectsNullElement)
{
    CHECK_FALSE(
        da_Append(&array, NULL));

    UNSIGNED_LONGS_EQUAL(0, array.size);
}

TEST(DynamicArrayInsertion, PrependRejectsNullElement)
{
    CHECK_FALSE(
        da_Prepend(&array, NULL));

    UNSIGNED_LONGS_EQUAL(0, array.size);
}

TEST(DynamicArrayInsertion, InsertRejectsNullElement)
{
    CHECK_FALSE(
        da_Insert(&array, 0, NULL));

    UNSIGNED_LONGS_EQUAL(0, array.size);
}

TEST(DynamicArrayInsertion, AppendExpandsCapacity)
{
    size_t originalCapacity = array.capacity;

    for (size_t i = 0; i <= originalCapacity; i++)
    {
        int value = static_cast<int>(i);

        CHECK_TRUE(
            da_Append(&array, &value));
    }

    UNSIGNED_LONGS_EQUAL(
        originalCapacity + 1,
        array.size);

    CHECK(array.capacity > originalCapacity);
    CHECK(array.capacity >= array.size);

    for (size_t i = 0; i < array.size; i++)
    {
        LONGS_EQUAL(
            static_cast<long>(i),
            values()[i]);
    }
}

TEST(DynamicArrayInsertion, PrependWhileFullExpandsAndPreservesData)
{
    size_t originalCapacity = array.capacity;

    for (size_t i = 0; i < originalCapacity; i++)
    {
        int value = static_cast<int>(i + 1);

        CHECK_TRUE(
            da_Append(&array, &value));
    }

    int prepended = 0;

    CHECK_TRUE(
        da_Prepend(&array, &prepended));

    UNSIGNED_LONGS_EQUAL(
        originalCapacity + 1,
        array.size);

    CHECK(array.capacity > originalCapacity);

    LONGS_EQUAL(0, values()[0]);

    for (size_t i = 0; i < originalCapacity; i++)
    {
        LONGS_EQUAL(
            static_cast<long>(i + 1),
            values()[i + 1]);
    }
}

TEST(DynamicArrayInsertion, InsertInMiddleWhileFullExpandsAndPreservesData)
{
    size_t originalCapacity = array.capacity;

    for (size_t i = 0; i < originalCapacity; i++)
    {
        int value = static_cast<int>(i);

        CHECK_TRUE(
            da_Append(&array, &value));
    }

    int inserted = 999;

    CHECK_TRUE(
        da_Insert(&array, 2, &inserted));

    UNSIGNED_LONGS_EQUAL(
        originalCapacity + 1,
        array.size);

    CHECK(array.capacity > originalCapacity);

    LONGS_EQUAL(0, values()[0]);
    LONGS_EQUAL(1, values()[1]);
    LONGS_EQUAL(999, values()[2]);

    for (size_t i = 2; i < originalCapacity; i++)
    {
        LONGS_EQUAL(
            static_cast<long>(i),
            values()[i + 1]);
    }
}

/* =========================================================
 * Generic insertion tests using doubles
 * ========================================================= */

TEST_GROUP(DynamicArrayDoubleInsertion)
{
    DynamicArray_t array;

    void setup()
    {
        array.data = NULL;
        array.size = 0;
        array.capacity = 0;
        array.typeInfo = InvalidTypeInfo();

        CHECK_TRUE(da_Init(&array, DoubleTypeInfo()));
    }

    void teardown()
    {
        da_Destroy(&array);
    }

    double *values()
    {
        return static_cast<double *>(array.data);
    }
};

TEST(DynamicArrayDoubleInsertion, AppendSupportsDoubleValues)
{
    double first = 1.5;
    double second = 2.5;

    CHECK_TRUE(da_Append(&array, &first));
    CHECK_TRUE(da_Append(&array, &second));

    UNSIGNED_LONGS_EQUAL(2, array.size);

    DOUBLES_EQUAL(1.5, values()[0], 0.000001);
    DOUBLES_EQUAL(2.5, values()[1], 0.000001);
}

TEST(DynamicArrayDoubleInsertion, PrependSupportsDoubleValues)
{
    double second = 2.5;
    double first = 1.5;

    CHECK_TRUE(da_Append(&array, &second));
    CHECK_TRUE(da_Prepend(&array, &first));

    UNSIGNED_LONGS_EQUAL(2, array.size);

    DOUBLES_EQUAL(1.5, values()[0], 0.000001);
    DOUBLES_EQUAL(2.5, values()[1], 0.000001);
}

TEST(DynamicArrayDoubleInsertion, InsertSupportsDoubleValues)
{
    double first = 1.5;
    double third = 3.5;
    double second = 2.5;

    CHECK_TRUE(da_Append(&array, &first));
    CHECK_TRUE(da_Append(&array, &third));
    CHECK_TRUE(da_Insert(&array, 1, &second));

    UNSIGNED_LONGS_EQUAL(3, array.size);

    DOUBLES_EQUAL(1.5, values()[0], 0.000001);
    DOUBLES_EQUAL(2.5, values()[1], 0.000001);
    DOUBLES_EQUAL(3.5, values()[2], 0.000001);
}

/* =========================================================
 * Removal and clear tests
 * ========================================================= */

TEST_GROUP(DynamicArrayRemoval)
{
    DynamicArray_t array;

    void setup()
    {
        array.data = NULL;
        array.size = 0;
        array.capacity = 0;
        array.typeInfo = InvalidTypeInfo();

        CHECK_TRUE(da_Init(&array, IntTypeInfo()));
    }

    void teardown()
    {
        da_Destroy(&array);
    }

    int *values()
    {
        return static_cast<int *>(array.data);
    }

    void appendValues()
    {
        int first = 10;
        int second = 20;
        int third = 30;
        int fourth = 40;

        CHECK_TRUE(da_Append(&array, &first));
        CHECK_TRUE(da_Append(&array, &second));
        CHECK_TRUE(da_Append(&array, &third));
        CHECK_TRUE(da_Append(&array, &fourth));
    }
};

TEST(DynamicArrayRemoval, RemoveFirstElement)
{
    appendValues();

    CHECK_TRUE(
        da_Remove(&array, 0));

    UNSIGNED_LONGS_EQUAL(3, array.size);

    LONGS_EQUAL(20, values()[0]);
    LONGS_EQUAL(30, values()[1]);
    LONGS_EQUAL(40, values()[2]);
}

TEST(DynamicArrayRemoval, RemoveMiddleElement)
{
    appendValues();

    CHECK_TRUE(
        da_Remove(&array, 1));

    UNSIGNED_LONGS_EQUAL(3, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(30, values()[1]);
    LONGS_EQUAL(40, values()[2]);
}

TEST(DynamicArrayRemoval, RemoveLastElement)
{
    appendValues();

    CHECK_TRUE(
        da_Remove(&array, 3));

    UNSIGNED_LONGS_EQUAL(3, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
    LONGS_EQUAL(30, values()[2]);
}

TEST(DynamicArrayRemoval, RemoveOnlyElement)
{
    int value = 10;

    CHECK_TRUE(
        da_Append(&array, &value));

    CHECK_TRUE(
        da_Remove(&array, 0));

    UNSIGNED_LONGS_EQUAL(0, array.size);
    CHECK_TRUE(da_IsEmpty(&array));
}

TEST(DynamicArrayRemoval, RemoveFromEmptyArrayFails)
{
    CHECK_FALSE(
        da_Remove(&array, 0));

    UNSIGNED_LONGS_EQUAL(0, array.size);
}

TEST(DynamicArrayRemoval, RemoveIndexEqualToSizeFails)
{
    appendValues();

    CHECK_FALSE(
        da_Remove(&array, array.size));

    UNSIGNED_LONGS_EQUAL(4, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
    LONGS_EQUAL(30, values()[2]);
    LONGS_EQUAL(40, values()[3]);
}

TEST(DynamicArrayRemoval, RemoveBeyondSizeFails)
{
    appendValues();

    CHECK_FALSE(
        da_Remove(&array, 100));

    UNSIGNED_LONGS_EQUAL(4, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
    LONGS_EQUAL(30, values()[2]);
    LONGS_EQUAL(40, values()[3]);
}

TEST(DynamicArrayRemoval, RemoveRejectsNullArray)
{
    CHECK_FALSE(
        da_Remove(NULL, 0));
}

TEST(DynamicArrayRemoval, ClearRemovesAllElements)
{
    appendValues();

    size_t oldCapacity = array.capacity;
    void *oldData = array.data;

    da_Clear(&array);

    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(oldCapacity, array.capacity);
    POINTERS_EQUAL(oldData, array.data);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.typeInfo.elementSize);

    CHECK_TRUE(
        da_IsEmpty(&array));
}

TEST(DynamicArrayRemoval, ClearEmptyArrayIsSafe)
{
    size_t oldCapacity = array.capacity;
    void *oldData = array.data;

    da_Clear(&array);

    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(oldCapacity, array.capacity);
    POINTERS_EQUAL(oldData, array.data);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.typeInfo.elementSize);
}

TEST(DynamicArrayRemoval, ClearHandlesNullArray)
{
    da_Clear(NULL);
}

TEST(DynamicArrayRemoval, ArrayCanBeReusedAfterClear)
{
    appendValues();

    size_t oldCapacity = array.capacity;
    void *oldData = array.data;

    da_Clear(&array);

    int newValue = 99;

    CHECK_TRUE(
        da_Append(&array, &newValue));

    UNSIGNED_LONGS_EQUAL(1, array.size);
    UNSIGNED_LONGS_EQUAL(oldCapacity, array.capacity);
    POINTERS_EQUAL(oldData, array.data);
    LONGS_EQUAL(99, values()[0]);
}

TEST(DynamicArrayRemoval, RemoveDoesNotChangeCapacity)
{
    appendValues();

    size_t oldCapacity = array.capacity;
    void *oldData = array.data;

    CHECK_TRUE(
        da_Remove(&array, 1));

    UNSIGNED_LONGS_EQUAL(3, array.size);
    UNSIGNED_LONGS_EQUAL(oldCapacity, array.capacity);
    POINTERS_EQUAL(oldData, array.data);
}

TEST(DynamicArrayRemoval, MultipleRemovalsPreserveRemainingOrder)
{
    appendValues();

    CHECK_TRUE(da_Remove(&array, 1));
    CHECK_TRUE(da_Remove(&array, 1));

    UNSIGNED_LONGS_EQUAL(2, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(40, values()[1]);
}

TEST(DynamicArrayRemoval, CanAppendAfterRemovingElement)
{
    appendValues();

    CHECK_TRUE(
        da_Remove(&array, 1));

    int value = 50;

    CHECK_TRUE(
        da_Append(&array, &value));

    UNSIGNED_LONGS_EQUAL(4, array.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(30, values()[1]);
    LONGS_EQUAL(40, values()[2]);
    LONGS_EQUAL(50, values()[3]);
}
/* =========================================================
 * Type-info callback tests
 * ========================================================= */

static int printCallCount = 0;
static int printedTotal = 0;
static int destroyCallCount = 0;

static void CountIntPrint(const void *element)
{
    ++printCallCount;
    printedTotal += *static_cast<const int *>(element);
}

static void CountDestroy(void *element)
{
    ++destroyCallCount;
    int **owned = static_cast<int **>(element);
    free(*owned);
    *owned = NULL;
}

TEST_GROUP(DynamicArrayTypeInfo)
{
    DynamicArray_t array;

    void setup()
    {
        array.data = NULL;
        array.size = 0;
        array.capacity = 0;
        array.typeInfo = InvalidTypeInfo();

        printCallCount = 0;
        printedTotal = 0;
        destroyCallCount = 0;
    }

    void teardown()
    {
        da_Destroy(&array);
    }
};

TEST(DynamicArrayTypeInfo, PrintUsesConfiguredPrintFunctionForEveryElement)
{
    ADT_TypeInfo_t info = {
        sizeof(int),
        CompareInt,
        CountIntPrint,
        NULL};

    CHECK_TRUE(da_Init(&array, info));

    int values[] = {10, 20, 30};
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(da_Append(&array, &values[i]));
    }

    da_Print(&array);

    LONGS_EQUAL(3, printCallCount);
    LONGS_EQUAL(60, printedTotal);
}

TEST(DynamicArrayTypeInfo, DestroyCallsConfiguredDestructorForEveryElement)
{
    ADT_TypeInfo_t info = {
        sizeof(int *),
        NULL,
        NULL,
        CountDestroy};

    CHECK_TRUE(da_Init(&array, info));

    for (int i = 0; i < 3; ++i)
    {
        int *value = static_cast<int *>(malloc(sizeof(int)));
        CHECK(value != NULL);
        *value = i;
        CHECK_TRUE(da_Append(&array, &value));
    }

    da_Destroy(&array);

    LONGS_EQUAL(3, destroyCallCount);
    POINTERS_EQUAL(NULL, array.data);
    UNSIGNED_LONGS_EQUAL(0, array.typeInfo.elementSize);
}

/* =========================================================
 * Access, lookup, and state-query tests
 * ========================================================= */

TEST_GROUP(DynamicArrayAccess)
{
    DynamicArray_t array;

    void setup()
    {
        array = DynamicArray_t{};
        CHECK_TRUE(da_Init(&array, IntTypeInfo()));
    }

    void teardown()
    {
        da_Destroy(&array);
    }

    void appendThreeValues()
    {
        int values[] = {10, 20, 30};
        for (size_t i = 0; i < 3; ++i)
        {
            CHECK_TRUE(da_Append(&array, &values[i]));
        }
    }
};

TEST(DynamicArrayAccess, SizeAndIsEmptyReflectCurrentState)
{
    UNSIGNED_LONGS_EQUAL(0, da_Size(&array));
    CHECK_TRUE(da_IsEmpty(&array));

    int value = 10;
    CHECK_TRUE(da_Append(&array, &value));

    UNSIGNED_LONGS_EQUAL(1, da_Size(&array));
    CHECK_FALSE(da_IsEmpty(&array));
}

TEST(DynamicArrayAccess, NullArrayHasSizeZeroAndIsEmpty)
{
    UNSIGNED_LONGS_EQUAL(0, da_Size(NULL));
    CHECK_TRUE(da_IsEmpty(NULL));
}

TEST(DynamicArrayAccess, GetCopiesElementAtIndex)
{
    appendThreeValues();

    int result = 0;
    CHECK_TRUE(da_Get(&array, 1, &result));
    LONGS_EQUAL(20, result);
}

TEST(DynamicArrayAccess, GetRejectsInvalidArguments)
{
    appendThreeValues();

    int result = 0;
    CHECK_FALSE(da_Get(NULL, 0, &result));
    CHECK_FALSE(da_Get(&array, 0, NULL));
    CHECK_FALSE(da_Get(&array, array.size, &result));
}

TEST(DynamicArrayAccess, SetReplacesElementAtIndex)
{
    appendThreeValues();

    int replacement = 99;
    CHECK_TRUE(da_Set(&array, 1, &replacement));

    int result = 0;
    CHECK_TRUE(da_Get(&array, 1, &result));
    LONGS_EQUAL(99, result);
    UNSIGNED_LONGS_EQUAL(3, array.size);
}

TEST(DynamicArrayAccess, SetRejectsInvalidArguments)
{
    appendThreeValues();

    int replacement = 99;
    CHECK_FALSE(da_Set(NULL, 0, &replacement));
    CHECK_FALSE(da_Set(&array, 0, NULL));
    CHECK_FALSE(da_Set(&array, array.size, &replacement));
}

TEST(DynamicArrayAccess, IndexOfFindsFirstMatchingElement)
{
    int values[] = {10, 20, 10};
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(da_Append(&array, &values[i]));
    }

    int target = 10;
    size_t index = 999;

    CHECK_TRUE(da_IndexOf(&array, &target, &index));
    UNSIGNED_LONGS_EQUAL(0, index);
}

TEST(DynamicArrayAccess, IndexOfReturnsFalseWhenElementIsAbsent)
{
    appendThreeValues();

    int target = 99;
    size_t index = 123;

    CHECK_FALSE(da_IndexOf(&array, &target, &index));
    UNSIGNED_LONGS_EQUAL(123, index);
}

TEST(DynamicArrayAccess, IndexOfRejectsInvalidArguments)
{
    int target = 10;
    size_t index = 0;

    CHECK_FALSE(da_IndexOf(NULL, &target, &index));
    CHECK_FALSE(da_IndexOf(&array, NULL, &index));
    CHECK_FALSE(da_IndexOf(&array, &target, NULL));
}

TEST(DynamicArrayAccess, ContainsUsesConfiguredComparator)
{
    appendThreeValues();

    int present = 20;
    int absent = 99;

    CHECK_TRUE(da_Contains(&array, &present));
    CHECK_FALSE(da_Contains(&array, &absent));
    CHECK_FALSE(da_Contains(NULL, &present));
    CHECK_FALSE(da_Contains(&array, NULL));
}

TEST(DynamicArrayAccess, IndexOfFallsBackToByteComparisonWithoutComparator)
{
    da_Destroy(&array);

    ADT_TypeInfo_t info = {
        sizeof(int),
        NULL,
        PrintInt,
        NULL};

    CHECK_TRUE(da_Init(&array, info));

    int values[] = {7, 8, 9};
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(da_Append(&array, &values[i]));
    }

    int target = 8;
    size_t index = 0;

    CHECK_TRUE(da_IndexOf(&array, &target, &index));
    UNSIGNED_LONGS_EQUAL(1, index);
}

/* =========================================================
 * Print and debug-output tests
 * ========================================================= */

static std::string CaptureStdout(void (*action)(void *), void *context)
{
    fflush(stdout);

    FILE *capture = tmpfile();
    CHECK(capture != NULL);

    int savedStdout = dup(fileno(stdout));
    CHECK(savedStdout >= 0);
    CHECK(dup2(fileno(capture), fileno(stdout)) >= 0);

    action(context);
    fflush(stdout);

    CHECK(dup2(savedStdout, fileno(stdout)) >= 0);
    close(savedStdout);

    CHECK(fseek(capture, 0, SEEK_END) == 0);
    long length = ftell(capture);
    CHECK(length >= 0);
    CHECK(fseek(capture, 0, SEEK_SET) == 0);

    std::string output(static_cast<size_t>(length), '\0');
    if (length > 0)
    {
        size_t bytesRead = fread(&output[0], 1, static_cast<size_t>(length), capture);
        UNSIGNED_LONGS_EQUAL(static_cast<size_t>(length), bytesRead);
    }

    fclose(capture);
    return output;
}

static void PrintArrayAction(void *context)
{
    da_Print(static_cast<const DynamicArray_t *>(context));
}

struct DebugPrintContext
{
    const DynamicArray_t *array;
    const char *expression;
    const char *file;
    int line;
};

static void DebugPrintAction(void *context)
{
    DebugPrintContext *debug = static_cast<DebugPrintContext *>(context);
    da_PrintDebug(debug->array, debug->expression, debug->file, debug->line);
}

TEST_GROUP(DynamicArrayPrinting)
{
    DynamicArray_t array;

    void setup()
    {
        array = DynamicArray_t{};
        printCallCount = 0;
        printedTotal = 0;
    }

    void teardown()
    {
        da_Destroy(&array);
    }
};

TEST(DynamicArrayPrinting, PrintIncludesMetadataAndElements)
{
    CHECK_TRUE(da_Init(&array, IntTypeInfo()));

    int values[] = {10, 20, 30};
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(da_Append(&array, &values[i]));
    }

    std::string output = CaptureStdout(PrintArrayAction, &array);

    CHECK(output.find("DynamicArray (size: 3, capacity: 8): [10, 20, 30]") != std::string::npos);
}

TEST(DynamicArrayPrinting, PrintHandlesNullArray)
{
    std::string output = CaptureStdout(PrintArrayAction, NULL);
    CHECK(output.find("DynamicArray is NULL") != std::string::npos);
}

TEST(DynamicArrayPrinting, PrintHandlesMissingPrinter)
{
    ADT_TypeInfo_t info = {sizeof(int), CompareInt, NULL, NULL};
    CHECK_TRUE(da_Init(&array, info));

    std::string output = CaptureStdout(PrintArrayAction, &array);
    CHECK(output.find("print function is NULL") != std::string::npos);
}

TEST(DynamicArrayPrinting, DebugPrintIncludesExpressionLocationAndArrayState)
{
    CHECK_TRUE(da_Init(&array, IntTypeInfo()));

    int values[] = {4, 8};
    CHECK_TRUE(da_Append(&array, &values[0]));
    CHECK_TRUE(da_Append(&array, &values[1]));

    DebugPrintContext context = {&array, "numbers", "example.c", 42};
    std::string output = CaptureStdout(DebugPrintAction, &context);

    CHECK(output.find("DynamicArray Debug") != std::string::npos);
    CHECK(output.find("expression: numbers") != std::string::npos);
    CHECK(output.find("location: example.c:42") != std::string::npos);
    CHECK(output.find("size: 2") != std::string::npos);
    CHECK(output.find("capacity: 8") != std::string::npos);
    CHECK(output.find("element size: ") != std::string::npos);
    CHECK(output.find("comparator: set") != std::string::npos);
    CHECK(output.find("printer: set") != std::string::npos);
    CHECK(output.find("destructor: NULL") != std::string::npos);
    CHECK(output.find("elements: [4, 8]") != std::string::npos);
}

TEST(DynamicArrayPrinting, DebugPrintHandlesNullArray)
{
    DebugPrintContext context = {NULL, "array", "test.c", 7};
    std::string output = CaptureStdout(DebugPrintAction, &context);

    CHECK(output.find("expression: array") != std::string::npos);
    CHECK(output.find("location: test.c:7") != std::string::npos);
    CHECK(output.find("state: NULL") != std::string::npos);
}

TEST(DynamicArrayPrinting, DebugPrintReportsMissingStorage)
{
    array = DynamicArray_t{};
    array.typeInfo = IntTypeInfo();

    DebugPrintContext context = {&array, "array", "test.c", 9};
    std::string output = CaptureStdout(DebugPrintAction, &context);

    CHECK(output.find("data: (nil)") != std::string::npos ||
          output.find("data: 0x0") != std::string::npos);
    CHECK(output.find("elements: <no storage>") != std::string::npos);
}

TEST(DynamicArrayPrinting, DebugPrintReportsMissingPrinter)
{
    ADT_TypeInfo_t info = {sizeof(int), CompareInt, NULL, NULL};
    CHECK_TRUE(da_Init(&array, info));

    DebugPrintContext context = {&array, "array", "test.c", 11};
    std::string output = CaptureStdout(DebugPrintAction, &context);

    CHECK(output.find("printer: NULL") != std::string::npos);
    CHECK(output.find("elements: <no print function>") != std::string::npos);
}

TEST(DynamicArrayPrinting, DebugPrintUsesConfiguredPrinterForEveryElement)
{
    ADT_TypeInfo_t info = {sizeof(int), CompareInt, CountIntPrint, NULL};
    CHECK_TRUE(da_Init(&array, info));

    int values[] = {2, 3, 5};
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(da_Append(&array, &values[i]));
    }

    DebugPrintContext context = {&array, "values", "test.c", 13};
    CaptureStdout(DebugPrintAction, &context);

    LONGS_EQUAL(3, printCallCount);
    LONGS_EQUAL(10, printedTotal);
}
