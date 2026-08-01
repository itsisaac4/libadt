extern "C"
{
#include "libadt/dynamic_array.h"
#include "libadt/linked_list.h"
}

#include "CppUTest/TestHarness.h"
#include <cstdlib>
#include <cstring>

static int CompareIntDescendingForSort(const void *first, const void *second)
{
    return CompareInt(second, first);
}

static void CheckArrayValues(const DynamicArray_t *array, const int *expected, size_t count)
{
    UNSIGNED_LONGS_EQUAL(count, adt_Size(array));

    for (size_t i = 0; i < count; i++)
    {
        int actual = 0;
        CHECK_TRUE(da_Get(array, i, &actual));
        LONGS_EQUAL(expected[i], actual);
    }
}

static void CheckListValues(const LinkedList_t *list, const int *expected, size_t count)
{
    UNSIGNED_LONGS_EQUAL(count, adt_Size(list));

    for (size_t i = 0; i < count; i++)
    {
        int actual = 0;
        CHECK_TRUE(ll_Get(list, i, &actual));
        LONGS_EQUAL(expected[i], actual);
    }
}

typedef struct
{
    int key;
    char *resource;
} OwnedValue_t;

static size_t destroyCount;

static int CompareOwnedValue(const void *first, const void *second)
{
    const OwnedValue_t *left = static_cast<const OwnedValue_t *>(first);
    const OwnedValue_t *right = static_cast<const OwnedValue_t *>(second);
    return (left->key > right->key) - (left->key < right->key);
}

static void DestroyOwnedValue(void *element)
{
    OwnedValue_t *value = static_cast<OwnedValue_t *>(element);
    free(value->resource);
    value->resource = NULL;
    destroyCount++;
}

static char *CopyString(const char *text)
{
    size_t size = strlen(text) + 1;
    char *copy = static_cast<char *>(malloc(size));
    CHECK(copy != NULL);
    memcpy(copy, text, size);
    return copy;
}

TEST_GROUP(Sorting)
{
    void setup()
    {
        destroyCount = 0;
    }
};

TEST(Sorting, SortsDynamicArrayWithConfiguredComparator)
{
    int values[] = {4, 1, 5, 2, 3};
    int expected[] = {1, 2, 3, 4, 5};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_InitFrom(&array, values, 5, type));
    CHECK_TRUE(adt_Sort(&array, ADT_SORT_BUBBLE));
    CheckArrayValues(&array, expected, 5);

    da_Destroy(&array);
}

TEST(Sorting, SortsLinkedListWithConfiguredComparator)
{
    int values[] = {4, 1, 5, 2, 3};
    int expected[] = {1, 2, 3, 4, 5};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    LinkedList_t list = {};

    CHECK_TRUE(ll_InitFrom(&list, values, 5, type));
    CHECK_TRUE(adt_Sort(&list, ADT_SORT_BUBBLE));
    CheckListValues(&list, expected, 5);

    ll_Destroy(&list);
}

TEST(Sorting, SelectionSortWorksAcrossContainerTypes)
{
    int values[] = {5, 2, 4, 1, 3, 2};
    int expected[] = {1, 2, 2, 3, 4, 5};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 6, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 6, type));

    CHECK_TRUE(adt_Sort(&array, ADT_SORT_SELECTION));
    CHECK_TRUE(adt_Sort(&list, ADT_SORT_SELECTION));
    CheckArrayValues(&array, expected, 6);
    CheckListValues(&list, expected, 6);

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(Sorting, SelectionSortUsesComparatorOverride)
{
    int values[] = {1, 4, 2, 5, 3};
    int expected[] = {5, 4, 3, 2, 1};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_InitFrom(&array, values, 5, type));
    CHECK_TRUE(adt_SortBy(&array, ADT_SORT_SELECTION, CompareIntDescendingForSort));
    CheckArrayValues(&array, expected, 5);

    da_Destroy(&array);
}

TEST(Sorting, InsertionSortWorksAcrossContainerTypes)
{
    int values[] = {1, 3, 2, 5, 4, 4};
    int expected[] = {1, 2, 3, 4, 4, 5};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 6, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 6, type));

    CHECK_TRUE(adt_Sort(&array, ADT_SORT_INSERTION));
    CHECK_TRUE(adt_Sort(&list, ADT_SORT_INSERTION));
    CheckArrayValues(&array, expected, 6);
    CheckListValues(&list, expected, 6);

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(Sorting, QuickSortWorksAcrossContainerTypes)
{
    int values[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 1};
    int expected[] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 10, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 10, type));

    CHECK_TRUE(adt_Sort(&array, ADT_SORT_QUICK));
    CHECK_TRUE(adt_Sort(&list, ADT_SORT_QUICK));
    CheckArrayValues(&array, expected, 10);
    CheckListValues(&list, expected, 10);

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(Sorting, QuickSortUsesComparatorOverride)
{
    int values[] = {1, 5, 2, 4, 3};
    int expected[] = {5, 4, 3, 2, 1};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    LinkedList_t list = {};

    CHECK_TRUE(ll_InitFrom(&list, values, 5, type));
    CHECK_TRUE(adt_SortBy(&list, ADT_SORT_QUICK, CompareIntDescendingForSort));
    CheckListValues(&list, expected, 5);

    ll_Destroy(&list);
}

TEST(Sorting, QuickSortHandlesDuplicateOnlyInput)
{
    int values[128] = {};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_InitFrom(&array, values, 128, type));
    CHECK_TRUE(adt_Sort(&array, ADT_SORT_QUICK));
    CheckArrayValues(&array, values, 128);

    da_Destroy(&array);
}

TEST(Sorting, BogoSortWorksAcrossContainerTypes)
{
    int values[] = {3, 1, 4, 2};
    int expected[] = {1, 2, 3, 4};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 4, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 4, type));

    CHECK_TRUE(adt_Sort(&array, ADT_SORT_BOGO));
    CHECK_TRUE(adt_Sort(&list, ADT_SORT_BOGO));
    CheckArrayValues(&array, expected, 4);
    CheckListValues(&list, expected, 4);

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(Sorting, BogoSortRejectsOversizedUnsortedInputWithoutChangingIt)
{
    int values[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_InitFrom(&array, values, 9, type));
    CHECK_FALSE(adt_Sort(&array, ADT_SORT_BOGO));
    CheckArrayValues(&array, values, 9);

    da_Destroy(&array);
}

TEST(Sorting, BogoSortAcceptsOversizedInputWhenAlreadySorted)
{
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    LinkedList_t list = {};

    CHECK_TRUE(ll_InitFrom(&list, values, 9, type));
    CHECK_TRUE(adt_Sort(&list, ADT_SORT_BOGO));
    CheckListValues(&list, values, 9);

    ll_Destroy(&list);
}

TEST(Sorting, ComparatorOverrideControlsOrdering)
{
    int values[] = {2, 5, 1, 4, 3};
    int expected[] = {5, 4, 3, 2, 1};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 5, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 5, type));

    CHECK_TRUE(adt_SortBy(&array, ADT_SORT_BUBBLE, CompareIntDescendingForSort));
    CHECK_TRUE(adt_SortBy(&list, ADT_SORT_BUBBLE, CompareIntDescendingForSort));
    CheckArrayValues(&array, expected, 5);
    CheckListValues(&list, expected, 5);

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(Sorting, ComparatorOverrideWorksWithoutConfiguredComparator)
{
    int values[] = {3, 1, 2};
    int expected[] = {1, 2, 3};
    ADT_ElementTypeInfo_t type = {sizeof(int), NULL, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_InitFrom(&array, values, 3, type));
    CHECK_FALSE(adt_Sort(&array, ADT_SORT_BUBBLE));
    CHECK_TRUE(adt_SortBy(&array, ADT_SORT_BUBBLE, CompareInt));
    CheckArrayValues(&array, expected, 3);

    da_Destroy(&array);
}

TEST(Sorting, AcceptsEmptyAndSingleElementContainers)
{
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};
    int value = 7;

    CHECK_TRUE(da_Init(&array, type));
    CHECK_TRUE(ll_InitFrom(&list, &value, 1, type));

    CHECK_TRUE(adt_Sort(&array, ADT_SORT_BUBBLE));
    CHECK_TRUE(adt_Sort(&list, ADT_SORT_QUICK));
    CheckListValues(&list, &value, 1);

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(Sorting, RejectsInvalidArgumentsAndAlgorithm)
{
    int values[] = {2, 1};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_InitFrom(&array, values, 2, type));

    CHECK_FALSE(adt_Sort(NULL, ADT_SORT_BUBBLE));
    CHECK_FALSE(adt_SortBy(NULL, ADT_SORT_BUBBLE, CompareInt));
    CHECK_FALSE(adt_SortBy(&array, ADT_SORT_BUBBLE, NULL));
    CHECK_FALSE(adt_Sort(&array, static_cast<ADT_SortAlgorithm_t>(99)));

    da_Destroy(&array);
}

TEST(Sorting, PreservesResourceOwnershipWhileReordering)
{
    OwnedValue_t values[] = {
        {3, CopyString("three")},
        {1, CopyString("one")},
        {2, CopyString("two")}};
    ADT_ElementTypeInfo_t type = {
        sizeof(OwnedValue_t),
        CompareOwnedValue,
        NULL,
        NULL,
        DestroyOwnedValue};
    LinkedList_t list = {};

    CHECK_TRUE(ll_InitFrom(&list, values, 3, type));
    CHECK_TRUE(adt_Sort(&list, ADT_SORT_QUICK));
    UNSIGNED_LONGS_EQUAL(0, destroyCount);

    for (size_t i = 0; i < 3; i++)
    {
        OwnedValue_t actual = {};
        CHECK_TRUE(ll_Get(&list, i, &actual));
        LONGS_EQUAL(static_cast<long>(i + 1), actual.key);
    }

    ll_Destroy(&list);
    UNSIGNED_LONGS_EQUAL(3, destroyCount);
}

TEST(Sorting, IsSortedWorksAcrossContiguousAndLinkedContainers)
{
    int sortedValues[] = {1, 2, 2, 4, 8};
    int unsortedValues[] = {1, 3, 2, 4};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, sortedValues, 5, type));
    CHECK_TRUE(ll_InitFrom(&list, unsortedValues, 4, type));
    CHECK_TRUE(adt_isSorted(&array));
    CHECK_FALSE(adt_isSorted(&list));

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(Sorting, IsSortedByUsesComparatorOverride)
{
    int values[] = {5, 4, 4, 2, 1};
    ADT_ElementTypeInfo_t type = {sizeof(int), NULL, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_InitFrom(&array, values, 5, type));
    CHECK_FALSE(adt_isSorted(&array));
    CHECK_TRUE(adt_isSortedBy(&array, CompareIntDescendingForSort));

    da_Destroy(&array);
}

TEST(Sorting, IsSortedAcceptsEmptyAndSingleElementContainers)
{
    int value = 7;
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_Init(&array, type));
    CHECK_TRUE(ll_InitFrom(&list, &value, 1, type));
    CHECK_TRUE(adt_isSorted(&array));
    CHECK_TRUE(adt_isSorted(&list));

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(Sorting, IsSortedRejectsInvalidArguments)
{
    int values[] = {1, 2};
    ADT_ElementTypeInfo_t type = {sizeof(int), CompareInt, PrintInt, ToNumberInt, NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_InitFrom(&array, values, 2, type));
    CHECK_FALSE(adt_isSorted(NULL));
    CHECK_FALSE(adt_isSortedBy(NULL, CompareInt));
    CHECK_FALSE(adt_isSortedBy(&array, NULL));

    da_Destroy(&array);
}
