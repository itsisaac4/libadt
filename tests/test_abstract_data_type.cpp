extern "C"
{
#include "libadt/dynamic_array.h"
#include "libadt/linked_list.h"
}

#include "CppUTest/TestHarness.h"
#include <cstddef>
#include <cstdio>
#include <string>
#include <unistd.h>

typedef struct
{
    int values[3];
    size_t indexes[3];
    size_t count;
} VisitContext_t;

static void CollectInt(const void *element, size_t index, void *context)
{
    VisitContext_t *visit = static_cast<VisitContext_t *>(context);
    visit->values[visit->count] = *static_cast<const int *>(element);
    visit->indexes[visit->count] = index;
    visit->count++;
}

static void AddIndexToInt(void *element, size_t index, void *context)
{
    size_t *count = static_cast<size_t *>(context);
    *static_cast<int *>(element) += static_cast<int>(index);
    (*count)++;
}

static int CompareIntDescending(const void *first, const void *second)
{
    return CompareInt(second, first);
}

typedef struct
{
    const ADT_t *adt;
    bool result;
} ADTPrintCall_t;

typedef struct
{
    const ADT_t *adt;
    const char *expression;
    const char *file;
    int line;
} ADTDebugCall_t;

static void CallADTPrint(void *context)
{
    ADTPrintCall_t *call = static_cast<ADTPrintCall_t *>(context);
    call->result = adt_Print(call->adt);
}

static void CallADTPrintDebug(void *context)
{
    ADTDebugCall_t *call = static_cast<ADTDebugCall_t *>(context);
    adt_PrintDebug(call->adt, call->expression, call->file, call->line);
}

static std::string CaptureADTStdout(void (*action)(void *), void *context)
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

TEST_GROUP(AbstractDataType)
{
};

TEST(AbstractDataType, NullSharedStateIsEmpty)
{
    UNSIGNED_LONGS_EQUAL(0, adt_Size(NULL));
    CHECK_TRUE(adt_IsEmpty(NULL));
    POINTERS_EQUAL(NULL, adt_ElementType(NULL));
}

TEST(AbstractDataType, ContainersExposeSharedState)
{
    int values[] = {10, 20, 30};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 3, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 3, type));

    const ADT_Super_t *containers[] = {
        &array.super,
        &list.super};

    POINTERS_EQUAL(
        static_cast<const void *>(&array),
        static_cast<const void *>(&array.super));
    POINTERS_EQUAL(
        static_cast<const void *>(&list),
        static_cast<const void *>(&list.super));

    for (const ADT_Super_t *container : containers)
    {
        UNSIGNED_LONGS_EQUAL(3, adt_Size(container));
        CHECK_FALSE(adt_IsEmpty(container));
        UNSIGNED_LONGS_EQUAL(sizeof(int), adt_ElementType(container)->elementSize);
        FUNCTIONPOINTERS_EQUAL(CompareInt, adt_ElementType(container)->compare);
        FUNCTIONPOINTERS_EQUAL(PrintInt, adt_ElementType(container)->print);
    }

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(AbstractDataType, SharedSizeTracksContainerOperations)
{
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};
    int value = 10;

    CHECK_TRUE(da_Init(&array, type));
    CHECK_TRUE(ll_Init(&list, type));
    CHECK_TRUE(da_detail_AppendRef(&array, &value));
    CHECK_TRUE(ll_detail_AppendRef(&list, &value));

    UNSIGNED_LONGS_EQUAL(1, adt_Size(&array.super));
    UNSIGNED_LONGS_EQUAL(1, adt_Size(&list.super));

    da_Clear(&array);
    ll_Clear(&list);

    CHECK_TRUE(adt_IsEmpty(&array.super));
    CHECK_TRUE(adt_IsEmpty(&list.super));

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(AbstractDataType, DynamicArrayVTableProvidesReadOnlyTraversal)
{
    int values[] = {10, 20, 30};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    VisitContext_t visit = {};

    CHECK_TRUE(da_InitFrom(&array, values, 3, type));
    CHECK(array.super._private.vtable != NULL);
    STRCMP_EQUAL("DynamicArray", array.super._private.vtable->containerName);
    CHECK(array.super._private.vtable->visit != NULL);

    array.super._private.vtable->visit(&array.super, CollectInt, &visit);

    UNSIGNED_LONGS_EQUAL(3, visit.count);
    for (size_t i = 0; i < visit.count; i++)
    {
        LONGS_EQUAL(values[i], visit.values[i]);
        UNSIGNED_LONGS_EQUAL(i, visit.indexes[i]);
    }

    da_Destroy(&array);
}

TEST(AbstractDataType, DynamicArrayVTableProvidesMutableTraversal)
{
    int values[] = {10, 20, 30};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    size_t visits = 0;

    CHECK_TRUE(da_InitFrom(&array, values, 3, type));
    CHECK(array.super._private.vtable != NULL);
    CHECK(array.super._private.vtable->visitMutable != NULL);

    array.super._private.vtable->visitMutable(&array.super, AddIndexToInt, &visits);

    UNSIGNED_LONGS_EQUAL(3, visits);
    for (size_t i = 0; i < array.super._private.size; i++)
    {
        int actual = 0;
        CHECK_TRUE(da_Get(&array, i, &actual));
        LONGS_EQUAL(values[i] + static_cast<int>(i), actual);
    }

    da_Destroy(&array);
}

TEST(AbstractDataType, LinkedListVTableProvidesReadOnlyTraversal)
{
    int values[] = {10, 20, 30};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    LinkedList_t list = {};
    VisitContext_t visit = {};

    CHECK_TRUE(ll_InitFrom(&list, values, 3, type));
    CHECK(list.super._private.vtable != NULL);
    STRCMP_EQUAL("LinkedList", list.super._private.vtable->containerName);
    CHECK(list.super._private.vtable->visit != NULL);

    list.super._private.vtable->visit(&list.super, CollectInt, &visit);

    UNSIGNED_LONGS_EQUAL(3, visit.count);
    for (size_t i = 0; i < visit.count; i++)
    {
        LONGS_EQUAL(values[i], visit.values[i]);
        UNSIGNED_LONGS_EQUAL(i, visit.indexes[i]);
    }

    ll_Destroy(&list);
}

TEST(AbstractDataType, LinkedListVTableProvidesMutableTraversal)
{
    int values[] = {10, 20, 30};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    LinkedList_t list = {};
    size_t visits = 0;

    CHECK_TRUE(ll_InitFrom(&list, values, 3, type));
    CHECK(list.super._private.vtable != NULL);
    CHECK(list.super._private.vtable->visitMutable != NULL);

    list.super._private.vtable->visitMutable(&list.super, AddIndexToInt, &visits);

    UNSIGNED_LONGS_EQUAL(3, visits);
    for (size_t i = 0; i < list.super._private.size; i++)
    {
        int actual = 0;
        CHECK_TRUE(ll_Get(&list, i, &actual));
        LONGS_EQUAL(values[i] + static_cast<int>(i), actual);
    }

    ll_Destroy(&list);
}

TEST(AbstractDataType, SharedDispatcherTraversesEveryContainer)
{
    int values[] = {10, 20, 30};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 3, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 3, type));

    const ADT_t *containers[] = {
        &array,
        &list};

    for (const ADT_t *container : containers)
    {
        VisitContext_t visit = {};
        CHECK_TRUE(adt_ForEach(container, CollectInt, &visit));
        UNSIGNED_LONGS_EQUAL(3, visit.count);

        for (size_t i = 0; i < visit.count; i++)
        {
            LONGS_EQUAL(values[i], visit.values[i]);
            UNSIGNED_LONGS_EQUAL(i, visit.indexes[i]);
        }
    }

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(AbstractDataType, SharedMutableDispatcherTraversesEveryContainer)
{
    int values[] = {10, 20, 30};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 3, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 3, type));

    ADT_t *containers[] = {
        &array,
        &list};

    for (ADT_t *container : containers)
    {
        size_t visits = 0;
        CHECK_TRUE(adt_ForEachMutable(container, AddIndexToInt, &visits));
        UNSIGNED_LONGS_EQUAL(3, visits);
    }

    for (size_t i = 0; i < 3; i++)
    {
        int arrayValue = 0;
        int listValue = 0;

        CHECK_TRUE(da_Get(&array, i, &arrayValue));
        CHECK_TRUE(ll_Get(&list, i, &listValue));
        LONGS_EQUAL(values[i] + static_cast<int>(i), arrayValue);
        LONGS_EQUAL(values[i] + static_cast<int>(i), listValue);
    }

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(AbstractDataType, SharedDispatchersRejectInvalidCalls)
{
    ADT_Super_t unsupported = {};
    DynamicArray_t destroyed = {};
    VisitContext_t visit = {};
    size_t mutableVisits = 0;
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};

    CHECK_TRUE(da_Init(&destroyed, type));
    da_Destroy(&destroyed);

    CHECK_FALSE(adt_ForEach(NULL, CollectInt, &visit));
    CHECK_FALSE(adt_ForEach(&unsupported, CollectInt, &visit));
    CHECK_FALSE(adt_ForEach(&destroyed, CollectInt, &visit));
    CHECK_FALSE(adt_ForEach(&unsupported, NULL, &visit));

    CHECK_FALSE(adt_ForEachMutable(NULL, AddIndexToInt, &mutableVisits));
    CHECK_FALSE(adt_ForEachMutable(&unsupported, AddIndexToInt, &mutableVisits));
    CHECK_FALSE(adt_ForEachMutable(&destroyed, AddIndexToInt, &mutableVisits));
    CHECK_FALSE(adt_ForEachMutable(&unsupported, NULL, &mutableVisits));
}

TEST(AbstractDataType, SharedPrintUsesContainerAndElementPolymorphism)
{
    int values[] = {10, 20, 30};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 3, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 3, type));

    ADTPrintCall_t arrayCall = {&array, false};
    ADTPrintCall_t listCall = {&list, false};
    std::string arrayOutput = CaptureADTStdout(CallADTPrint, &arrayCall);
    std::string listOutput = CaptureADTStdout(CallADTPrint, &listCall);

    CHECK_TRUE(arrayCall.result);
    CHECK_TRUE(listCall.result);
    STRCMP_EQUAL("DynamicArray (size: 3): [10, 20, 30]\n", arrayOutput.c_str());
    STRCMP_EQUAL("LinkedList (size: 3): [10, 20, 30]\n", listOutput.c_str());

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(AbstractDataType, SharedPrintHandlesEmptyContainers)
{
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_Init(&array, type));

    ADTPrintCall_t call = {&array, false};
    std::string output = CaptureADTStdout(CallADTPrint, &call);

    CHECK_TRUE(call.result);
    STRCMP_EQUAL("DynamicArray (size: 0): []\n", output.c_str());

    da_Destroy(&array);
}

TEST(AbstractDataType, SharedPrintRejectsInvalidConfigurationsSilently)
{
    ADT_ElementTypeInfo_t typeWithoutPrinter = {
        sizeof(int),
        CompareInt,
        NULL,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_Init(&array, typeWithoutPrinter));

    ADTPrintCall_t nullCall = {NULL, true};
    ADTPrintCall_t missingPrinterCall = {&array, true};
    std::string nullOutput = CaptureADTStdout(CallADTPrint, &nullCall);
    std::string missingPrinterOutput = CaptureADTStdout(CallADTPrint, &missingPrinterCall);

    CHECK_FALSE(nullCall.result);
    CHECK_FALSE(missingPrinterCall.result);
    CHECK_TRUE(nullOutput.empty());
    CHECK_TRUE(missingPrinterOutput.empty());

    da_Destroy(&array);
}

TEST(AbstractDataType, SharedDebugPrintSupportsLinkedLists)
{
    int values[] = {3, 5, 8};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    LinkedList_t list = {};

    CHECK_TRUE(ll_InitFrom(&list, values, 3, type));

    ADTDebugCall_t call = {&list, "values", "example.c", 27};
    std::string output = CaptureADTStdout(CallADTPrintDebug, &call);

    CHECK(output.find("ADT Debug") != std::string::npos);
    CHECK(output.find("expression: values") != std::string::npos);
    CHECK(output.find("location: example.c:27") != std::string::npos);
    CHECK(output.find("container: LinkedList") != std::string::npos);
    CHECK(output.find("size: 3") != std::string::npos);
    CHECK(output.find("elements: [3, 5, 8]") != std::string::npos);

    ll_Destroy(&list);
}

TEST(AbstractDataType, SharedMinimumSupportsEveryContainer)
{
    int values[] = {8, 3, 5, 2, 9};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 5, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 5, type));

    const ADT_t *containers[] = {
        &array,
        &list};

    for (const ADT_t *container : containers)
    {
        int minimum = 0;
        CHECK_TRUE(adt_Min(container, &minimum));
        LONGS_EQUAL(2, minimum);
    }

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(AbstractDataType, MinimumComparatorCanBeOverriddenPerCall)
{
    int values[] = {8, 3, 5, 2, 9};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    LinkedList_t list = {};
    int minimum = 0;

    CHECK_TRUE(ll_InitFrom(&list, values, 5, type));
    CHECK_TRUE(adt_MinBy(&list, CompareIntDescending, &minimum));
    LONGS_EQUAL(9, minimum);

    ll_Destroy(&list);
}

TEST(AbstractDataType, MinimumOverrideWorksWithoutConfiguredComparator)
{
    int values[] = {8, 3, 5, 2, 9};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        NULL,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    int minimum = 0;

    CHECK_TRUE(da_InitFrom(&array, values, 5, type));
    CHECK_FALSE(adt_Min(&array, &minimum));
    CHECK_TRUE(adt_MinBy(&array, CompareInt, &minimum));
    LONGS_EQUAL(2, minimum);

    da_Destroy(&array);
}

TEST(AbstractDataType, MinimumSupportsOverlappingOutputStorage)
{
    int values[] = {5, 1, 3};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_InitFrom(&array, values, 3, type));
    CHECK_TRUE(adt_Min(&array, array._private.storage.data));
    LONGS_EQUAL(1, static_cast<int *>(array._private.storage.data)[0]);

    da_Destroy(&array);
}

TEST(AbstractDataType, MinimumRejectsInvalidCalls)
{
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t empty = {};
    int minimum = 0;

    CHECK_TRUE(da_Init(&empty, type));

    CHECK_FALSE(adt_Min(NULL, &minimum));
    CHECK_FALSE(adt_Min(&empty, &minimum));
    CHECK_FALSE(adt_Min(&empty, NULL));
    CHECK_FALSE(adt_MinBy(&empty, CompareInt, &minimum));
    CHECK_FALSE(adt_MinBy(&empty, NULL, &minimum));

    da_Destroy(&empty);
}

TEST(AbstractDataType, SharedMaximumSupportsEveryContainer)
{
    int values[] = {8, 3, 5, 2, 9};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    LinkedList_t list = {};

    CHECK_TRUE(da_InitFrom(&array, values, 5, type));
    CHECK_TRUE(ll_InitFrom(&list, values, 5, type));

    const ADT_t *containers[] = {
        &array,
        &list};

    for (const ADT_t *container : containers)
    {
        int maximum = 0;
        CHECK_TRUE(adt_Max(container, &maximum));
        LONGS_EQUAL(9, maximum);
    }

    da_Destroy(&array);
    ll_Destroy(&list);
}

TEST(AbstractDataType, MaximumComparatorCanBeOverriddenPerCall)
{
    int values[] = {8, 3, 5, 2, 9};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    LinkedList_t list = {};
    int maximum = 0;

    CHECK_TRUE(ll_InitFrom(&list, values, 5, type));
    CHECK_TRUE(adt_MaxBy(&list, CompareIntDescending, &maximum));
    LONGS_EQUAL(2, maximum);

    ll_Destroy(&list);
}

TEST(AbstractDataType, MaximumOverrideWorksWithoutConfiguredComparator)
{
    int values[] = {8, 3, 5, 2, 9};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        NULL,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};
    int maximum = 0;

    CHECK_TRUE(da_InitFrom(&array, values, 5, type));
    CHECK_FALSE(adt_Max(&array, &maximum));
    CHECK_TRUE(adt_MaxBy(&array, CompareInt, &maximum));
    LONGS_EQUAL(9, maximum);

    da_Destroy(&array);
}

TEST(AbstractDataType, MaximumSupportsOverlappingOutputStorage)
{
    int values[] = {5, 9, 3};
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    DynamicArray_t array = {};

    CHECK_TRUE(da_InitFrom(&array, values, 3, type));
    CHECK_TRUE(adt_Max(&array, array._private.storage.data));
    LONGS_EQUAL(9, static_cast<int *>(array._private.storage.data)[0]);

    da_Destroy(&array);
}

TEST(AbstractDataType, MaximumRejectsInvalidCalls)
{
    ADT_ElementTypeInfo_t type = {
        sizeof(int),
        CompareInt,
        PrintInt,
        ToNumberInt,
        NULL};
    LinkedList_t empty = {};
    int maximum = 0;

    CHECK_TRUE(ll_Init(&empty, type));

    CHECK_FALSE(adt_Max(NULL, &maximum));
    CHECK_FALSE(adt_Max(&empty, &maximum));
    CHECK_FALSE(adt_Max(&empty, NULL));
    CHECK_FALSE(adt_MaxBy(&empty, CompareInt, &maximum));
    CHECK_FALSE(adt_MaxBy(&empty, NULL, &maximum));

    ll_Destroy(&empty);
}
