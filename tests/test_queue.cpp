extern "C"
{
#include "libadt/queue.h"
}

#include "CppUTest/TestHarness.h"
#include <cstdlib>
#include <cstring>

extern "C"
{
    bool C23QueueInitInfersIntType(void);
    bool C23QueueInitFromInfersIntType(void);
}

typedef struct
{
    int value;
    char *resource;
} QueueOwnedValue_t;

static size_t queueDestroyCount;

static ADT_ElementTypeInfo_t QueueIntType()
{
    return ADT_ElementTypeInfo_t{
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
}

static char *CopyQueueString(const char *text)
{
    const size_t size = strlen(text) + 1;
    char *copy = static_cast<char *>(malloc(size));
    CHECK(copy != NULL);
    memcpy(copy, text, size);
    return copy;
}

static void DestroyQueueOwnedValue(void *element)
{
    QueueOwnedValue_t *value = static_cast<QueueOwnedValue_t *>(element);
    free(value->resource);
    value->resource = NULL;
    queueDestroyCount++;
}

TEST_GROUP(Queue)
{
    Queue_t queue;

    void setup()
    {
        queue = Queue_t{};
        queueDestroyCount = 0;
    }

    void teardown()
    {
        qu_Destroy(&queue);
    }
};

TEST(Queue, InitCreatesEmptyPolymorphicQueue)
{
    CHECK_TRUE(qu_Init(&queue, QueueIntType()));
    CHECK_TRUE(adt_IsEmpty(&queue));
    POINTERS_EQUAL(static_cast<void *>(&queue), static_cast<void *>(&queue.super));
    STRCMP_EQUAL("Queue", queue.super._private.vtable->containerName);
    POINTERS_EQUAL(NULL, queue._private.storage.head);
    POINTERS_EQUAL(NULL, queue._private.storage.tail);
}

TEST(Queue, InitMacrosInferPrimitiveType)
{
    CHECK_TRUE(C23QueueInitInfersIntType());
    CHECK_TRUE(C23QueueInitFromInfersIntType());
}

TEST(Queue, InitFromTreatsValuesAsFrontToBack)
{
    int values[] = {10, 20, 30};
    CHECK_TRUE(qu_InitFrom(&queue, values, 3, QueueIntType()));

    values[0] = 99;
    int front = 0;
    int back = 0;
    CHECK_TRUE(qu_Front(&queue, &front));
    CHECK_TRUE(qu_Back(&queue, &back));
    LONGS_EQUAL(10, front);
    LONGS_EQUAL(30, back);
    UNSIGNED_LONGS_EQUAL(3, adt_Size(&queue));
}

TEST(Queue, InitAndInitFromRejectInvalidArguments)
{
    int value = 1;
    ADT_ElementTypeInfo_t invalid = {};

    CHECK_FALSE(qu_Init(NULL, QueueIntType()));
    CHECK_FALSE(qu_Init(&queue, invalid));
    CHECK_FALSE(qu_InitFrom(NULL, &value, 1, QueueIntType()));
    CHECK_FALSE(qu_InitFrom(&queue, NULL, 1, QueueIntType()));
    CHECK_FALSE(qu_InitFrom(&queue, &value, 1, invalid));
    CHECK_FALSE(qu_InitFrom(
        &queue,
        &value,
        static_cast<size_t>(-1),
        QueueIntType()));
}

TEST(Queue, EnqueueFrontBackAndDequeueUseFifoOrder)
{
    int first = 1;
    int second = 2;
    int third = 3;
    CHECK_TRUE(qu_Init(&queue, QueueIntType()));
    CHECK_TRUE(qu_detail_EnqueueRef(&queue, &first));
    CHECK_TRUE(qu_detail_EnqueueRef(&queue, &second));
    CHECK_TRUE(qu_detail_EnqueueRef(&queue, &third));

    int actual = 0;
    CHECK_TRUE(qu_Front(&queue, &actual));
    LONGS_EQUAL(1, actual);
    CHECK_TRUE(qu_Back(&queue, &actual));
    LONGS_EQUAL(3, actual);
    CHECK_TRUE(qu_Dequeue(&queue, &actual));
    LONGS_EQUAL(1, actual);
    CHECK_TRUE(qu_Dequeue(&queue, &actual));
    LONGS_EQUAL(2, actual);
    CHECK_TRUE(qu_Dequeue(&queue, &actual));
    LONGS_EQUAL(3, actual);
    CHECK_TRUE(adt_IsEmpty(&queue));
}

TEST(Queue, EmptyOperationsAndInvalidEnqueuesFail)
{
    int value = 0;
    CHECK_TRUE(qu_Init(&queue, QueueIntType()));

    CHECK_FALSE(qu_Front(&queue, &value));
    CHECK_FALSE(qu_Back(&queue, &value));
    CHECK_FALSE(qu_Dequeue(&queue, &value));
    CHECK_FALSE(qu_Discard(&queue));
    CHECK_FALSE(qu_detail_EnqueueRef(NULL, &value));
    CHECK_FALSE(qu_detail_EnqueueRef(&queue, NULL));
    CHECK_FALSE(qu_Front(NULL, &value));
    CHECK_FALSE(qu_Front(&queue, NULL));
}

TEST(Queue, PrimitiveWrapperRejectsDifferentSizedType)
{
    CHECK_TRUE(qu_Init(&queue, QueueIntType()));
    CHECK_FALSE(qu_detail_EnqueueDouble(&queue, 4.5));
    CHECK_TRUE(adt_IsEmpty(&queue));
}

TEST(Queue, DequeueTransfersResourcesWithoutDestroyingThem)
{
    ADT_ElementTypeInfo_t type = {
        sizeof(QueueOwnedValue_t),
        NULL,
        NULL,
        NULL,
        DestroyQueueOwnedValue};
    QueueOwnedValue_t value = {7, CopyQueueString("owned")};
    QueueOwnedValue_t taken = {};

    CHECK_TRUE(qu_Init(&queue, type));
    CHECK_TRUE(qu_detail_EnqueueRef(&queue, &value));
    CHECK_TRUE(qu_Dequeue(&queue, &taken));
    UNSIGNED_LONGS_EQUAL(0, queueDestroyCount);
    STRCMP_EQUAL("owned", taken.resource);

    DestroyQueueOwnedValue(&taken);
    UNSIGNED_LONGS_EQUAL(1, queueDestroyCount);
}

TEST(Queue, DiscardAndClearDestroyOwnedResources)
{
    ADT_ElementTypeInfo_t type = {
        sizeof(QueueOwnedValue_t),
        NULL,
        NULL,
        NULL,
        DestroyQueueOwnedValue};
    QueueOwnedValue_t first = {1, CopyQueueString("first")};
    QueueOwnedValue_t second = {2, CopyQueueString("second")};

    CHECK_TRUE(qu_Init(&queue, type));
    CHECK_TRUE(qu_detail_EnqueueRef(&queue, &first));
    CHECK_TRUE(qu_detail_EnqueueRef(&queue, &second));
    CHECK_TRUE(qu_Discard(&queue));
    UNSIGNED_LONGS_EQUAL(1, queueDestroyCount);

    qu_Clear(&queue);
    UNSIGNED_LONGS_EQUAL(2, queueDestroyCount);
    CHECK_TRUE(adt_IsEmpty(&queue));
    POINTERS_EQUAL(NULL, queue._private.storage.head);
    POINTERS_EQUAL(NULL, queue._private.storage.tail);
    CHECK(queue.super._private.elementType.destroy == DestroyQueueOwnedValue);
}

TEST(Queue, DequeueRejectsOutputInsideOwnedStorage)
{
    int value = 1;
    CHECK_TRUE(qu_Init(&queue, QueueIntType()));
    CHECK_TRUE(qu_detail_EnqueueRef(&queue, &value));

    CHECK_FALSE(qu_Dequeue(&queue, queue._private.storage.head->data));
    UNSIGNED_LONGS_EQUAL(1, adt_Size(&queue));
}

TEST(Queue, SharedStatisticsAndSortingUseFrontToBackTraversal)
{
    int values[] = {4, 1, 2, 2};
    CHECK_TRUE(qu_InitFrom(&queue, values, 4, QueueIntType()));

    double mean = 0.0;
    double median = 0.0;
    double mode = 0.0;
    CHECK_TRUE(adt_Mean(&queue, &mean));
    CHECK_TRUE(adt_Median(&queue, &median));
    CHECK_TRUE(adt_Mode(&queue, &mode));
    DOUBLES_EQUAL(2.25, mean, 0.000001);
    DOUBLES_EQUAL(2.0, median, 0.000001);
    DOUBLES_EQUAL(2.0, mode, 0.000001);

    CHECK_TRUE(adt_Sort(&queue, ADT_SORT_QUICK));
    int front = 0;
    int back = 0;
    CHECK_TRUE(qu_Front(&queue, &front));
    CHECK_TRUE(qu_Back(&queue, &back));
    LONGS_EQUAL(1, front);
    LONGS_EQUAL(4, back);
}

TEST(Queue, DestroyResetsStateAndSupportsRepeatedCalls)
{
    CHECK_TRUE(qu_Init(&queue, QueueIntType()));
    qu_Destroy(&queue);
    qu_Destroy(&queue);
    qu_Destroy(NULL);

    POINTERS_EQUAL(NULL, queue._private.storage.head);
    POINTERS_EQUAL(NULL, queue._private.storage.tail);
    UNSIGNED_LONGS_EQUAL(0, adt_Size(&queue));
    UNSIGNED_LONGS_EQUAL(0, queue.super._private.elementType.elementSize);
}
