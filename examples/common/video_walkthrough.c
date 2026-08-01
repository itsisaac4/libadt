/*
 * A single, presentation-friendly tour of libadt.
 */

#include <stdio.h>
#include <stdlib.h>

#include "libadt/libadt.h"

typedef struct
{
    int id;
    char name[32];
    double score;
} Student_t;

/* These callbacks restore type-specific behavior after type erasure. */
static int CompareStudentId(const void *first, const void *second)
{
    const Student_t *left = first;
    const Student_t *right = second;
    return (left->id > right->id) - (left->id < right->id);
}

static int CompareStudentScoreDescending(const void *first, const void *second)
{
    const Student_t *left = first;
    const Student_t *right = second;
    return (right->score > left->score) - (right->score < left->score);
}

static void PrintStudent(const void *element)
{
    const Student_t *student = element;
    printf("{%d, %s, %.1f}", student->id, student->name, student->score);
}

static double StudentScoreToNumber(const void *element)
{
    const Student_t *student = element;
    return student->score;
}

/*
 * The ADT_t parameter is an upcast. The same code works for contiguous and
 * linked storage because adt_* calls dispatch through the container's vtable.
 */
static bool SortAndPrint(const char *label, ADT_t *container)
{
    double mean = 0.0;
    double median = 0.0;
    double mode = 0.0;

    if (!adt_Mean(container, &mean) ||
        !adt_Median(container, &median) ||
        !adt_Mode(container, &mode) ||
        !adt_Sort(container, ADT_SORT_QUICK))
    {
        return false;
    }

    printf("%s: ", label);
    if (!adt_Print(container))
    {
        return false;
    }

    printf(
        "               statistics: mean = %.2f, median = %.2f, mode = %.2f\n",
        mean,
        median,
        mode);
    return true;
}

int main(void)
{
    int grades[] = {88, 95, 72, 95, 84};
    DynamicArray_t gradeArray = {0};
    LinkedList_t gradeList = {0};
    Stack_t gradeStack = {0};
    Queue_t gradeQueue = {0};
    DynamicArray_t students = {0};
    int result = EXIT_FAILURE;

    puts("\n\n\n=== 1. One Polymorphic API, Four ADTs ===\n");

    /* The macros infer primitive metadata at compile time with C23 _Generic. */
    if (!DA_INIT_FROM(&gradeArray, grades) ||
        !LL_INIT_FROM(&gradeList, grades) ||
        !ST_INIT_FROM(&gradeStack, grades) ||
        !QU_INIT_FROM(&gradeQueue, grades))
    {
        goto cleanup;
    }

    printf("original:      ");
    if (!adt_Print(&gradeArray))
    {
        goto cleanup;
    }
    puts("\n");

    ADT_t *gradeContainers[] = {
        &gradeArray,
        &gradeList,
        &gradeStack,
        &gradeQueue};
    const char *gradeLabels[] = {
        "dynamic array",
        "linked list  ",
        "stack        ",
        "queue        "};

    for (size_t i = 0; i < ARRAY_COUNT(gradeContainers); i++)
    {
        if (!SortAndPrint(gradeLabels[i], gradeContainers[i]))
        {
            goto cleanup;
        }
    }

    puts("\n=== 2. Type Erasure + Runtime Metadata ===\n");

    Student_t initialStudents[] = {
        {.id = 1003, .name = "Dennis", .score = 88.0},
        {.id = 1001, .name = "Ada", .score = 97.5},
        {.id = 1002, .name = "Grace", .score = 94.0}};
    Student_t newStudent = {.id = 1004, .name = "Barbara", .score = 91.5};

    const ADT_ElementTypeInfo_t studentType = ADT_ELEMENT_TYPE_INFO(
        Student_t,
        CompareStudentId,
        PrintStudent,
        StudentScoreToNumber,
        NULL);

    if (!da_InitFrom(&students, initialStudents, ARRAY_COUNT(initialStudents), studentType) ||
        !da_Append(&students, &newStudent))
    {
        goto cleanup;
    }

    printf("custom records: ");
    if (!adt_Print(&students))
    {
        goto cleanup;
    }

    // CALCULATE MEAN BY SCORE AND SORT BY SCORE DESCENDING CALLBACK WITH INSERTION SORT
    double classMean = 0.0;
    if (!adt_Mean(&students, &classMean) ||
        !adt_SortBy(&students, ADT_SORT_INSERTION, CompareStudentScoreDescending))
    {
        goto cleanup;
    }
    printf("mean score from callback: %.2f\n", classMean);
    printf("sorted by score callback: ");
    if (!adt_Print(&students))
    {
        goto cleanup;
    }

    // SORT BY DEFAULT ID CALLBACK WITH BUBBLE SORT
    if (!adt_Sort(&students, ADT_SORT_BUBBLE))
    {
        goto cleanup;
    }
    printf("sorted by default ID callback: ");
    if (!adt_Print(&students))
    {
        goto cleanup;
    }

    result = EXIT_SUCCESS;

    puts("\n");

cleanup:
    /* C makes ownership explicit: every initialized container is destroyed. */
    da_Destroy(&students);
    qu_Destroy(&gradeQueue);
    st_Destroy(&gradeStack);
    ll_Destroy(&gradeList);
    da_Destroy(&gradeArray);
    return result;
}
