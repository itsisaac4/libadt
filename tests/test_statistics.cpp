extern "C"
{
#include "libadt/dynamic_array.h"
#include "libadt/linked_list.h"
}

#include <cfloat>
#include <cstdint>

#include "CppUTest/TestHarness.h"

typedef struct
{
    double score;
    int age;
} StatisticRecord_t;

static ADT_ElementTypeInfo_t IntElementType()
{
    ADT_ElementTypeInfo_t elementType = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    return elementType;
}

static double RecordScoreToNumber(const void *element)
{
    return static_cast<const StatisticRecord_t *>(element)->score;
}

static double RecordAgeToNumber(const void *element)
{
    return (double)static_cast<const StatisticRecord_t *>(element)->age;
}

TEST_GROUP(SharedStatistics)
{
    DynamicArray_t array;
    LinkedList_t list;

    void setup()
    {
        array = DynamicArray_t{};
        list = LinkedList_t{};
    }

    void teardown()
    {
        da_Destroy(&array);
        ll_Destroy(&list);
    }

    void initializeInts(const int *values, size_t count)
    {
        ADT_ElementTypeInfo_t elementType = IntElementType();
        CHECK_TRUE(da_InitFrom(&array, values, count, elementType));
        CHECK_TRUE(ll_InitFrom(&list, values, count, elementType));
    }
};

TEST(SharedStatistics, MeanMedianAndModeSupportEveryContainer)
{
    int values[] = {1, 2, 2, 4, 6};
    initializeInts(values, 5);

    const ADT_t *containers[] = {&array, &list};

    for (const ADT_t *container : containers)
    {
        double mean = 0.0;
        double median = 0.0;
        double mode = 0.0;

        CHECK_TRUE(adt_Mean(container, &mean));
        CHECK_TRUE(adt_Median(container, &median));
        CHECK_TRUE(adt_Mode(container, &mode));
        DOUBLES_EQUAL(3.0, mean, 0.000001);
        DOUBLES_EQUAL(2.0, median, 0.000001);
        DOUBLES_EQUAL(2.0, mode, 0.000001);
    }
}

TEST(SharedStatistics, MedianAveragesTheTwoMiddleValues)
{
    int values[] = {9, 1, 5, 3};
    initializeInts(values, 4);

    double arrayMedian = 0.0;
    double listMedian = 0.0;

    CHECK_TRUE(adt_Median(&array, &arrayMedian));
    CHECK_TRUE(adt_Median(&list, &listMedian));
    DOUBLES_EQUAL(4.0, arrayMedian, 0.000001);
    DOUBLES_EQUAL(4.0, listMedian, 0.000001);
}

TEST(SharedStatistics, MedianAvoidsOverflowWhenAveraging)
{
    double values[] = {DBL_MAX, DBL_MAX};
    ADT_ElementTypeInfo_t elementType = {
        sizeof(double),
        CompareDouble,
        PrintDouble,
        ToNumberDouble,
        NULL};
    double median = 0.0;

    CHECK_TRUE(da_InitFrom(&array, values, 2, elementType));
    CHECK_TRUE(adt_Median(&array, &median));
    DOUBLES_EQUAL(DBL_MAX, median, 0.0);
}

TEST(SharedStatistics, ModeUsesTheSmallestValueWhenFrequenciesTie)
{
    int values[] = {3, 3, 2, 2, 5};
    initializeInts(values, 5);

    double arrayMode = 0.0;
    double listMode = 0.0;

    CHECK_TRUE(adt_Mode(&array, &arrayMode));
    CHECK_TRUE(adt_Mode(&list, &listMode));
    DOUBLES_EQUAL(2.0, arrayMode, 0.0);
    DOUBLES_EQUAL(2.0, listMode, 0.0);
}

TEST(SharedStatistics, ModeRejectsDataWithoutRepeatedValues)
{
    int values[] = {1, 2, 3, 4};
    initializeInts(values, 4);

    double arrayMode = 91.0;
    double listMode = 92.0;

    CHECK_FALSE(adt_Mode(&array, &arrayMode));
    CHECK_FALSE(adt_Mode(&list, &listMode));
    DOUBLES_EQUAL(91.0, arrayMode, 0.0);
    DOUBLES_EQUAL(92.0, listMode, 0.0);
}

TEST(SharedStatistics, MedianAndModeDoNotReorderContainers)
{
    int values[] = {4, 1, 4, 2};
    initializeInts(values, 4);

    double result = 0.0;
    CHECK_TRUE(adt_Median(&array, &result));
    CHECK_TRUE(adt_Mode(&array, &result));
    CHECK_TRUE(adt_Median(&list, &result));
    CHECK_TRUE(adt_Mode(&list, &result));

    for (size_t i = 0; i < 4; i++)
    {
        int arrayValue = 0;
        int listValue = 0;

        CHECK_TRUE(da_Get(&array, i, &arrayValue));
        CHECK_TRUE(ll_Get(&list, i, &listValue));
        LONGS_EQUAL(values[i], arrayValue);
        LONGS_EQUAL(values[i], listValue);
    }
}

TEST(SharedStatistics, ProjectionOverridesSupportAlternateCustomFields)
{
    StatisticRecord_t values[] = {
        {.score = 10.0, .age = 20},
        {.score = 20.0, .age = 20},
        {.score = 20.0, .age = 30},
        {.score = 30.0, .age = 30},
        {.score = 40.0, .age = 30}};
    ADT_ElementTypeInfo_t elementType = {
        sizeof(StatisticRecord_t),
        NULL,
        NULL,
        RecordScoreToNumber,
        NULL};

    CHECK_TRUE(da_InitFrom(&array, values, 5, elementType));
    CHECK_TRUE(ll_InitFrom(&list, values, 5, elementType));

    const ADT_t *containers[] = {&array, &list};

    for (const ADT_t *container : containers)
    {
        double result = 0.0;

        CHECK_TRUE(adt_Mean(container, &result));
        DOUBLES_EQUAL(24.0, result, 0.000001);
        CHECK_TRUE(adt_Median(container, &result));
        DOUBLES_EQUAL(20.0, result, 0.000001);
        CHECK_TRUE(adt_Mode(container, &result));
        DOUBLES_EQUAL(20.0, result, 0.000001);

        CHECK_TRUE(adt_MeanBy(container, RecordAgeToNumber, &result));
        DOUBLES_EQUAL(26.0, result, 0.000001);
        CHECK_TRUE(adt_MedianBy(container, RecordAgeToNumber, &result));
        DOUBLES_EQUAL(30.0, result, 0.000001);
        CHECK_TRUE(adt_ModeBy(container, RecordAgeToNumber, &result));
        DOUBLES_EQUAL(30.0, result, 0.000001);
    }
}

TEST(SharedStatistics, OverridesWorkWithoutConfiguredProjection)
{
    int values[] = {1, 2, 2, 3};
    ADT_ElementTypeInfo_t elementType = {
        sizeof(int),
        CompareInt,
        PrintInt,
        NULL,
        NULL};
    double result = 0.0;

    CHECK_TRUE(da_InitFrom(&array, values, 4, elementType));
    CHECK_FALSE(adt_Mean(&array, &result));
    CHECK_FALSE(adt_Median(&array, &result));
    CHECK_FALSE(adt_Mode(&array, &result));
    CHECK_TRUE(adt_MeanBy(&array, ToNumberInt, &result));
    CHECK_TRUE(adt_MedianBy(&array, ToNumberInt, &result));
    CHECK_TRUE(adt_ModeBy(&array, ToNumberInt, &result));
}

TEST(SharedStatistics, RejectInvalidArgumentsWithoutChangingOutputs)
{
    int value = 5;
    initializeInts(&value, 1);

    double result = 73.0;

    CHECK_FALSE(adt_Mean(NULL, &result));
    CHECK_FALSE(adt_Mean(&array, NULL));
    CHECK_FALSE(adt_MeanBy(&array, NULL, &result));
    CHECK_FALSE(adt_Median(NULL, &result));
    CHECK_FALSE(adt_Median(&array, NULL));
    CHECK_FALSE(adt_MedianBy(&array, NULL, &result));
    CHECK_FALSE(adt_Mode(NULL, &result));
    CHECK_FALSE(adt_Mode(&array, NULL));
    CHECK_FALSE(adt_ModeBy(&array, NULL, &result));
    DOUBLES_EQUAL(73.0, result, 0.0);
}

TEST(SharedStatistics, RejectEmptyContainers)
{
    ADT_ElementTypeInfo_t elementType = IntElementType();
    double result = 81.0;

    CHECK_TRUE(da_Init(&array, elementType));
    CHECK_TRUE(ll_Init(&list, elementType));
    CHECK_FALSE(adt_Mean(&array, &result));
    CHECK_FALSE(adt_Median(&array, &result));
    CHECK_FALSE(adt_Mode(&array, &result));
    CHECK_FALSE(adt_Mean(&list, &result));
    CHECK_FALSE(adt_Median(&list, &result));
    CHECK_FALSE(adt_Mode(&list, &result));
    DOUBLES_EQUAL(81.0, result, 0.0);
}

TEST(SharedStatistics, RejectNumericBufferSizeOverflow)
{
    ADT_ElementTypeInfo_t elementType = IntElementType();
    double result = 0.0;

    CHECK_TRUE(da_Init(&array, elementType));
    array.super._private.size = SIZE_MAX;

    CHECK_FALSE(adt_Median(&array, &result));
    CHECK_FALSE(adt_Mode(&array, &result));

    array.super._private.size = 0;
}
