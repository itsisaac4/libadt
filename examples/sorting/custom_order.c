#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libadt/libadt.h"

typedef struct
{
    int id;
    char name[32];
    double score;
} Student_t;

static int CompareStudentId(const void *first, const void *second)
{
    const Student_t *left = first;
    const Student_t *right = second;
    return (left->id > right->id) - (left->id < right->id);
}

static int CompareStudentName(const void *first, const void *second)
{
    const Student_t *left = first;
    const Student_t *right = second;
    return strcmp(left->name, right->name);
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

int main(void)
{
    Student_t values[] = {
        {.id = 1003, .name = "Dennis", .score = 88.0},
        {.id = 1001, .name = "Ada", .score = 97.5},
        {.id = 1002, .name = "Grace", .score = 94.0}};
    const ADT_ElementTypeInfo_t studentType = ADT_ELEMENT_TYPE_INFO(
        Student_t,
        CompareStudentId,
        PrintStudent,
        StudentScoreToNumber,
        NULL);
    DynamicArray_t students = {0};

    if (!da_InitFrom(&students, values, ARRAY_COUNT(values), studentType))
    {
        return EXIT_FAILURE;
    }

    if (!adt_Sort(&students, ADT_SORT_QUICK))
    {
        da_Destroy(&students);
        return EXIT_FAILURE;
    }

    printf("by id: ");
    if (!adt_Print(&students))
    {
        da_Destroy(&students);
        return EXIT_FAILURE;
    }

    if (!adt_SortBy(&students, ADT_SORT_INSERTION, CompareStudentName))
    {
        da_Destroy(&students);
        return EXIT_FAILURE;
    }

    printf("by name: ");
    if (!adt_Print(&students))
    {
        da_Destroy(&students);
        return EXIT_FAILURE;
    }

    if (!adt_SortBy(&students, ADT_SORT_SELECTION, CompareStudentScoreDescending))
    {
        da_Destroy(&students);
        return EXIT_FAILURE;
    }

    printf("by score: ");
    if (!adt_Print(&students))
    {
        da_Destroy(&students);
        return EXIT_FAILURE;
    }

    da_Destroy(&students);
    return EXIT_SUCCESS;
}
