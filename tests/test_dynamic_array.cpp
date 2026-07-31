extern "C"
{
#include "libadt/dynamic_array.h"
#include "libadt/element/comparators.h"
}

#include "CppUTest/TestHarness.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <string>
#include <unistd.h>

#define INITIAL_CAPACITY 8

extern "C"
{
    bool C23UnknownTypeUsesSafeDefaults(void);
    bool C23InitFromInfersIntType(void);
}

static ADT_ElementTypeInfo_t IntTypeInfo()
{
    ADT_ElementTypeInfo_t info = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    return info;
}

static ADT_ElementTypeInfo_t DoubleTypeInfo()
{
    ADT_ElementTypeInfo_t info = {sizeof(double), CompareDouble, PrintDouble, ToNumberDouble, NULL};
    return info;
}

static ADT_ElementTypeInfo_t InvalidTypeInfo()
{
    ADT_ElementTypeInfo_t info = {0, NULL, NULL, NULL, NULL};
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
        array._private.data = NULL;
        array.super._private.size = 0;
        array._private.capacity = 0;
        array.super._private.elementType = InvalidTypeInfo();
    }

    void teardown()
    {
        da_Destroy(&array);
    }
};

TEST(DynamicArrayInitialization, InitCreatesEmptyArray)
{
    CHECK_TRUE(da_Init(&array, IntTypeInfo()));

    CHECK(array._private.data != NULL);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(INITIAL_CAPACITY, array._private.capacity);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.super._private.elementType.elementSize);
}

TEST(DynamicArrayInitialization, InitFromCopiesIntegerElements)
{
    int input[] = {10, 20, 30, 40};

    CHECK_TRUE(da_InitFrom(&array, input, 4, IntTypeInfo()));

    int *stored = static_cast<int *>(array._private.data);

    UNSIGNED_LONGS_EQUAL(4, array.super._private.size);
    CHECK(array._private.capacity >= array.super._private.size);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.super._private.elementType.elementSize);

    LONGS_EQUAL(10, stored[0]);
    LONGS_EQUAL(20, stored[1]);
    LONGS_EQUAL(30, stored[2]);
    LONGS_EQUAL(40, stored[3]);
}

TEST(DynamicArrayInitialization, InitFromMakesIndependentCopy)
{
    int input[] = {10, 20, 30};

    CHECK_TRUE(da_InitFrom(&array, input, 3, IntTypeInfo()));

    CHECK(array._private.data != input);

    input[0] = 999;

    int *stored = static_cast<int *>(array._private.data);

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

    UNSIGNED_LONGS_EQUAL(20, array.super._private.size);
    CHECK(array._private.capacity >= array.super._private.size);
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

    int *stored = static_cast<int *>(array._private.data);

    UNSIGNED_LONGS_EQUAL(count, array.super._private.size);
    CHECK(array._private.capacity >= count);

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

    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
    CHECK(array._private.capacity >= INITIAL_CAPACITY);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.super._private.elementType.elementSize);
}

TEST(DynamicArrayInitialization, InitRejectsNullArray)
{
    CHECK_FALSE(da_Init(NULL, IntTypeInfo()));
}

TEST(DynamicArrayInitialization, InitRejectsZeroElementSize)
{
    CHECK_FALSE(da_Init(&array, InvalidTypeInfo()));

    POINTERS_EQUAL(NULL, array._private.data);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(0, array._private.capacity);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.elementType.elementSize);
}

TEST(DynamicArrayInitialization, InitRejectsElementSizeOverflow)
{
    ADT_ElementTypeInfo_t elementType = {
        static_cast<size_t>(-1),
        NULL,
        NULL,
        NULL,
        NULL};

    CHECK_FALSE(da_Init(&array, elementType));
    POINTERS_EQUAL(NULL, array._private.data);
}

TEST(DynamicArrayInitialization, InitFromRejectsNullArray)
{
    CHECK_FALSE(da_InitFrom(&array, NULL, 3, IntTypeInfo()));
}

TEST(DynamicArrayInitialization, InitFromRejectsNullElementsWithCount)
{
    CHECK_FALSE(da_InitFrom(&array, NULL, 3, IntTypeInfo()));

    POINTERS_EQUAL(NULL, array._private.data);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(0, array._private.capacity);
}

TEST(DynamicArrayInitialization, InitFromRejectsCapacityOverflow)
{
    int value = 1;

    CHECK_FALSE(da_InitFrom(
        &array,
        &value,
        static_cast<size_t>(-1),
        IntTypeInfo()));

    POINTERS_EQUAL(NULL, array._private.data);
}

TEST(DynamicArrayInitialization, InitFromSupportsDoubleElements)
{
    double input[] = {1.5, 2.5, 3.5};

    CHECK_TRUE(da_InitFrom(&array, input, 3, DoubleTypeInfo()));

    double *stored = static_cast<double *>(array._private.data);

    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(sizeof(double), array.super._private.elementType.elementSize);

    DOUBLES_EQUAL(1.5, stored[0], 0.000001);
    DOUBLES_EQUAL(2.5, stored[1], 0.000001);
    DOUBLES_EQUAL(3.5, stored[2], 0.000001);
}

TEST(DynamicArrayInitialization, DestroyResetsArray)
{
    CHECK_TRUE(da_Init(&array, IntTypeInfo()));

    da_Destroy(&array);

    POINTERS_EQUAL(NULL, array._private.data);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(0, array._private.capacity);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.elementType.elementSize);
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

    POINTERS_EQUAL(NULL, array._private.data);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(0, array._private.capacity);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.elementType.elementSize);
}

/* =========================================================
 * Integer insertion tests
 * ========================================================= */

TEST_GROUP(DynamicArrayInsertion)
{
    DynamicArray_t array;

    void setup()
    {
        array._private.data = NULL;
        array.super._private.size = 0;
        array._private.capacity = 0;
        array.super._private.elementType = InvalidTypeInfo();

        CHECK_TRUE(da_Init(&array, IntTypeInfo()));
    }

    void teardown()
    {
        da_Destroy(&array);
    }

    int *values()
    {
        return static_cast<int *>(array._private.data);
    }
};

TEST(DynamicArrayInsertion, AppendAddsElementToEmptyArray)
{
    int value = 10;

    CHECK_TRUE(
        da_detail_AppendRef(&array, &value));

    UNSIGNED_LONGS_EQUAL(1, array.super._private.size);
    LONGS_EQUAL(10, values()[0]);
}

TEST(DynamicArrayInsertion, AppendPreservesInsertionOrder)
{
    int first = 10;
    int second = 20;
    int third = 30;

    CHECK_TRUE(da_detail_AppendRef(&array, &first));
    CHECK_TRUE(da_detail_AppendRef(&array, &second));
    CHECK_TRUE(da_detail_AppendRef(&array, &third));

    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
    LONGS_EQUAL(30, values()[2]);
}

TEST(DynamicArrayInsertion, PrependAddsElementToEmptyArray)
{
    int value = 10;

    CHECK_TRUE(
        da_detail_PrependRef(&array, &value));

    UNSIGNED_LONGS_EQUAL(1, array.super._private.size);
    LONGS_EQUAL(10, values()[0]);
}

TEST(DynamicArrayInsertion, PrependShiftsExistingElementsRight)
{
    int twenty = 20;
    int thirty = 30;
    int ten = 10;

    CHECK_TRUE(da_detail_AppendRef(&array, &twenty));
    CHECK_TRUE(da_detail_AppendRef(&array, &thirty));
    CHECK_TRUE(da_detail_PrependRef(&array, &ten));

    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
    LONGS_EQUAL(30, values()[2]);
}

TEST(DynamicArrayInsertion, InsertIntoEmptyArrayAtZeroSucceeds)
{
    int value = 10;

    CHECK_TRUE(
        da_detail_InsertRef(&array, 0, &value));

    UNSIGNED_LONGS_EQUAL(1, array.super._private.size);
    LONGS_EQUAL(10, values()[0]);
}

TEST(DynamicArrayInsertion, InsertAddsElementInMiddle)
{
    int ten = 10;
    int thirty = 30;
    int twenty = 20;

    CHECK_TRUE(da_detail_AppendRef(&array, &ten));
    CHECK_TRUE(da_detail_AppendRef(&array, &thirty));
    CHECK_TRUE(da_detail_InsertRef(&array, 1, &twenty));

    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
    LONGS_EQUAL(30, values()[2]);
}

TEST(DynamicArrayInsertion, InsertAtZeroBehavesLikePrepend)
{
    int twenty = 20;
    int ten = 10;

    CHECK_TRUE(da_detail_AppendRef(&array, &twenty));
    CHECK_TRUE(da_detail_InsertRef(&array, 0, &ten));

    UNSIGNED_LONGS_EQUAL(2, array.super._private.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
}

TEST(DynamicArrayInsertion, InsertAtSizeBehavesLikeAppend)
{
    int ten = 10;
    int twenty = 20;

    CHECK_TRUE(da_detail_AppendRef(&array, &ten));
    CHECK_TRUE(da_detail_InsertRef(&array, array.super._private.size, &twenty));

    UNSIGNED_LONGS_EQUAL(2, array.super._private.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
}

TEST(DynamicArrayInsertion, InsertBeyondSizeFails)
{
    int value = 10;

    CHECK_FALSE(
        da_detail_InsertRef(&array, 4, &value));

    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
}

TEST(DynamicArrayInsertion, AppendRejectsSizeOverflow)
{
    unsigned char storage = 0;
    ADT_ElementTypeInfo_t elementType = {1, NULL, NULL, NULL, NULL};
    DynamicArray_t fullArray = {
        .super = {
            ._private = {
                .vtable = nullptr,
                .size = static_cast<size_t>(-1),
                .elementType = elementType}},
        ._private = {
            .data = &storage,
            .capacity = static_cast<size_t>(-1)}};
    unsigned char value = 1;

    CHECK_FALSE(da_detail_AppendRef(&fullArray, &value));
}

TEST(DynamicArrayInsertion, AppendRejectsNullArray)
{
    int value = 10;

    CHECK_FALSE(
        da_detail_AppendRef(NULL, &value));
}

TEST(DynamicArrayInsertion, PrependRejectsNullArray)
{
    int value = 10;

    CHECK_FALSE(
        da_detail_PrependRef(NULL, &value));
}

TEST(DynamicArrayInsertion, InsertRejectsNullArray)
{
    int value = 10;

    CHECK_FALSE(
        da_detail_InsertRef(NULL, 0, &value));
}

TEST(DynamicArrayInsertion, AppendRejectsNullElement)
{
    CHECK_FALSE(
        da_detail_AppendRef(&array, NULL));

    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
}

TEST(DynamicArrayInsertion, PrependRejectsNullElement)
{
    CHECK_FALSE(
        da_detail_PrependRef(&array, NULL));

    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
}

TEST(DynamicArrayInsertion, InsertRejectsNullElement)
{
    CHECK_FALSE(
        da_detail_InsertRef(&array, 0, NULL));

    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
}

TEST(DynamicArrayInsertion, AppendExpandsCapacity)
{
    size_t originalCapacity = array._private.capacity;

    for (size_t i = 0; i <= originalCapacity; i++)
    {
        int value = static_cast<int>(i);

        CHECK_TRUE(
            da_detail_AppendRef(&array, &value));
    }

    UNSIGNED_LONGS_EQUAL(
        originalCapacity + 1,
        array.super._private.size);

    CHECK(array._private.capacity > originalCapacity);
    CHECK(array._private.capacity >= array.super._private.size);

    for (size_t i = 0; i < array.super._private.size; i++)
    {
        LONGS_EQUAL(
            static_cast<long>(i),
            values()[i]);
    }
}

TEST(DynamicArrayInsertion, PrependWhileFullExpandsAndPreservesData)
{
    size_t originalCapacity = array._private.capacity;

    for (size_t i = 0; i < originalCapacity; i++)
    {
        int value = static_cast<int>(i + 1);

        CHECK_TRUE(
            da_detail_AppendRef(&array, &value));
    }

    int prepended = 0;

    CHECK_TRUE(
        da_detail_PrependRef(&array, &prepended));

    UNSIGNED_LONGS_EQUAL(
        originalCapacity + 1,
        array.super._private.size);

    CHECK(array._private.capacity > originalCapacity);

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
    size_t originalCapacity = array._private.capacity;

    for (size_t i = 0; i < originalCapacity; i++)
    {
        int value = static_cast<int>(i);

        CHECK_TRUE(
            da_detail_AppendRef(&array, &value));
    }

    int inserted = 999;

    CHECK_TRUE(
        da_detail_InsertRef(&array, 2, &inserted));

    UNSIGNED_LONGS_EQUAL(
        originalCapacity + 1,
        array.super._private.size);

    CHECK(array._private.capacity > originalCapacity);

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

TEST(DynamicArrayInsertion, InsertCopiesAnElementAlreadyStoredInTheArray)
{
    int initial[] = {10, 20, 30};

    da_Destroy(&array);
    CHECK_TRUE(da_InitFrom(&array, initial, 3, IntTypeInfo()));

    int *stored = values();
    CHECK_TRUE(da_detail_InsertRef(&array, 0, &stored[2]));

    int expected[] = {30, 10, 20, 30};
    for (size_t i = 0; i < ARRAY_COUNT(expected); i++)
    {
        LONGS_EQUAL(expected[i], values()[i]);
    }
}

TEST(DynamicArrayInsertion, InsertPreservesInternalSourceAcrossReallocation)
{
    const size_t initialCapacity = array._private.capacity;

    for (size_t i = 0; i < initialCapacity; i++)
    {
        int value = static_cast<int>(i + 1);
        CHECK_TRUE(da_detail_AppendRef(&array, &value));
    }

    int *stored = values();
    int expectedSource = stored[initialCapacity - 1];

    CHECK_TRUE(da_detail_InsertRef(&array, 0, &stored[initialCapacity - 1]));
    LONGS_EQUAL(expectedSource, values()[0]);

    for (size_t i = 0; i < initialCapacity; i++)
    {
        LONGS_EQUAL(static_cast<long>(i + 1), values()[i + 1]);
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
        array._private.data = NULL;
        array.super._private.size = 0;
        array._private.capacity = 0;
        array.super._private.elementType = InvalidTypeInfo();

        CHECK_TRUE(da_Init(&array, DoubleTypeInfo()));
    }

    void teardown()
    {
        da_Destroy(&array);
    }

    double *values()
    {
        return static_cast<double *>(array._private.data);
    }
};

TEST(DynamicArrayDoubleInsertion, AppendSupportsDoubleValues)
{
    double first = 1.5;
    double second = 2.5;

    CHECK_TRUE(da_detail_AppendRef(&array, &first));
    CHECK_TRUE(da_detail_AppendRef(&array, &second));

    UNSIGNED_LONGS_EQUAL(2, array.super._private.size);

    DOUBLES_EQUAL(1.5, values()[0], 0.000001);
    DOUBLES_EQUAL(2.5, values()[1], 0.000001);
}

TEST(DynamicArrayDoubleInsertion, PrependSupportsDoubleValues)
{
    double second = 2.5;
    double first = 1.5;

    CHECK_TRUE(da_detail_AppendRef(&array, &second));
    CHECK_TRUE(da_detail_PrependRef(&array, &first));

    UNSIGNED_LONGS_EQUAL(2, array.super._private.size);

    DOUBLES_EQUAL(1.5, values()[0], 0.000001);
    DOUBLES_EQUAL(2.5, values()[1], 0.000001);
}

TEST(DynamicArrayDoubleInsertion, InsertSupportsDoubleValues)
{
    double first = 1.5;
    double third = 3.5;
    double second = 2.5;

    CHECK_TRUE(da_detail_AppendRef(&array, &first));
    CHECK_TRUE(da_detail_AppendRef(&array, &third));
    CHECK_TRUE(da_detail_InsertRef(&array, 1, &second));

    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);

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
        array._private.data = NULL;
        array.super._private.size = 0;
        array._private.capacity = 0;
        array.super._private.elementType = InvalidTypeInfo();

        CHECK_TRUE(da_Init(&array, IntTypeInfo()));
    }

    void teardown()
    {
        da_Destroy(&array);
    }

    int *values()
    {
        return static_cast<int *>(array._private.data);
    }

    void appendValues()
    {
        int first = 10;
        int second = 20;
        int third = 30;
        int fourth = 40;

        CHECK_TRUE(da_detail_AppendRef(&array, &first));
        CHECK_TRUE(da_detail_AppendRef(&array, &second));
        CHECK_TRUE(da_detail_AppendRef(&array, &third));
        CHECK_TRUE(da_detail_AppendRef(&array, &fourth));
    }
};

TEST(DynamicArrayRemoval, RemoveFirstElement)
{
    appendValues();

    CHECK_TRUE(
        da_Remove(&array, 0));

    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);

    LONGS_EQUAL(20, values()[0]);
    LONGS_EQUAL(30, values()[1]);
    LONGS_EQUAL(40, values()[2]);
}

TEST(DynamicArrayRemoval, RemoveMiddleElement)
{
    appendValues();

    CHECK_TRUE(
        da_Remove(&array, 1));

    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(30, values()[1]);
    LONGS_EQUAL(40, values()[2]);
}

TEST(DynamicArrayRemoval, RemoveLastElement)
{
    appendValues();

    CHECK_TRUE(
        da_Remove(&array, 3));

    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(20, values()[1]);
    LONGS_EQUAL(30, values()[2]);
}

TEST(DynamicArrayRemoval, RemoveOnlyElement)
{
    int value = 10;

    CHECK_TRUE(
        da_detail_AppendRef(&array, &value));

    CHECK_TRUE(
        da_Remove(&array, 0));

    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
    CHECK_TRUE(adt_IsEmpty(&array));
}

TEST(DynamicArrayRemoval, RemoveFromEmptyArrayFails)
{
    CHECK_FALSE(
        da_Remove(&array, 0));

    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
}

TEST(DynamicArrayRemoval, RemoveIndexEqualToSizeFails)
{
    appendValues();

    CHECK_FALSE(
        da_Remove(&array, array.super._private.size));

    UNSIGNED_LONGS_EQUAL(4, array.super._private.size);

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

    UNSIGNED_LONGS_EQUAL(4, array.super._private.size);

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

    size_t oldCapacity = array._private.capacity;
    void *oldData = array._private.data;

    da_Clear(&array);

    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(oldCapacity, array._private.capacity);
    POINTERS_EQUAL(oldData, array._private.data);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.super._private.elementType.elementSize);

    CHECK_TRUE(
        adt_IsEmpty(&array));
}

TEST(DynamicArrayRemoval, ClearEmptyArrayIsSafe)
{
    size_t oldCapacity = array._private.capacity;
    void *oldData = array._private.data;

    da_Clear(&array);

    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(oldCapacity, array._private.capacity);
    POINTERS_EQUAL(oldData, array._private.data);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.super._private.elementType.elementSize);
}

TEST(DynamicArrayRemoval, ClearHandlesNullArray)
{
    da_Clear(NULL);
}

TEST(DynamicArrayRemoval, ArrayCanBeReusedAfterClear)
{
    appendValues();

    size_t oldCapacity = array._private.capacity;
    void *oldData = array._private.data;

    da_Clear(&array);

    int newValue = 99;

    CHECK_TRUE(
        da_detail_AppendRef(&array, &newValue));

    UNSIGNED_LONGS_EQUAL(1, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(oldCapacity, array._private.capacity);
    POINTERS_EQUAL(oldData, array._private.data);
    LONGS_EQUAL(99, values()[0]);
}

TEST(DynamicArrayRemoval, RemoveDoesNotChangeCapacity)
{
    appendValues();

    size_t oldCapacity = array._private.capacity;
    void *oldData = array._private.data;

    CHECK_TRUE(
        da_Remove(&array, 1));

    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(oldCapacity, array._private.capacity);
    POINTERS_EQUAL(oldData, array._private.data);
}

TEST(DynamicArrayRemoval, MultipleRemovalsPreserveRemainingOrder)
{
    appendValues();

    CHECK_TRUE(da_Remove(&array, 1));
    CHECK_TRUE(da_Remove(&array, 1));

    UNSIGNED_LONGS_EQUAL(2, array.super._private.size);

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
        da_detail_AppendRef(&array, &value));

    UNSIGNED_LONGS_EQUAL(4, array.super._private.size);

    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(30, values()[1]);
    LONGS_EQUAL(40, values()[2]);
    LONGS_EQUAL(50, values()[3]);
}

TEST(DynamicArrayRemoval, TakeRemovesElementAndReturnsItsValue)
{
    appendValues();
    int taken = 0;

    CHECK_TRUE(da_Take(&array, 1, &taken));

    LONGS_EQUAL(20, taken);
    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);
    LONGS_EQUAL(10, values()[0]);
    LONGS_EQUAL(30, values()[1]);
    LONGS_EQUAL(40, values()[2]);
}

TEST(DynamicArrayRemoval, TakeRejectsInvalidArgumentsAndContainerStorage)
{
    appendValues();
    int output = 0;

    CHECK_FALSE(da_Take(NULL, 0, &output));
    CHECK_FALSE(da_Take(&array, 0, NULL));
    CHECK_FALSE(da_Take(&array, array.super._private.size, &output));
    CHECK_FALSE(da_Take(&array, 0, array._private.data));
    UNSIGNED_LONGS_EQUAL(4, array.super._private.size);
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
        array._private.data = NULL;
        array.super._private.size = 0;
        array._private.capacity = 0;
        array.super._private.elementType = InvalidTypeInfo();

        printCallCount = 0;
        printedTotal = 0;
        destroyCallCount = 0;
    }

    void teardown()
    {
        da_Destroy(&array);
    }
};

TEST(DynamicArrayTypeInfo, DestroyCallsConfiguredDestructorForEveryElement)
{
    ADT_ElementTypeInfo_t info = {
        sizeof(int *),
        NULL,
        NULL,
        NULL,
        CountDestroy};

    CHECK_TRUE(da_Init(&array, info));

    for (int i = 0; i < 3; ++i)
    {
        int *value = static_cast<int *>(malloc(sizeof(int)));
        CHECK(value != NULL);
        *value = i;
        CHECK_TRUE(da_detail_AppendRef(&array, &value));
    }

    da_Destroy(&array);

    LONGS_EQUAL(3, destroyCallCount);
    POINTERS_EQUAL(NULL, array._private.data);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.elementType.elementSize);
}

TEST(DynamicArrayTypeInfo, SetDestroysReplacedElement)
{
    ADT_ElementTypeInfo_t info = {
        sizeof(int *),
        NULL,
        NULL,
        NULL,
        CountDestroy};

    CHECK_TRUE(da_Init(&array, info));

    int *original = static_cast<int *>(malloc(sizeof(int)));
    int *replacement = static_cast<int *>(malloc(sizeof(int)));
    CHECK(original != NULL);
    CHECK(replacement != NULL);

    *original = 10;
    *replacement = 20;

    CHECK_TRUE(da_detail_AppendRef(&array, &original));
    CHECK_TRUE(da_detail_SetRef(&array, 0, &replacement));

    LONGS_EQUAL(1, destroyCallCount);
    POINTERS_EQUAL(replacement, static_cast<int **>(array._private.data)[0]);

    da_Destroy(&array);
    LONGS_EQUAL(2, destroyCallCount);
}

TEST(DynamicArrayTypeInfo, SetFromSameElementDoesNotDestroyIt)
{
    ADT_ElementTypeInfo_t info = {
        sizeof(int *),
        NULL,
        NULL,
        NULL,
        CountDestroy};

    CHECK_TRUE(da_Init(&array, info));

    int *value = static_cast<int *>(malloc(sizeof(int)));
    CHECK(value != NULL);
    *value = 10;

    CHECK_TRUE(da_detail_AppendRef(&array, &value));
    CHECK_TRUE(da_detail_SetRef(&array, 0, array._private.data));

    LONGS_EQUAL(0, destroyCallCount);

    da_Destroy(&array);
    LONGS_EQUAL(1, destroyCallCount);
}

TEST(DynamicArrayTypeInfo, RemoveDestroysRemovedElementOnly)
{
    ADT_ElementTypeInfo_t info = {
        sizeof(int *),
        NULL,
        NULL,
        NULL,
        CountDestroy};

    CHECK_TRUE(da_Init(&array, info));

    int *values[3];
    for (int i = 0; i < 3; ++i)
    {
        values[i] = static_cast<int *>(malloc(sizeof(int)));
        CHECK(values[i] != NULL);
        *values[i] = i;
        CHECK_TRUE(da_detail_AppendRef(&array, &values[i]));
    }

    CHECK_TRUE(da_Remove(&array, 1));

    LONGS_EQUAL(1, destroyCallCount);
    UNSIGNED_LONGS_EQUAL(2, array.super._private.size);
    POINTERS_EQUAL(values[0], static_cast<int **>(array._private.data)[0]);
    POINTERS_EQUAL(values[2], static_cast<int **>(array._private.data)[1]);

    da_Destroy(&array);
    LONGS_EQUAL(3, destroyCallCount);
}

TEST(DynamicArrayTypeInfo, TakeTransfersOwnershipWithoutDestroying)
{
    ADT_ElementTypeInfo_t info = {
        sizeof(int *),
        NULL,
        NULL,
        NULL,
        CountDestroy};

    CHECK_TRUE(da_Init(&array, info));

    int *owned = static_cast<int *>(malloc(sizeof(int)));
    CHECK(owned != NULL);
    *owned = 42;
    CHECK_TRUE(da_detail_AppendRef(&array, &owned));

    int *taken = NULL;
    CHECK_TRUE(da_Take(&array, 0, &taken));

    LONGS_EQUAL(0, destroyCallCount);
    POINTERS_EQUAL(owned, taken);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);

    free(taken);
}

TEST(DynamicArrayTypeInfo, ClearDestroysElementsAndRetainsStorage)
{
    ADT_ElementTypeInfo_t info = {
        sizeof(int *),
        NULL,
        NULL,
        NULL,
        CountDestroy};

    CHECK_TRUE(da_Init(&array, info));

    for (int i = 0; i < 3; ++i)
    {
        int *value = static_cast<int *>(malloc(sizeof(int)));
        CHECK(value != NULL);
        *value = i;
        CHECK_TRUE(da_detail_AppendRef(&array, &value));
    }

    void *const storage = array._private.data;
    const size_t capacity = array._private.capacity;

    da_Clear(&array);

    LONGS_EQUAL(3, destroyCallCount);
    UNSIGNED_LONGS_EQUAL(0, array.super._private.size);
    UNSIGNED_LONGS_EQUAL(capacity, array._private.capacity);
    POINTERS_EQUAL(storage, array._private.data);

    da_Destroy(&array);
    LONGS_EQUAL(3, destroyCallCount);
}

/* =========================================================
 * Built-in callback and C23 macro tests
 * ========================================================= */

TEST_GROUP(BuiltInTypeSupport){};

TEST(BuiltInTypeSupport, FloatComparatorsOrderNaNAfterNumbers)
{
    float floatValue = 1.0F;
    float floatNaN = std::numeric_limits<float>::quiet_NaN();
    double doubleValue = 1.0;
    double doubleNaN = std::numeric_limits<double>::quiet_NaN();

    LONGS_EQUAL(1, CompareFloat(&floatNaN, &floatValue));
    LONGS_EQUAL(-1, CompareFloat(&floatValue, &floatNaN));
    LONGS_EQUAL(0, CompareFloat(&floatNaN, &floatNaN));

    LONGS_EQUAL(1, CompareDouble(&doubleNaN, &doubleValue));
    LONGS_EQUAL(-1, CompareDouble(&doubleValue, &doubleNaN));
    LONGS_EQUAL(0, CompareDouble(&doubleNaN, &doubleNaN));
}

TEST(BuiltInTypeSupport, PointerComparatorProvidesStableAddressOrder)
{
    int firstValue = 1;
    int secondValue = 2;
    void *first = &firstValue;
    void *firstCopy = &firstValue;
    void *second = &secondValue;

    LONGS_EQUAL(0, ComparePointer(&first, &firstCopy));

    const int forward = ComparePointer(&first, &second);
    const int reverse = ComparePointer(&second, &first);

    CHECK(forward == -1 || forward == 1);
    LONGS_EQUAL(-forward, reverse);
}

TEST(BuiltInTypeSupport, UnknownTypeMacrosUseSafeDefaults)
{
    CHECK_TRUE(C23UnknownTypeUsesSafeDefaults());
}

TEST(BuiltInTypeSupport, InitFromMacroInfersBuiltInTypeCallbacks)
{
    CHECK_TRUE(C23InitFromInfersIntType());
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
            CHECK_TRUE(da_detail_AppendRef(&array, &values[i]));
        }
    }
};

TEST(DynamicArrayAccess, SizeAndIsEmptyReflectCurrentState)
{
    UNSIGNED_LONGS_EQUAL(0, adt_Size(&array));
    CHECK_TRUE(adt_IsEmpty(&array));

    int value = 10;
    CHECK_TRUE(da_detail_AppendRef(&array, &value));

    UNSIGNED_LONGS_EQUAL(1, adt_Size(&array));
    CHECK_FALSE(adt_IsEmpty(&array));
}

TEST(DynamicArrayAccess, NullArrayHasSizeZeroAndIsEmpty)
{
    UNSIGNED_LONGS_EQUAL(0, adt_Size(NULL));
    CHECK_TRUE(adt_IsEmpty(NULL));
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
    CHECK_FALSE(da_Get(&array, array.super._private.size, &result));
}

TEST(DynamicArrayAccess, SetReplacesElementAtIndex)
{
    appendThreeValues();

    int replacement = 99;
    CHECK_TRUE(da_detail_SetRef(&array, 1, &replacement));

    int result = 0;
    CHECK_TRUE(da_Get(&array, 1, &result));
    LONGS_EQUAL(99, result);
    UNSIGNED_LONGS_EQUAL(3, array.super._private.size);
}

TEST(DynamicArrayAccess, SetRejectsInvalidArguments)
{
    appendThreeValues();

    int replacement = 99;
    CHECK_FALSE(da_detail_SetRef(NULL, 0, &replacement));
    CHECK_FALSE(da_detail_SetRef(&array, 0, NULL));
    CHECK_FALSE(da_detail_SetRef(&array, array.super._private.size, &replacement));
}

TEST(DynamicArrayAccess, IndexOfFindsFirstMatchingElement)
{
    int values[] = {10, 20, 10};
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(da_detail_AppendRef(&array, &values[i]));
    }

    int target = 10;
    size_t index = 999;

    CHECK_TRUE(da_detail_IndexOfRef(&array, &target, &index));
    UNSIGNED_LONGS_EQUAL(0, index);
}

TEST(DynamicArrayAccess, IndexOfReturnsFalseWhenElementIsAbsent)
{
    appendThreeValues();

    int target = 99;
    size_t index = 123;

    CHECK_FALSE(da_detail_IndexOfRef(&array, &target, &index));
    UNSIGNED_LONGS_EQUAL(123, index);
}

TEST(DynamicArrayAccess, IndexOfRejectsInvalidArguments)
{
    int target = 10;
    size_t index = 0;

    CHECK_FALSE(da_detail_IndexOfRef(NULL, &target, &index));
    CHECK_FALSE(da_detail_IndexOfRef(&array, NULL, &index));
    CHECK_FALSE(da_detail_IndexOfRef(&array, &target, NULL));
}

TEST(DynamicArrayAccess, ContainsUsesConfiguredComparator)
{
    appendThreeValues();

    int present = 20;
    int absent = 99;

    CHECK_TRUE(da_detail_ContainsRef(&array, &present));
    CHECK_FALSE(da_detail_ContainsRef(&array, &absent));
    CHECK_FALSE(da_detail_ContainsRef(NULL, &present));
    CHECK_FALSE(da_detail_ContainsRef(&array, NULL));
}

TEST(DynamicArrayAccess, IndexOfFallsBackToByteComparisonWithoutComparator)
{
    da_Destroy(&array);

    ADT_ElementTypeInfo_t info = {
        sizeof(int),
        NULL,
        PrintInt,
        ToNumberInt,
        NULL};

    CHECK_TRUE(da_Init(&array, info));

    int values[] = {7, 8, 9};
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(da_detail_AppendRef(&array, &values[i]));
    }

    int target = 8;
    size_t index = 0;

    CHECK_TRUE(da_detail_IndexOfRef(&array, &target, &index));
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
    adt_Print(static_cast<const DynamicArray_t *>(context));
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
    adt_PrintDebug(debug->array, debug->expression, debug->file, debug->line);
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

TEST(DynamicArrayPrinting, PrintUsesConfiguredPrintFunctionForEveryElement)
{
    ADT_ElementTypeInfo_t info = {
        sizeof(int),
        CompareInt,
        CountIntPrint,
        ToNumberInt,
        NULL};

    CHECK_TRUE(da_Init(&array, info));

    int values[] = {10, 20, 30};
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(da_detail_AppendRef(&array, &values[i]));
    }

    CaptureStdout(PrintArrayAction, &array);

    LONGS_EQUAL(3, printCallCount);
    LONGS_EQUAL(60, printedTotal);
}

TEST(DynamicArrayPrinting, PrintUsesUnifiedContainerFormat)
{
    CHECK_TRUE(da_Init(&array, IntTypeInfo()));

    int values[] = {10, 20, 30};
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(da_detail_AppendRef(&array, &values[i]));
    }

    std::string output = CaptureStdout(PrintArrayAction, &array);

    STRCMP_EQUAL("DynamicArray (size: 3): [10, 20, 30]\n", output.c_str());
}

TEST(DynamicArrayPrinting, PrintHandlesNullArray)
{
    std::string output = CaptureStdout(PrintArrayAction, NULL);
    CHECK_TRUE(output.empty());
}

TEST(DynamicArrayPrinting, PrintHandlesMissingPrinter)
{
    ADT_ElementTypeInfo_t info = {sizeof(int), CompareInt, NULL, ToNumberInt, NULL};
    CHECK_TRUE(da_Init(&array, info));

    std::string output = CaptureStdout(PrintArrayAction, &array);
    CHECK_TRUE(output.empty());
}

TEST(DynamicArrayPrinting, SharedDebugPrintIncludesContainerState)
{
    CHECK_TRUE(da_Init(&array, IntTypeInfo()));

    int values[] = {4, 8};
    CHECK_TRUE(da_detail_AppendRef(&array, &values[0]));
    CHECK_TRUE(da_detail_AppendRef(&array, &values[1]));

    DebugPrintContext context = {&array, "numbers", "example.c", 42};
    std::string output = CaptureStdout(DebugPrintAction, &context);

    CHECK(output.find("ADT Debug") != std::string::npos);
    CHECK(output.find("expression: numbers") != std::string::npos);
    CHECK(output.find("location: example.c:42") != std::string::npos);
    CHECK(output.find("container: DynamicArray") != std::string::npos);
    CHECK(output.find("size: 2") != std::string::npos);
    CHECK(output.find("element size: ") != std::string::npos);
    CHECK(output.find("comparator: set") != std::string::npos);
    CHECK(output.find("printer: set") != std::string::npos);
    CHECK(output.find("destructor: NULL") != std::string::npos);
    CHECK(output.find("elements: [4, 8]") != std::string::npos);
}

TEST(DynamicArrayPrinting, SharedDebugPrintHandlesNullContainer)
{
    DebugPrintContext context = {NULL, "array", "test.c", 7};
    std::string output = CaptureStdout(DebugPrintAction, &context);

    CHECK(output.find("expression: array") != std::string::npos);
    CHECK(output.find("location: test.c:7") != std::string::npos);
    CHECK(output.find("state: NULL") != std::string::npos);
}

TEST(DynamicArrayPrinting, SharedDebugPrintReportsUninitializedContainer)
{
    array = DynamicArray_t{};
    array.super._private.elementType = IntTypeInfo();

    DebugPrintContext context = {&array, "array", "test.c", 9};
    std::string output = CaptureStdout(DebugPrintAction, &context);

    CHECK(output.find("container: <uninitialized>") != std::string::npos);
    CHECK(output.find("elements: <unavailable>") != std::string::npos);
}

TEST(DynamicArrayPrinting, SharedDebugPrintReportsMissingPrinter)
{
    ADT_ElementTypeInfo_t info = {sizeof(int), CompareInt, NULL, ToNumberInt, NULL};
    CHECK_TRUE(da_Init(&array, info));

    DebugPrintContext context = {&array, "array", "test.c", 11};
    std::string output = CaptureStdout(DebugPrintAction, &context);

    CHECK(output.find("printer: NULL") != std::string::npos);
    CHECK(output.find("elements: <no print function>") != std::string::npos);
}

TEST(DynamicArrayPrinting, SharedDebugPrintUsesConfiguredPrinterForEveryElement)
{
    ADT_ElementTypeInfo_t info = {sizeof(int), CompareInt, CountIntPrint, ToNumberInt, NULL};
    CHECK_TRUE(da_Init(&array, info));

    int values[] = {2, 3, 5};
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(da_detail_AppendRef(&array, &values[i]));
    }

    DebugPrintContext context = {&array, "values", "test.c", 13};
    CaptureStdout(DebugPrintAction, &context);

    LONGS_EQUAL(3, printCallCount);
    LONGS_EQUAL(10, printedTotal);
}
