extern "C"
{
#include "libadt/internal/storage/contiguous_storage.h"
}

#include "CppUTest/TestHarness.h"
#include <cstdint>

TEST_GROUP(ContiguousStorage)
{
    ContiguousStorage_t storage;

    void setup()
    {
        storage = ContiguousStorage_t{};
    }

    void teardown()
    {
        contiguousStorage_Destroy(&storage);
    }
};

TEST(ContiguousStorage, InitAllocatesInitialCapacity)
{
    CHECK_TRUE(contiguousStorage_Init(&storage, sizeof(int)));
    CHECK(storage.data != NULL);
    UNSIGNED_LONGS_EQUAL(8, storage.capacity);
}

TEST(ContiguousStorage, InitRejectsInvalidArguments)
{
    CHECK_FALSE(contiguousStorage_Init(NULL, sizeof(int)));
    CHECK_FALSE(contiguousStorage_Init(&storage, 0));
    CHECK_FALSE(contiguousStorage_Init(&storage, SIZE_MAX));
    POINTERS_EQUAL(NULL, storage.data);
    UNSIGNED_LONGS_EQUAL(0, storage.capacity);
}

TEST(ContiguousStorage, InitFromCopiesElementsAndGrows)
{
    int values[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    CHECK_TRUE(contiguousStorage_InitFrom(&storage, values, 10, sizeof(int)));
    CHECK(storage.capacity >= 10);

    values[0] = 99;
    LONGS_EQUAL(0, *static_cast<const int *>(
                       contiguousStorage_AtConst(&storage, 0, sizeof(int))));
    LONGS_EQUAL(9, *static_cast<const int *>(
                       contiguousStorage_AtConst(&storage, 9, sizeof(int))));
}

TEST(ContiguousStorage, InitFromAllowsEmptyInput)
{
    CHECK_TRUE(contiguousStorage_InitFrom(&storage, NULL, 0, sizeof(int)));
    POINTERS_EQUAL(NULL, storage.data);
    UNSIGNED_LONGS_EQUAL(0, storage.capacity);

    int value = 7;
    CHECK_TRUE(contiguousStorage_Insert(
        &storage,
        0,
        0,
        &value,
        sizeof(int)));
    LONGS_EQUAL(
        value,
        *static_cast<int *>(
            contiguousStorage_At(&storage, 0, sizeof(int))));
}

TEST(ContiguousStorage, InitFromRejectsInvalidArguments)
{
    int value = 1;

    CHECK_FALSE(contiguousStorage_InitFrom(NULL, &value, 1, sizeof(int)));
    CHECK_FALSE(contiguousStorage_InitFrom(&storage, NULL, 1, sizeof(int)));
    CHECK_FALSE(contiguousStorage_InitFrom(&storage, &value, 1, 0));
}

TEST(ContiguousStorage, ReservePreservesValuesAndDoesNotShrink)
{
    int values[] = {1, 2, 3};
    CHECK_TRUE(contiguousStorage_InitFrom(&storage, values, 3, sizeof(int)));

    CHECK_TRUE(contiguousStorage_Reserve(&storage, 20, sizeof(int)));
    CHECK(storage.capacity >= 20);
    LONGS_EQUAL(1, *static_cast<int *>(
                       contiguousStorage_At(&storage, 0, sizeof(int))));
    LONGS_EQUAL(3, *static_cast<int *>(
                       contiguousStorage_At(&storage, 2, sizeof(int))));

    size_t capacity = storage.capacity;
    void *data = storage.data;
    CHECK_TRUE(contiguousStorage_Reserve(&storage, 2, sizeof(int)));
    UNSIGNED_LONGS_EQUAL(capacity, storage.capacity);
    POINTERS_EQUAL(data, storage.data);
}

TEST(ContiguousStorage, ReserveRejectsInvalidStateAndOverflow)
{
    ContiguousStorage_t invalid = {
        .data = NULL,
        .capacity = 1};

    CHECK_FALSE(contiguousStorage_Reserve(NULL, 1, sizeof(int)));
    CHECK_FALSE(contiguousStorage_Reserve(&invalid, 1, sizeof(int)));
    CHECK_FALSE(contiguousStorage_Reserve(&storage, 1, 0));

    CHECK_TRUE(contiguousStorage_Init(&storage, sizeof(int)));
    CHECK_FALSE(contiguousStorage_Reserve(&storage, SIZE_MAX, sizeof(int)));
}

TEST(ContiguousStorage, AtChecksPhysicalBounds)
{
    CHECK_TRUE(contiguousStorage_Init(&storage, sizeof(int)));

    CHECK(contiguousStorage_At(&storage, 0, sizeof(int)) != NULL);
    CHECK(contiguousStorage_AtConst(&storage, 7, sizeof(int)) != NULL);
    POINTERS_EQUAL(NULL, contiguousStorage_At(&storage, 8, sizeof(int)));
    POINTERS_EQUAL(NULL, contiguousStorage_At(NULL, 0, sizeof(int)));
    POINTERS_EQUAL(NULL, contiguousStorage_At(&storage, 0, 0));
}

TEST(ContiguousStorage, InsertSupportsBeginningMiddleAndEnd)
{
    int values[] = {2, 4};
    int one = 1;
    int three = 3;
    int five = 5;
    CHECK_TRUE(contiguousStorage_InitFrom(&storage, values, 2, sizeof(int)));

    CHECK_TRUE(contiguousStorage_Insert(&storage, 2, 0, &one, sizeof(int)));
    CHECK_TRUE(contiguousStorage_Insert(&storage, 3, 2, &three, sizeof(int)));
    CHECK_TRUE(contiguousStorage_Insert(&storage, 4, 4, &five, sizeof(int)));

    int expected[] = {1, 2, 3, 4, 5};
    for (size_t i = 0; i < 5; ++i)
    {
        LONGS_EQUAL(
            expected[i],
            *static_cast<int *>(contiguousStorage_At(&storage, i, sizeof(int))));
    }
}

TEST(ContiguousStorage, InsertCopiesAliasedElementBeforeGrowth)
{
    int values[] = {0, 1, 2, 3, 4, 5, 6, 7};
    CHECK_TRUE(contiguousStorage_InitFrom(&storage, values, 8, sizeof(int)));

    const void *aliasedElement =
        contiguousStorage_AtConst(&storage, 2, sizeof(int));
    CHECK_TRUE(contiguousStorage_Insert(
        &storage,
        8,
        8,
        aliasedElement,
        sizeof(int)));

    LONGS_EQUAL(
        2,
        *static_cast<int *>(contiguousStorage_At(&storage, 8, sizeof(int))));
}

TEST(ContiguousStorage, InsertRejectsInvalidArguments)
{
    int value = 1;
    CHECK_TRUE(contiguousStorage_Init(&storage, sizeof(int)));

    CHECK_FALSE(contiguousStorage_Insert(NULL, 0, 0, &value, sizeof(int)));
    CHECK_FALSE(contiguousStorage_Insert(&storage, 0, 0, NULL, sizeof(int)));
    CHECK_FALSE(contiguousStorage_Insert(&storage, 0, 1, &value, sizeof(int)));
    CHECK_FALSE(contiguousStorage_Insert(&storage, 0, 0, &value, 0));
    CHECK_FALSE(contiguousStorage_Insert(
        &storage,
        SIZE_MAX,
        0,
        &value,
        sizeof(int)));
}

TEST(ContiguousStorage, EraseShiftsFollowingElements)
{
    int values[] = {1, 2, 3, 4};
    CHECK_TRUE(contiguousStorage_InitFrom(&storage, values, 4, sizeof(int)));

    CHECK_TRUE(contiguousStorage_Erase(&storage, 4, 1, sizeof(int)));

    int expected[] = {1, 3, 4};
    for (size_t i = 0; i < 3; ++i)
    {
        LONGS_EQUAL(
            expected[i],
            *static_cast<int *>(contiguousStorage_At(&storage, i, sizeof(int))));
    }
}

TEST(ContiguousStorage, EraseRejectsInvalidArguments)
{
    CHECK_TRUE(contiguousStorage_Init(&storage, sizeof(int)));

    CHECK_FALSE(contiguousStorage_Erase(NULL, 1, 0, sizeof(int)));
    CHECK_FALSE(contiguousStorage_Erase(&storage, 0, 0, sizeof(int)));
    CHECK_FALSE(contiguousStorage_Erase(&storage, 1, 1, sizeof(int)));
    CHECK_FALSE(contiguousStorage_Erase(&storage, 1, 0, 0));
}

TEST(ContiguousStorage, ContainsAddressChecksAllocatedBuffer)
{
    CHECK_TRUE(contiguousStorage_Init(&storage, sizeof(int)));

    void *first = contiguousStorage_At(&storage, 0, sizeof(int));
    void *last = contiguousStorage_At(&storage, 7, sizeof(int));
    int outside = 0;

    CHECK_TRUE(contiguousStorage_ContainsAddress(&storage, first, sizeof(int)));
    CHECK_TRUE(contiguousStorage_ContainsAddress(&storage, last, sizeof(int)));
    CHECK_FALSE(contiguousStorage_ContainsAddress(
        &storage,
        &outside,
        sizeof(int)));
    CHECK_FALSE(contiguousStorage_ContainsAddress(NULL, first, sizeof(int)));
}

TEST(ContiguousStorage, DestroyResetsStorage)
{
    CHECK_TRUE(contiguousStorage_Init(&storage, sizeof(int)));

    contiguousStorage_Destroy(&storage);

    POINTERS_EQUAL(NULL, storage.data);
    UNSIGNED_LONGS_EQUAL(0, storage.capacity);
    contiguousStorage_Destroy(NULL);
}
