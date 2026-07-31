#include <stdbool.h>

#include "libadt/libadt.h"

typedef struct
{
    int value;
} MacroStruct;

typedef int (*MacroFunctionFn_t)(int value);

static int Increment(int value)
{
    return value + 1;
}

static double MacroStructToNumber(const void *element)
{
    return (double)((const MacroStruct *)element)->value;
}

bool C23UnknownTypeUsesSafeDefaults(void)
{
    DynamicArray_t array = {0};

    if (!DA_INIT(&array, MacroStruct))
    {
        return false;
    }

    const bool usesSafeDefaults =
        array.super._private.elementType.compare == NULL &&
        array.super._private.elementType.print == NULL &&
        array.super._private.elementType.toNumber == NULL;

    MacroStruct value = {.value = 42};
    MacroStruct target = {.value = 42};
    const bool supportsByteComparison =
        da_Append(&array, &value) &&
        da_Contains(&array, &target);

    da_Destroy(&array);
    return usesSafeDefaults && supportsByteComparison;
}

bool C23InitFromInfersIntType(void)
{
    int values[] = {10, 20, 30};
    DynamicArray_t array = {0};

    if (!DA_INIT_FROM(&array, values))
    {
        return false;
    }

    const bool inferredInt =
        array.super._private.elementType.elementSize == sizeof(int) &&
        array.super._private.elementType.compare == CompareInt &&
        array.super._private.elementType.print == PrintInt &&
        array.super._private.elementType.toNumber == ToNumberInt &&
        array.super._private.size == ARRAY_COUNT(values);

    da_Destroy(&array);
    return inferredInt;
}

bool C23LinkedListInitInfersIntType(void)
{
    LinkedList_t list = {0};

    if (!LL_INIT(&list, int))
    {
        return false;
    }

    const bool inferredInt =
        list.super._private.elementType.elementSize == sizeof(int) &&
        list.super._private.elementType.compare == CompareInt &&
        list.super._private.elementType.print == PrintInt &&
        list.super._private.elementType.toNumber == ToNumberInt &&
        list.super._private.size == 0;

    ll_Destroy(&list);
    return inferredInt;
}

bool C23LinkedListInitFromInfersIntType(void)
{
    int values[] = {10, 20, 30};
    LinkedList_t list = {0};

    if (!LL_INIT_FROM(&list, values))
    {
        return false;
    }

    const bool inferredInt =
        list.super._private.elementType.elementSize == sizeof(int) &&
        list.super._private.elementType.compare == CompareInt &&
        list.super._private.elementType.print == PrintInt &&
        list.super._private.elementType.toNumber == ToNumberInt &&
        list.super._private.size == ARRAY_COUNT(values);

    ll_Destroy(&list);
    return inferredInt;
}

bool C23StackInitInfersIntType(void)
{
    Stack_t stack = {0};

    if (!ST_INIT(&stack, int))
    {
        return false;
    }

    const bool inferredInt =
        stack.super._private.elementType.elementSize == sizeof(int) &&
        stack.super._private.elementType.compare == CompareInt &&
        stack.super._private.elementType.print == PrintInt &&
        stack.super._private.elementType.toNumber == ToNumberInt &&
        stack.super._private.size == 0;

    st_Destroy(&stack);
    return inferredInt;
}

bool C23StackInitFromInfersIntType(void)
{
    int values[] = {10, 20, 30};
    Stack_t stack = {0};

    if (!ST_INIT_FROM(&stack, values))
    {
        return false;
    }

    int top = 0;
    const bool inferredInt =
        stack.super._private.elementType.elementSize == sizeof(int) &&
        stack.super._private.elementType.compare == CompareInt &&
        stack.super._private.elementType.print == PrintInt &&
        stack.super._private.elementType.toNumber == ToNumberInt &&
        stack.super._private.size == ARRAY_COUNT(values) &&
        st_Peek(&stack, &top) &&
        top == 30;

    st_Destroy(&stack);
    return inferredInt;
}

bool C23QueueInitInfersIntType(void)
{
    Queue_t queue = {0};

    if (!QU_INIT(&queue, int))
    {
        return false;
    }

    const bool inferredInt =
        queue.super._private.elementType.elementSize == sizeof(int) &&
        queue.super._private.elementType.compare == CompareInt &&
        queue.super._private.elementType.print == PrintInt &&
        queue.super._private.elementType.toNumber == ToNumberInt &&
        queue.super._private.size == 0;

    qu_Destroy(&queue);
    return inferredInt;
}

bool C23QueueInitFromInfersIntType(void)
{
    int values[] = {10, 20, 30};
    Queue_t queue = {0};

    if (!QU_INIT_FROM(&queue, values))
    {
        return false;
    }

    int front = 0;
    int back = 0;
    const bool inferredInt =
        queue.super._private.elementType.elementSize == sizeof(int) &&
        queue.super._private.elementType.compare == CompareInt &&
        queue.super._private.elementType.print == PrintInt &&
        queue.super._private.elementType.toNumber == ToNumberInt &&
        queue.super._private.size == ARRAY_COUNT(values) &&
        qu_Front(&queue, &front) &&
        qu_Back(&queue, &back) &&
        front == 10 &&
        back == 30;

    qu_Destroy(&queue);
    return inferredInt;
}

#define DEFINE_PRIMITIVE_DISPATCH_CHECK(Suffix, Type)                  \
    static bool CheckDynamicArray##Suffix(void)                         \
    {                                                                  \
        const ADT_ElementTypeInfo_t type = {                           \
            .elementSize = sizeof(Type),                               \
            .compare = Compare##Suffix,                                \
            .print = Print##Suffix,                                    \
            .toNumber = ToNumber##Suffix,                              \
            .destroy = NULL};                                          \
        DynamicArray_t array = {0};                                    \
        Type value = (Type)7;                                          \
                                                                       \
        if (!da_Init(&array, type))                                    \
        {                                                              \
            return false;                                              \
        }                                                              \
                                                                       \
        const bool works =                                             \
            da_Append(&array, value) &&                                \
            da_Contains(&array, value);                                \
        da_Destroy(&array);                                            \
        return works;                                                  \
    }                                                                  \
                                                                       \
    static bool CheckLinkedList##Suffix(void)                          \
    {                                                                  \
        const ADT_ElementTypeInfo_t type = {                           \
            .elementSize = sizeof(Type),                               \
            .compare = Compare##Suffix,                                \
            .print = Print##Suffix,                                    \
            .toNumber = ToNumber##Suffix,                              \
            .destroy = NULL};                                          \
        LinkedList_t list = {0};                                       \
        Type value = (Type)7;                                          \
                                                                       \
        if (!ll_Init(&list, type))                                     \
        {                                                              \
            return false;                                              \
        }                                                              \
                                                                       \
        const bool works =                                             \
            ll_Append(&list, value) &&                                 \
            ll_Contains(&list, value);                                 \
        ll_Destroy(&list);                                             \
        return works;                                                  \
    }                                                                  \
                                                                       \
    static bool CheckStack##Suffix(void)                               \
    {                                                                  \
        const ADT_ElementTypeInfo_t type = {                           \
            .elementSize = sizeof(Type),                               \
            .compare = Compare##Suffix,                                \
            .print = Print##Suffix,                                    \
            .toNumber = ToNumber##Suffix,                              \
            .destroy = NULL};                                          \
        Stack_t stack = {0};                                           \
        Type value = (Type)7;                                          \
        Type actual = (Type)0;                                         \
                                                                       \
        if (!st_Init(&stack, type))                                    \
        {                                                              \
            return false;                                              \
        }                                                              \
                                                                       \
        const bool works =                                             \
            st_Push(&stack, value) &&                                  \
            st_Pop(&stack, &actual) &&                                 \
            actual == value;                                           \
        st_Destroy(&stack);                                            \
        return works;                                                  \
    }                                                                  \
                                                                       \
    static bool CheckQueue##Suffix(void)                               \
    {                                                                  \
        const ADT_ElementTypeInfo_t type = {                           \
            .elementSize = sizeof(Type),                               \
            .compare = Compare##Suffix,                                \
            .print = Print##Suffix,                                    \
            .toNumber = ToNumber##Suffix,                              \
            .destroy = NULL};                                          \
        Queue_t queue = {0};                                           \
        Type value = (Type)7;                                          \
        Type actual = (Type)0;                                         \
                                                                       \
        if (!qu_Init(&queue, type))                                    \
        {                                                              \
            return false;                                              \
        }                                                              \
                                                                       \
        const bool works =                                             \
            qu_Enqueue(&queue, value) &&                               \
            qu_Dequeue(&queue, &actual) &&                             \
            actual == value;                                           \
        qu_Destroy(&queue);                                            \
        return works;                                                  \
    }
DEFINE_PRIMITIVE_DISPATCH_CHECK(Char, char)
DEFINE_PRIMITIVE_DISPATCH_CHECK(Int, int)
DEFINE_PRIMITIVE_DISPATCH_CHECK(UnsignedInt, unsigned int)
DEFINE_PRIMITIVE_DISPATCH_CHECK(Long, long)
DEFINE_PRIMITIVE_DISPATCH_CHECK(Float, float)
DEFINE_PRIMITIVE_DISPATCH_CHECK(Double, double)
#undef DEFINE_PRIMITIVE_DISPATCH_CHECK

bool C23PrimitiveTypesDispatchByValue(void)
{
#define CHECK_PRIMITIVE_DISPATCH(Suffix, Type) \
    if (!CheckDynamicArray##Suffix() ||                             \
        !CheckLinkedList##Suffix() ||                               \
        !CheckStack##Suffix() ||                                    \
        !CheckQueue##Suffix())                                      \
    {                                                              \
        return false;                                              \
    }
    CHECK_PRIMITIVE_DISPATCH(Char, char)
    CHECK_PRIMITIVE_DISPATCH(Int, int)
    CHECK_PRIMITIVE_DISPATCH(UnsignedInt, unsigned int)
    CHECK_PRIMITIVE_DISPATCH(Long, long)
    CHECK_PRIMITIVE_DISPATCH(Float, float)
    CHECK_PRIMITIVE_DISPATCH(Double, double)
#undef CHECK_PRIMITIVE_DISPATCH

    return true;
}

bool C23PrimitiveOperationsDispatchByValue(void)
{
    const ADT_ElementTypeInfo_t type = {
        .elementSize = sizeof(int),
        .compare = CompareInt,
        .print = PrintInt,
        .toNumber = ToNumberInt,
        .destroy = NULL};
    DynamicArray_t array = {0};
    LinkedList_t list = {0};
    Stack_t stack = {0};
    Queue_t queue = {0};
    size_t arrayIndex = 0;
    size_t listIndex = 0;

    if (!da_Init(&array, type) ||
        !ll_Init(&list, type) ||
        !st_Init(&stack, type) ||
        !qu_Init(&queue, type))
    {
        da_Destroy(&array);
        ll_Destroy(&list);
        st_Destroy(&stack);
        qu_Destroy(&queue);
        return false;
    }

    const bool arrayWorks =
        da_Append(&array, 2) &&
        da_Prepend(&array, 1) &&
        da_Insert(&array, 1, 3) &&
        da_Set(&array, 2, 4) &&
        da_Contains(&array, 3) &&
        da_IndexOf(&array, 4, &arrayIndex) &&
        arrayIndex == 2;

    const bool listWorks =
        ll_Append(&list, 2) &&
        ll_Prepend(&list, 1) &&
        ll_Insert(&list, 1, 3) &&
        ll_Set(&list, 2, 4) &&
        ll_Contains(&list, 3) &&
        ll_IndexOf(&list, 4, &listIndex) &&
        listIndex == 2;

    int stackValue = 0;
    const bool stackWorks =
        st_Push(&stack, 3) &&
        st_Pop(&stack, &stackValue) &&
        stackValue == 3;

    int queueValue = 0;
    const bool queueWorks =
        qu_Enqueue(&queue, 4) &&
        qu_Dequeue(&queue, &queueValue) &&
        queueValue == 4;

    da_Destroy(&array);
    ll_Destroy(&list);
    st_Destroy(&stack);
    qu_Destroy(&queue);
    return arrayWorks && listWorks && stackWorks && queueWorks;
}

bool C23ElementTypeInfoConstructorDefinesAllCapabilities(void)
{
    const ADT_ElementTypeInfo_t type = ADT_ELEMENT_TYPE_INFO(
        MacroStruct,
        NULL,
        NULL,
        MacroStructToNumber,
        NULL);
    MacroStruct value = {.value = 42};

    return type.elementSize == sizeof(MacroStruct) &&
           type.compare == NULL &&
           type.print == NULL &&
           type.toNumber == MacroStructToNumber &&
           type.destroy == NULL &&
           type.toNumber(&value) == 42.0;
}

bool C23CustomOperationsDispatchByAddress(void)
{
    const ADT_ElementTypeInfo_t type = {
        .elementSize = sizeof(MacroStruct),
        .compare = NULL,
        .print = NULL,
        .destroy = NULL};
    DynamicArray_t array = {0};
    LinkedList_t list = {0};
    Stack_t stack = {0};
    Queue_t queue = {0};
    MacroStruct one = {.value = 1};
    MacroStruct two = {.value = 2};
    MacroStruct three = {.value = 3};
    MacroStruct four = {.value = 4};
    size_t arrayIndex = 0;
    size_t listIndex = 0;

    if (!da_Init(&array, type) ||
        !ll_Init(&list, type) ||
        !st_Init(&stack, type) ||
        !qu_Init(&queue, type))
    {
        da_Destroy(&array);
        ll_Destroy(&list);
        st_Destroy(&stack);
        qu_Destroy(&queue);
        return false;
    }

    const bool arrayWorks =
        da_Append(&array, &two) &&
        da_Prepend(&array, &one) &&
        da_Insert(&array, 1, &three) &&
        da_Set(&array, 2, &four) &&
        da_Contains(&array, &three) &&
        da_IndexOf(&array, &four, &arrayIndex) &&
        arrayIndex == 2;

    const bool listWorks =
        ll_Append(&list, &two) &&
        ll_Prepend(&list, &one) &&
        ll_Insert(&list, 1, &three) &&
        ll_Set(&list, 2, &four) &&
        ll_Contains(&list, &three) &&
        ll_IndexOf(&list, &four, &listIndex) &&
        listIndex == 2;

    MacroStruct stackValue = {0};
    const bool stackWorks =
        st_Push(&stack, &three) &&
        st_Pop(&stack, &stackValue) &&
        stackValue.value == three.value;

    MacroStruct queueValue = {0};
    const bool queueWorks =
        qu_Enqueue(&queue, &four) &&
        qu_Dequeue(&queue, &queueValue) &&
        queueValue.value == four.value;

    da_Destroy(&array);
    ll_Destroy(&list);
    st_Destroy(&stack);
    qu_Destroy(&queue);
    return arrayWorks && listWorks && stackWorks && queueWorks;
}

bool C23FunctionPointersDispatchByAddress(void)
{
    const ADT_ElementTypeInfo_t type = {
        .elementSize = sizeof(MacroFunctionFn_t),
        .compare = NULL,
        .print = NULL,
        .destroy = NULL};
    DynamicArray_t array = {0};
    LinkedList_t list = {0};
    Stack_t stack = {0};
    Queue_t queue = {0};
    MacroFunctionFn_t function = Increment;
    MacroFunctionFn_t arrayFunction = NULL;
    MacroFunctionFn_t listFunction = NULL;
    MacroFunctionFn_t stackFunction = NULL;
    MacroFunctionFn_t queueFunction = NULL;

    if (!da_Init(&array, type) ||
        !ll_Init(&list, type) ||
        !st_Init(&stack, type) ||
        !qu_Init(&queue, type))
    {
        da_Destroy(&array);
        ll_Destroy(&list);
        st_Destroy(&stack);
        qu_Destroy(&queue);
        return false;
    }

    const bool works =
        da_Append(&array, &function) &&
        ll_Append(&list, &function) &&
        st_Push(&stack, &function) &&
        qu_Enqueue(&queue, &function) &&
        da_Get(&array, 0, &arrayFunction) &&
        ll_Get(&list, 0, &listFunction) &&
        st_Pop(&stack, &stackFunction) &&
        qu_Dequeue(&queue, &queueFunction) &&
        arrayFunction(41) == 42 &&
        listFunction(41) == 42 &&
        stackFunction(41) == 42 &&
        queueFunction(41) == 42;

    da_Destroy(&array);
    ll_Destroy(&list);
    st_Destroy(&stack);
    qu_Destroy(&queue);
    return works;
}
