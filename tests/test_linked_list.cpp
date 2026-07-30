extern "C"
{
#include "libadt/linked_list.h"
#include "libadt/detail/linked_list_node.h"
}

#include "CppUTest/TestHarness.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>

extern "C"
{
    bool C23LinkedListInitInfersIntType(void);
    bool C23LinkedListInitFromInfersIntType(void);
}

static ADT_TypeInfo_t LinkedListIntTypeInfo()
{
    ADT_TypeInfo_t typeInfo = {
        sizeof(int),
        CompareInt,
        PrintInt,
        NULL};
    return typeInfo;
}

static ADT_TypeInfo_t LinkedListDoubleTypeInfo()
{
    ADT_TypeInfo_t typeInfo = {
        sizeof(double),
        CompareDouble,
        PrintDouble,
        NULL};
    return typeInfo;
}

static ADT_TypeInfo_t InvalidLinkedListTypeInfo()
{
    ADT_TypeInfo_t typeInfo = {0, NULL, NULL, NULL};
    return typeInfo;
}

static void CheckIntList(const LinkedList_t *list, const int *expected, size_t expectedCount)
{
    UNSIGNED_LONGS_EQUAL(expectedCount, list->super._private.size);

    if (expectedCount == 0)
    {
        POINTERS_EQUAL(NULL, list->_private.head);
        POINTERS_EQUAL(NULL, list->_private.tail);
        return;
    }

    POINTERS_EQUAL(NULL, list->_private.head->_private.previous);
    POINTERS_EQUAL(NULL, list->_private.tail->_private.next);

    LinkedListNode_t *current = list->_private.head;
    LinkedListNode_t *previous = NULL;

    for (size_t i = 0; i < expectedCount; ++i)
    {
        CHECK(current != NULL);
        POINTERS_EQUAL(previous, current->_private.previous);
        LONGS_EQUAL(expected[i], *static_cast<int *>(current->_private.data));
        previous = current;
        current = current->_private.next;
    }

    POINTERS_EQUAL(NULL, current);
    POINTERS_EQUAL(list->_private.tail, previous);

    current = list->_private.tail;
    LinkedListNode_t *next = NULL;

    for (size_t i = expectedCount; i > 0; --i)
    {
        CHECK(current != NULL);
        POINTERS_EQUAL(next, current->_private.next);
        LONGS_EQUAL(expected[i - 1], *static_cast<int *>(current->_private.data));
        next = current;
        current = current->_private.previous;
    }

    POINTERS_EQUAL(NULL, current);
    POINTERS_EQUAL(list->_private.head, next);
}

/* =========================================================
 * Initialization and destruction tests
 * ========================================================= */

TEST_GROUP(LinkedListInitialization)
{
    LinkedList_t list;

    void setup()
    {
        list = LinkedList_t{};
    }

    void teardown()
    {
        ll_Destroy(&list);
    }
};

TEST(LinkedListInitialization, InitCreatesEmptyList)
{
    CHECK_TRUE(ll_Init(&list, LinkedListIntTypeInfo()));

    CheckIntList(&list, NULL, 0);
    UNSIGNED_LONGS_EQUAL(sizeof(int), list.super._private.type.elementSize);
    FUNCTIONPOINTERS_EQUAL(CompareInt, list.super._private.type.compare);
    FUNCTIONPOINTERS_EQUAL(PrintInt, list.super._private.type.print);
}

TEST(LinkedListInitialization, InitMacroInfersBuiltInTypeCallbacks)
{
    CHECK_TRUE(C23LinkedListInitInfersIntType());
}

TEST(LinkedListInitialization, InitFromMacroInfersBuiltInTypeCallbacks)
{
    CHECK_TRUE(C23LinkedListInitFromInfersIntType());
}

TEST(LinkedListInitialization, InitRejectsInvalidArguments)
{
    CHECK_FALSE(ll_Init(NULL, LinkedListIntTypeInfo()));
    CHECK_FALSE(ll_Init(&list, InvalidLinkedListTypeInfo()));
    CheckIntList(&list, NULL, 0);
}

TEST(LinkedListInitialization, InitFromCopiesElementsInOrder)
{
    int values[] = {10, 20, 30};

    CHECK_TRUE(ll_InitFrom(
        &list,
        values,
        3,
        LinkedListIntTypeInfo()));

    CheckIntList(&list, values, 3);
}

TEST(LinkedListInitialization, InitFromMakesIndependentCopies)
{
    int values[] = {10, 20};
    int expected[] = {10, 20};

    CHECK_TRUE(ll_InitFrom(
        &list,
        values,
        2,
        LinkedListIntTypeInfo()));

    values[0] = 99;
    CheckIntList(&list, expected, 2);
}

TEST(LinkedListInitialization, InitFromAllowsEmptyInput)
{
    CHECK_TRUE(ll_InitFrom(
        &list,
        NULL,
        0,
        LinkedListIntTypeInfo()));

    CheckIntList(&list, NULL, 0);
}

TEST(LinkedListInitialization, InitFromRejectsInvalidArguments)
{
    int value = 10;

    CHECK_FALSE(ll_InitFrom(
        NULL,
        &value,
        1,
        LinkedListIntTypeInfo()));

    CHECK_FALSE(ll_InitFrom(
        &list,
        NULL,
        1,
        LinkedListIntTypeInfo()));

    CHECK_FALSE(ll_InitFrom(
        &list,
        &value,
        1,
        InvalidLinkedListTypeInfo()));

    CHECK_FALSE(ll_InitFrom(
        &list,
        &value,
        static_cast<size_t>(-1),
        LinkedListIntTypeInfo()));

    CheckIntList(&list, NULL, 0);
}

TEST(LinkedListInitialization, InitFromSupportsDoubleElements)
{
    double values[] = {1.5, 2.5, 3.5};

    CHECK_TRUE(ll_InitFrom(
        &list,
        values,
        3,
        LinkedListDoubleTypeInfo()));

    LinkedListNode_t *current = list._private.head;
    for (size_t i = 0; i < 3; ++i)
    {
        CHECK(current != NULL);
        DOUBLES_EQUAL(values[i], *static_cast<double *>(current->_private.data), 0.000001);
        current = current->_private.next;
    }
}

TEST(LinkedListInitialization, DestroyResetsList)
{
    int values[] = {10, 20, 30};
    CHECK_TRUE(ll_InitFrom(
        &list,
        values,
        3,
        LinkedListIntTypeInfo()));

    ll_Destroy(&list);

    CheckIntList(&list, NULL, 0);
    UNSIGNED_LONGS_EQUAL(0, list.super._private.type.elementSize);
    POINTERS_EQUAL(NULL, list.super._private.type.compare);
    POINTERS_EQUAL(NULL, list.super._private.type.print);
    POINTERS_EQUAL(NULL, list.super._private.type.destroy);
}

TEST(LinkedListInitialization, DestroyHandlesNullAndRepeatedCalls)
{
    CHECK_TRUE(ll_Init(&list, LinkedListIntTypeInfo()));

    ll_Destroy(NULL);
    ll_Destroy(&list);
    ll_Destroy(&list);

    CheckIntList(&list, NULL, 0);
}

/* =========================================================
 * Insertion tests
 * ========================================================= */

TEST_GROUP(LinkedListInsertion)
{
    LinkedList_t list;

    void setup()
    {
        list = LinkedList_t{};
        CHECK_TRUE(ll_Init(&list, LinkedListIntTypeInfo()));
    }

    void teardown()
    {
        ll_Destroy(&list);
    }
};

TEST(LinkedListInsertion, AppendAddsElementToEmptyList)
{
    int value = 10;
    CHECK_TRUE(ll_detail_AppendRef(&list, &value));

    int expected[] = {10};
    CheckIntList(&list, expected, 1);
}

TEST(LinkedListInsertion, AppendPreservesInsertionOrder)
{
    int values[] = {10, 20, 30};

    for (size_t i = 0; i < 3; ++i)
    {
        CHECK_TRUE(ll_detail_AppendRef(&list, &values[i]));
    }

    CheckIntList(&list, values, 3);
}

TEST(LinkedListInsertion, PrependAddsAndShiftsElements)
{
    int twenty = 20;
    int thirty = 30;
    int ten = 10;

    CHECK_TRUE(ll_detail_AppendRef(&list, &twenty));
    CHECK_TRUE(ll_detail_AppendRef(&list, &thirty));
    CHECK_TRUE(ll_detail_PrependRef(&list, &ten));

    int expected[] = {10, 20, 30};
    CheckIntList(&list, expected, 3);
}

TEST(LinkedListInsertion, InsertAddsElementInMiddle)
{
    int ten = 10;
    int thirty = 30;
    int twenty = 20;

    CHECK_TRUE(ll_detail_AppendRef(&list, &ten));
    CHECK_TRUE(ll_detail_AppendRef(&list, &thirty));
    CHECK_TRUE(ll_detail_InsertRef(&list, 1, &twenty));

    int expected[] = {10, 20, 30};
    CheckIntList(&list, expected, 3);
}

TEST(LinkedListInsertion, InsertAtZeroBehavesLikePrepend)
{
    int twenty = 20;
    int ten = 10;

    CHECK_TRUE(ll_detail_AppendRef(&list, &twenty));
    CHECK_TRUE(ll_detail_InsertRef(&list, 0, &ten));

    int expected[] = {10, 20};
    CheckIntList(&list, expected, 2);
}

TEST(LinkedListInsertion, InsertAtSizeBehavesLikeAppend)
{
    int ten = 10;
    int twenty = 20;

    CHECK_TRUE(ll_detail_AppendRef(&list, &ten));
    CHECK_TRUE(ll_detail_InsertRef(&list, list.super._private.size, &twenty));

    int expected[] = {10, 20};
    CheckIntList(&list, expected, 2);
}

TEST(LinkedListInsertion, InsertRejectsInvalidArguments)
{
    int value = 10;
    LinkedList_t invalidList = {};

    CHECK_FALSE(ll_detail_AppendRef(NULL, &value));
    CHECK_FALSE(ll_detail_AppendRef(&list, NULL));
    CHECK_FALSE(ll_detail_AppendRef(&invalidList, &value));
    CHECK_FALSE(ll_detail_PrependRef(NULL, &value));
    CHECK_FALSE(ll_detail_PrependRef(&list, NULL));
    CHECK_FALSE(ll_detail_InsertRef(NULL, 0, &value));
    CHECK_FALSE(ll_detail_InsertRef(&list, 0, NULL));
    CHECK_FALSE(ll_detail_InsertRef(&list, 1, &value));

    CheckIntList(&list, NULL, 0);
}

TEST(LinkedListInsertion, InsertRejectsSizeOverflow)
{
    int value = 10;
    list.super._private.size = static_cast<size_t>(-1);

    CHECK_FALSE(ll_detail_AppendRef(&list, &value));
    CHECK_FALSE(ll_detail_PrependRef(&list, &value));

    POINTERS_EQUAL(NULL, list._private.head);
    POINTERS_EQUAL(NULL, list._private.tail);
    list.super._private.size = 0;
}

TEST(LinkedListInsertion, OperationsMakeIndependentCopies)
{
    int appended = 10;
    int prepended = 5;
    int inserted = 7;

    CHECK_TRUE(ll_detail_AppendRef(&list, &appended));
    CHECK_TRUE(ll_detail_PrependRef(&list, &prepended));
    CHECK_TRUE(ll_detail_InsertRef(&list, 1, &inserted));

    appended = 100;
    prepended = 50;
    inserted = 70;

    int expected[] = {5, 7, 10};
    CheckIntList(&list, expected, 3);
}

TEST(LinkedListInsertion, InsertSupportsDoubleValues)
{
    ll_Destroy(&list);
    CHECK_TRUE(ll_Init(&list, LinkedListDoubleTypeInfo()));

    double first = 1.5;
    double third = 3.5;
    double second = 2.5;

    CHECK_TRUE(ll_detail_AppendRef(&list, &first));
    CHECK_TRUE(ll_detail_AppendRef(&list, &third));
    CHECK_TRUE(ll_detail_InsertRef(&list, 1, &second));

    DOUBLES_EQUAL(1.5, *static_cast<double *>(list._private.head->_private.data), 0.000001);
    DOUBLES_EQUAL(2.5, *static_cast<double *>(list._private.head->_private.next->_private.data), 0.000001);
    DOUBLES_EQUAL(3.5, *static_cast<double *>(list._private.tail->_private.data), 0.000001);
}

/* =========================================================
 * Removal and ownership-transfer tests
 * ========================================================= */

TEST_GROUP(LinkedListRemoval)
{
    LinkedList_t list;

    void setup()
    {
        int values[] = {10, 20, 30, 40};
        list = LinkedList_t{};
        CHECK_TRUE(ll_InitFrom(
            &list,
            values,
            4,
            LinkedListIntTypeInfo()));
    }

    void teardown()
    {
        ll_Destroy(&list);
    }
};

TEST(LinkedListRemoval, RemoveFirstElement)
{
    CHECK_TRUE(ll_Remove(&list, 0));

    int expected[] = {20, 30, 40};
    CheckIntList(&list, expected, 3);
}

TEST(LinkedListRemoval, RemoveMiddleElement)
{
    CHECK_TRUE(ll_Remove(&list, 1));

    int expected[] = {10, 30, 40};
    CheckIntList(&list, expected, 3);
}

TEST(LinkedListRemoval, RemoveLastElement)
{
    CHECK_TRUE(ll_Remove(&list, 3));

    int expected[] = {10, 20, 30};
    CheckIntList(&list, expected, 3);
}

TEST(LinkedListRemoval, RemoveOnlyElementResetsHeadAndTail)
{
    ll_Clear(&list);
    int value = 10;
    CHECK_TRUE(ll_detail_AppendRef(&list, &value));
    CHECK_TRUE(ll_Remove(&list, 0));

    CheckIntList(&list, NULL, 0);
}

TEST(LinkedListRemoval, RemoveRejectsInvalidArguments)
{
    CHECK_FALSE(ll_Remove(NULL, 0));
    CHECK_FALSE(ll_Remove(&list, list.super._private.size));
    CHECK_FALSE(ll_Remove(&list, 100));

    int expected[] = {10, 20, 30, 40};
    CheckIntList(&list, expected, 4);
}

TEST(LinkedListRemoval, TakeRemovesAndReturnsElement)
{
    int taken = 0;
    CHECK_TRUE(ll_Take(&list, 1, &taken));

    LONGS_EQUAL(20, taken);
    int expected[] = {10, 30, 40};
    CheckIntList(&list, expected, 3);
}

TEST(LinkedListRemoval, TakeRejectsInvalidArguments)
{
    int output = 0;

    CHECK_FALSE(ll_Take(NULL, 0, &output));
    CHECK_FALSE(ll_Take(&list, 0, NULL));
    CHECK_FALSE(ll_Take(&list, list.super._private.size, &output));
    CHECK_FALSE(ll_Take(&list, 0, list._private.head->_private.data));

    int expected[] = {10, 20, 30, 40};
    CheckIntList(&list, expected, 4);
}

TEST(LinkedListRemoval, ClearRemovesAllElementsAndAllowsReuse)
{
    ll_Clear(&list);
    CheckIntList(&list, NULL, 0);
    UNSIGNED_LONGS_EQUAL(sizeof(int), list.super._private.type.elementSize);

    int value = 99;
    CHECK_TRUE(ll_detail_AppendRef(&list, &value));

    int expected[] = {99};
    CheckIntList(&list, expected, 1);
}

TEST(LinkedListRemoval, ClearHandlesNullAndEmptyLists)
{
    ll_Clear(NULL);
    ll_Clear(&list);
    ll_Clear(&list);

    CheckIntList(&list, NULL, 0);
}

/* =========================================================
 * Access and lookup tests
 * ========================================================= */

TEST_GROUP(LinkedListAccess)
{
    LinkedList_t list;

    void setup()
    {
        int values[] = {10, 20, 20, 30};
        list = LinkedList_t{};
        CHECK_TRUE(ll_InitFrom(
            &list,
            values,
            4,
            LinkedListIntTypeInfo()));
    }

    void teardown()
    {
        ll_Destroy(&list);
    }
};

TEST(LinkedListAccess, SizeAndIsEmptyReflectState)
{
    UNSIGNED_LONGS_EQUAL(4, adt_Size(&list));
    CHECK_FALSE(adt_IsEmpty(&list));

    ll_Clear(&list);

    UNSIGNED_LONGS_EQUAL(0, adt_Size(&list));
    CHECK_TRUE(adt_IsEmpty(&list));
}

TEST(LinkedListAccess, NullListHasSizeZeroAndIsEmpty)
{
    UNSIGNED_LONGS_EQUAL(0, adt_Size(NULL));
    CHECK_TRUE(adt_IsEmpty(NULL));
}

TEST(LinkedListAccess, GetCopiesElementsFromBothEnds)
{
    int first = 0;
    int last = 0;

    CHECK_TRUE(ll_Get(&list, 0, &first));
    CHECK_TRUE(ll_Get(&list, 3, &last));

    LONGS_EQUAL(10, first);
    LONGS_EQUAL(30, last);
}

TEST(LinkedListAccess, GetRejectsInvalidArguments)
{
    int output = 0;

    CHECK_FALSE(ll_Get(NULL, 0, &output));
    CHECK_FALSE(ll_Get(&list, 0, NULL));
    CHECK_FALSE(ll_Get(&list, list.super._private.size, &output));
}

TEST(LinkedListAccess, SetReplacesElement)
{
    int replacement = 99;
    CHECK_TRUE(ll_detail_SetRef(&list, 2, &replacement));

    int expected[] = {10, 20, 99, 30};
    CheckIntList(&list, expected, 4);
}

TEST(LinkedListAccess, SetRejectsInvalidArguments)
{
    int replacement = 99;

    CHECK_FALSE(ll_detail_SetRef(NULL, 0, &replacement));
    CHECK_FALSE(ll_detail_SetRef(&list, 0, NULL));
    CHECK_FALSE(ll_detail_SetRef(&list, list.super._private.size, &replacement));

    int expected[] = {10, 20, 20, 30};
    CheckIntList(&list, expected, 4);
}

TEST(LinkedListAccess, IndexOfFindsFirstMatchingElement)
{
    int target = 20;
    size_t index = 99;

    CHECK_TRUE(ll_detail_IndexOfRef(&list, &target, &index));
    UNSIGNED_LONGS_EQUAL(1, index);
}

TEST(LinkedListAccess, IndexOfReturnsFalseWithoutChangingOutput)
{
    int target = 99;
    size_t index = 42;

    CHECK_FALSE(ll_detail_IndexOfRef(&list, &target, &index));
    UNSIGNED_LONGS_EQUAL(42, index);
}

TEST(LinkedListAccess, IndexOfRejectsInvalidArguments)
{
    int target = 20;
    size_t index = 0;

    CHECK_FALSE(ll_detail_IndexOfRef(NULL, &target, &index));
    CHECK_FALSE(ll_detail_IndexOfRef(&list, NULL, &index));
    CHECK_FALSE(ll_detail_IndexOfRef(&list, &target, NULL));
}

TEST(LinkedListAccess, ContainsUsesConfiguredComparator)
{
    int present = 30;
    int absent = 99;

    CHECK_TRUE(ll_detail_ContainsRef(&list, &present));
    CHECK_FALSE(ll_detail_ContainsRef(&list, &absent));
    CHECK_FALSE(ll_detail_ContainsRef(NULL, &present));
    CHECK_FALSE(ll_detail_ContainsRef(&list, NULL));
}

TEST(LinkedListAccess, IndexOfFallsBackToByteComparison)
{
    ll_Destroy(&list);

    ADT_TypeInfo_t typeInfo = {
        sizeof(int),
        NULL,
        PrintInt,
        NULL};
    int values[] = {7, 8, 9};

    CHECK_TRUE(ll_InitFrom(&list, values, 3, typeInfo));

    int target = 8;
    size_t index = 0;

    CHECK_TRUE(ll_detail_IndexOfRef(&list, &target, &index));
    UNSIGNED_LONGS_EQUAL(1, index);
}

/* =========================================================
 * Type-info callback tests
 * ========================================================= */

static int linkedListPrintCallCount = 0;
static int linkedListPrintedTotal = 0;
static int linkedListDestroyCallCount = 0;

static void CountLinkedListPrint(const void *element)
{
    linkedListPrintCallCount++;
    linkedListPrintedTotal += *static_cast<const int *>(element);
}

static void CountLinkedListDestroy(void *element)
{
    linkedListDestroyCallCount++;
    int **owned = static_cast<int **>(element);
    free(*owned);
    *owned = NULL;
}

static bool AppendOwnedInt(LinkedList_t *list, int value)
{
    int *owned = static_cast<int *>(malloc(sizeof(int)));

    if (owned == NULL)
    {
        return false;
    }

    *owned = value;
    if (!ll_detail_AppendRef(list, &owned))
    {
        free(owned);
        return false;
    }

    return true;
}

TEST_GROUP(LinkedListTypeInfo)
{
    LinkedList_t list;

    void setup()
    {
        list = LinkedList_t{};
        linkedListPrintCallCount = 0;
        linkedListPrintedTotal = 0;
        linkedListDestroyCallCount = 0;
    }

    void teardown()
    {
        ll_Destroy(&list);
    }

    void initializeOwnedList()
    {
        ADT_TypeInfo_t typeInfo = {
            sizeof(int *),
            NULL,
            NULL,
            CountLinkedListDestroy};
        CHECK_TRUE(ll_Init(&list, typeInfo));
    }
};

TEST(LinkedListTypeInfo, DestroyCallsDestructorForEveryElement)
{
    initializeOwnedList();

    CHECK_TRUE(AppendOwnedInt(&list, 1));
    CHECK_TRUE(AppendOwnedInt(&list, 2));
    CHECK_TRUE(AppendOwnedInt(&list, 3));

    ll_Destroy(&list);

    LONGS_EQUAL(3, linkedListDestroyCallCount);
}

TEST(LinkedListTypeInfo, SetDestroysReplacedElement)
{
    initializeOwnedList();
    CHECK_TRUE(AppendOwnedInt(&list, 10));

    int *replacement = static_cast<int *>(malloc(sizeof(int)));
    CHECK(replacement != NULL);
    *replacement = 20;

    const bool replaced = ll_detail_SetRef(&list, 0, &replacement);
    if (!replaced)
    {
        free(replacement);
    }
    CHECK_TRUE(replaced);

    LONGS_EQUAL(1, linkedListDestroyCallCount);

    ll_Destroy(&list);
    LONGS_EQUAL(2, linkedListDestroyCallCount);
}

TEST(LinkedListTypeInfo, SetFromSameElementDoesNotDestroyIt)
{
    initializeOwnedList();
    CHECK_TRUE(AppendOwnedInt(&list, 10));

    CHECK_TRUE(ll_detail_SetRef(&list, 0, list._private.head->_private.data));
    LONGS_EQUAL(0, linkedListDestroyCallCount);

    ll_Destroy(&list);
    LONGS_EQUAL(1, linkedListDestroyCallCount);
}

TEST(LinkedListTypeInfo, RemoveDestroysOnlyRemovedElement)
{
    initializeOwnedList();
    CHECK_TRUE(AppendOwnedInt(&list, 1));
    CHECK_TRUE(AppendOwnedInt(&list, 2));
    CHECK_TRUE(AppendOwnedInt(&list, 3));

    CHECK_TRUE(ll_Remove(&list, 1));
    LONGS_EQUAL(1, linkedListDestroyCallCount);

    ll_Destroy(&list);
    LONGS_EQUAL(3, linkedListDestroyCallCount);
}

TEST(LinkedListTypeInfo, ClearDestroysEveryElement)
{
    initializeOwnedList();
    CHECK_TRUE(AppendOwnedInt(&list, 1));
    CHECK_TRUE(AppendOwnedInt(&list, 2));
    CHECK_TRUE(AppendOwnedInt(&list, 3));

    ll_Clear(&list);

    LONGS_EQUAL(3, linkedListDestroyCallCount);
    CheckIntList(&list, NULL, 0);

    ll_Destroy(&list);
    LONGS_EQUAL(3, linkedListDestroyCallCount);
}

TEST(LinkedListTypeInfo, TakeTransfersOwnershipWithoutDestroying)
{
    initializeOwnedList();
    CHECK_TRUE(AppendOwnedInt(&list, 10));

    int *taken = NULL;
    CHECK_TRUE(ll_Take(&list, 0, &taken));

    LONGS_EQUAL(0, linkedListDestroyCallCount);
    CHECK(taken != NULL);
    LONGS_EQUAL(10, *taken);
    CheckIntList(&list, NULL, 0);

    free(taken);
}

/* =========================================================
 * Printing tests
 * ========================================================= */

static std::string CaptureLinkedListStdout(void (*action)(void *), void *context)
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
        size_t bytesRead =
            fread(&output[0], 1, static_cast<size_t>(length), capture);
        UNSIGNED_LONGS_EQUAL(static_cast<size_t>(length), bytesRead);
    }

    fclose(capture);
    return output;
}

static void PrintLinkedListAction(void *context)
{
    adt_Print(static_cast<const LinkedList_t *>(context));
}

TEST_GROUP(LinkedListPrinting)
{
    LinkedList_t list;

    void setup()
    {
        list = LinkedList_t{};
        linkedListPrintCallCount = 0;
        linkedListPrintedTotal = 0;
    }

    void teardown()
    {
        ll_Destroy(&list);
    }
};

TEST(LinkedListPrinting, PrintUsesUnifiedContainerFormat)
{
    int values[] = {10, 20, 30};
    CHECK_TRUE(ll_InitFrom(
        &list,
        values,
        3,
        LinkedListIntTypeInfo()));

    std::string output =
        CaptureLinkedListStdout(PrintLinkedListAction, &list);

    STRCMP_EQUAL("LinkedList (size: 3): [10, 20, 30]\n", output.c_str());
}

TEST(LinkedListPrinting, PrintUsesConfiguredPrinterForEveryElement)
{
    ADT_TypeInfo_t typeInfo = {
        sizeof(int),
        CompareInt,
        CountLinkedListPrint,
        NULL};
    int values[] = {10, 20, 30};

    CHECK_TRUE(ll_InitFrom(&list, values, 3, typeInfo));
    CaptureLinkedListStdout(PrintLinkedListAction, &list);

    LONGS_EQUAL(3, linkedListPrintCallCount);
    LONGS_EQUAL(60, linkedListPrintedTotal);
}

TEST(LinkedListPrinting, PrintHandlesEmptyList)
{
    CHECK_TRUE(ll_Init(&list, LinkedListIntTypeInfo()));

    std::string output =
        CaptureLinkedListStdout(PrintLinkedListAction, &list);

    CHECK(output.find("LinkedList (size: 0): []") != std::string::npos);
}

TEST(LinkedListPrinting, PrintHandlesNullList)
{
    std::string output =
        CaptureLinkedListStdout(PrintLinkedListAction, NULL);

    CHECK_TRUE(output.empty());
}

TEST(LinkedListPrinting, PrintHandlesMissingPrinter)
{
    ADT_TypeInfo_t typeInfo = {
        sizeof(int),
        CompareInt,
        NULL,
        NULL};
    CHECK_TRUE(ll_Init(&list, typeInfo));

    std::string output =
        CaptureLinkedListStdout(PrintLinkedListAction, &list);

    CHECK_TRUE(output.empty());
}
