extern "C"
{
#include "libadt/dynamic_array.h"
}

#include "CppUTest/TestHarness.h"

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
        array.elementSize = 0;
    }

    void teardown()
    {
        da_Destroy(&array);
    }
};

TEST(DynamicArrayInitialization, InitCreatesEmptyArray)
{
    CHECK_TRUE(da_Init(&array, sizeof(int)));

    CHECK(array.data != NULL);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(INITIAL_CAPACITY, array.capacity);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.elementSize);
}

TEST(DynamicArrayInitialization, InitFromCopiesIntegerElements)
{
    int input[] = {10, 20, 30, 40};

    CHECK_TRUE(
        da_InitFrom(&array, input, 4, sizeof(int)));

    int *stored = static_cast<int *>(array.data);

    UNSIGNED_LONGS_EQUAL(4, array.size);
    CHECK(array.capacity >= array.size);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.elementSize);

    LONGS_EQUAL(10, stored[0]);
    LONGS_EQUAL(20, stored[1]);
    LONGS_EQUAL(30, stored[2]);
    LONGS_EQUAL(40, stored[3]);
}

TEST(DynamicArrayInitialization, InitFromMakesIndependentCopy)
{
    int input[] = {10, 20, 30};

    CHECK_TRUE(
        da_InitFrom(&array, input, 3, sizeof(int)));

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

    CHECK_TRUE(
        da_InitFrom(&array, input, 20, sizeof(int)));

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

    CHECK_TRUE(
        da_InitFrom(&array, input, count, sizeof(int)));

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
    CHECK_TRUE(
        da_InitFrom(&array, NULL, 0, sizeof(int)));

    UNSIGNED_LONGS_EQUAL(0, array.size);
    CHECK(array.capacity >= INITIAL_CAPACITY);
    UNSIGNED_LONGS_EQUAL(sizeof(int), array.elementSize);
}

TEST(DynamicArrayInitialization, InitRejectsNullArray)
{
    CHECK_FALSE(
        da_Init(NULL, sizeof(int)));
}

TEST(DynamicArrayInitialization, InitRejectsZeroElementSize)
{
    CHECK_FALSE(
        da_Init(&array, 0));

    POINTERS_EQUAL(NULL, array.data);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(0, array.capacity);
    UNSIGNED_LONGS_EQUAL(0, array.elementSize);
}

TEST(DynamicArrayInitialization, InitFromRejectsNullArray)
{
    int input[] = {1, 2, 3};

    CHECK_FALSE(
        da_InitFrom(NULL, input, 3, sizeof(int)));
}

TEST(DynamicArrayInitialization, InitFromRejectsNullElementsWithCount)
{
    CHECK_FALSE(
        da_InitFrom(&array, NULL, 3, sizeof(int)));

    POINTERS_EQUAL(NULL, array.data);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(0, array.capacity);
}

TEST(DynamicArrayInitialization, InitFromRejectsZeroElementSize)
{
    int input[] = {1, 2, 3};

    CHECK_FALSE(
        da_InitFrom(&array, input, 3, 0));

    POINTERS_EQUAL(NULL, array.data);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(0, array.capacity);
}

TEST(DynamicArrayInitialization, InitFromSupportsDoubleElements)
{
    double input[] = {1.5, 2.5, 3.5};

    CHECK_TRUE(
        da_InitFrom(&array, input, 3, sizeof(double)));

    double *stored = static_cast<double *>(array.data);

    UNSIGNED_LONGS_EQUAL(3, array.size);
    UNSIGNED_LONGS_EQUAL(sizeof(double), array.elementSize);

    DOUBLES_EQUAL(1.5, stored[0], 0.000001);
    DOUBLES_EQUAL(2.5, stored[1], 0.000001);
    DOUBLES_EQUAL(3.5, stored[2], 0.000001);
}

TEST(DynamicArrayInitialization, DestroyResetsArray)
{
    CHECK_TRUE(
        da_Init(&array, sizeof(int)));

    da_Destroy(&array);

    POINTERS_EQUAL(NULL, array.data);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(0, array.capacity);
    UNSIGNED_LONGS_EQUAL(0, array.elementSize);
}

TEST(DynamicArrayInitialization, DestroyHandlesNullPointer)
{
    da_Destroy(NULL);
}

TEST(DynamicArrayInitialization, DestroyCanBeCalledTwice)
{
    CHECK_TRUE(
        da_Init(&array, sizeof(int)));

    da_Destroy(&array);
    da_Destroy(&array);

    POINTERS_EQUAL(NULL, array.data);
    UNSIGNED_LONGS_EQUAL(0, array.size);
    UNSIGNED_LONGS_EQUAL(0, array.capacity);
    UNSIGNED_LONGS_EQUAL(0, array.elementSize);
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
        array.elementSize = 0;

        CHECK_TRUE(
            da_Init(&array, sizeof(int)));
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
        array.elementSize = 0;

        CHECK_TRUE(
            da_Init(&array, sizeof(double)));
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