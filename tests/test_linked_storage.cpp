extern "C"
{
#include "libadt/internal/storage/linked_storage.h"
}

#include "CppUTest/TestHarness.h"
#include <cstdint>

TEST_GROUP(LinkedStorage)
{
    LinkedStorage_t storage;

    void setup()
    {
        storage = LinkedStorage_t{};
    }

    void teardown()
    {
        linkedStorage_Destroy(&storage);
    }
};

TEST(LinkedStorage, InitCreatesEmptyStorage)
{
    CHECK_TRUE(linkedStorage_Init(&storage));
    POINTERS_EQUAL(NULL, storage.head);
    POINTERS_EQUAL(NULL, storage.tail);
    CHECK_FALSE(linkedStorage_Init(NULL));
}

TEST(LinkedStorage, InitFromCopiesElements)
{
    int values[] = {1, 2, 3};
    CHECK_TRUE(linkedStorage_InitFrom(&storage, values, 3, sizeof(int)));

    values[0] = 99;
    LONGS_EQUAL(
        1,
        *static_cast<const int *>(
            linkedStorage_AtConst(&storage, 3, 0)));
    LONGS_EQUAL(
        3,
        *static_cast<const int *>(
            linkedStorage_AtConst(&storage, 3, 2)));
}

TEST(LinkedStorage, InitFromAllowsEmptyInput)
{
    CHECK_TRUE(linkedStorage_InitFrom(&storage, NULL, 0, sizeof(int)));
    POINTERS_EQUAL(NULL, storage.head);
    POINTERS_EQUAL(NULL, storage.tail);
}

TEST(LinkedStorage, InitFromRejectsInvalidArguments)
{
    int value = 1;

    CHECK_FALSE(linkedStorage_InitFrom(NULL, &value, 1, sizeof(int)));
    CHECK_FALSE(linkedStorage_InitFrom(&storage, NULL, 1, sizeof(int)));
    CHECK_FALSE(linkedStorage_InitFrom(&storage, &value, 1, 0));
    CHECK_FALSE(linkedStorage_InitFrom(
        &storage,
        &value,
        SIZE_MAX,
        sizeof(int)));
}

TEST(LinkedStorage, AtUsesLogicalBounds)
{
    int values[] = {1, 2, 3};
    CHECK_TRUE(linkedStorage_InitFrom(&storage, values, 3, sizeof(int)));

    LONGS_EQUAL(1, *static_cast<int *>(linkedStorage_At(&storage, 3, 0)));
    LONGS_EQUAL(3, *static_cast<int *>(linkedStorage_At(&storage, 3, 2)));
    POINTERS_EQUAL(NULL, linkedStorage_At(&storage, 3, 3));
    POINTERS_EQUAL(NULL, linkedStorage_At(NULL, 3, 0));
}

TEST(LinkedStorage, PrependAppendAndInsertPreserveOrder)
{
    int two = 2;
    int four = 4;
    int one = 1;
    int three = 3;

    CHECK_TRUE(linkedStorage_Init(&storage));
    CHECK_TRUE(linkedStorage_Append(&storage, &two, sizeof(int)));
    CHECK_TRUE(linkedStorage_Append(&storage, &four, sizeof(int)));
    CHECK_TRUE(linkedStorage_Prepend(&storage, &one, sizeof(int)));
    CHECK_TRUE(linkedStorage_Insert(
        &storage,
        3,
        2,
        &three,
        sizeof(int)));

    int expected[] = {1, 2, 3, 4};
    for (size_t i = 0; i < 4; ++i)
    {
        LONGS_EQUAL(
            expected[i],
            *static_cast<int *>(linkedStorage_At(&storage, 4, i)));
    }
}

TEST(LinkedStorage, InsertHandlesBothEndpoints)
{
    int two = 2;
    int one = 1;
    int three = 3;

    CHECK_TRUE(linkedStorage_Init(&storage));
    CHECK_TRUE(linkedStorage_Insert(&storage, 0, 0, &two, sizeof(int)));
    CHECK_TRUE(linkedStorage_Insert(&storage, 1, 0, &one, sizeof(int)));
    CHECK_TRUE(linkedStorage_Insert(&storage, 2, 2, &three, sizeof(int)));

    LONGS_EQUAL(1, *static_cast<int *>(linkedStorage_At(&storage, 3, 0)));
    LONGS_EQUAL(3, *static_cast<int *>(linkedStorage_At(&storage, 3, 2)));
}

TEST(LinkedStorage, InsertRejectsInvalidArguments)
{
    int value = 1;
    CHECK_TRUE(linkedStorage_Init(&storage));

    CHECK_FALSE(linkedStorage_Insert(NULL, 0, 0, &value, sizeof(int)));
    CHECK_FALSE(linkedStorage_Insert(&storage, 0, 0, NULL, sizeof(int)));
    CHECK_FALSE(linkedStorage_Insert(&storage, 0, 1, &value, sizeof(int)));
    CHECK_FALSE(linkedStorage_Insert(&storage, 0, 0, &value, 0));
    CHECK_FALSE(linkedStorage_Insert(
        &storage,
        SIZE_MAX,
        0,
        &value,
        sizeof(int)));
}

TEST(LinkedStorage, NodesExposeBidirectionalLinks)
{
    int values[] = {1, 2, 3};
    CHECK_TRUE(linkedStorage_InitFrom(&storage, values, 3, sizeof(int)));

    POINTERS_EQUAL(NULL, storage.head->previous);
    POINTERS_EQUAL(NULL, storage.tail->next);
    POINTERS_EQUAL(storage.head, storage.head->next->previous);
    POINTERS_EQUAL(storage.tail, storage.tail->previous->next);

    LONGS_EQUAL(1, *static_cast<int *>(storage.head->data));
    LONGS_EQUAL(2, *static_cast<int *>(storage.head->next->data));
    LONGS_EQUAL(3, *static_cast<int *>(storage.tail->data));
    LONGS_EQUAL(2, *static_cast<int *>(storage.tail->previous->data));
}

TEST(LinkedStorage, EraseSupportsHeadMiddleAndTail)
{
    int values[] = {1, 2, 3, 4, 5};
    CHECK_TRUE(linkedStorage_InitFrom(&storage, values, 5, sizeof(int)));

    CHECK_TRUE(linkedStorage_Erase(&storage, 5, 0));
    CHECK_TRUE(linkedStorage_Erase(&storage, 4, 1));
    CHECK_TRUE(linkedStorage_Erase(&storage, 3, 2));

    LONGS_EQUAL(2, *static_cast<int *>(linkedStorage_At(&storage, 2, 0)));
    LONGS_EQUAL(4, *static_cast<int *>(linkedStorage_At(&storage, 2, 1)));
}

TEST(LinkedStorage, EraseRejectsInvalidArguments)
{
    int value = 1;
    CHECK_TRUE(linkedStorage_InitFrom(&storage, &value, 1, sizeof(int)));

    CHECK_FALSE(linkedStorage_Erase(NULL, 1, 0));
    CHECK_FALSE(linkedStorage_Erase(&storage, 1, 1));
}

TEST(LinkedStorage, ContainsAddressChecksEveryElementAllocation)
{
    int values[] = {1, 2, 3};
    CHECK_TRUE(linkedStorage_InitFrom(&storage, values, 3, sizeof(int)));

    void *middle = linkedStorage_At(&storage, 3, 1);
    int outside = 0;

    CHECK_TRUE(linkedStorage_ContainsAddress(
        &storage,
        middle,
        sizeof(int)));
    CHECK_FALSE(linkedStorage_ContainsAddress(
        &storage,
        &outside,
        sizeof(int)));
    CHECK_FALSE(linkedStorage_ContainsAddress(
        NULL,
        middle,
        sizeof(int)));
}

TEST(LinkedStorage, DestroyResetsStorage)
{
    int values[] = {1, 2, 3};
    CHECK_TRUE(linkedStorage_InitFrom(&storage, values, 3, sizeof(int)));

    linkedStorage_Destroy(&storage);

    POINTERS_EQUAL(NULL, storage.head);
    POINTERS_EQUAL(NULL, storage.tail);
    linkedStorage_Destroy(NULL);
}
