#include <stdio.h>
#include <stdlib.h>

#include "libadt/libadt.h"

typedef struct
{
    int id;
    char name[32];
} Student_t;

static int CompareStudent(const void *first, const void *second)
{
    const Student_t *a = first;
    const Student_t *b = second;
    return (a->id > b->id) - (a->id < b->id);
}

static void PrintStudent(const void *element)
{
    const Student_t *student = element;
    printf("{id: %d, name: \"%s\"}", student->id, student->name);
}

static double StudentIdToNumber(const void *element)
{
    const Student_t *student = element;
    return (double)student->id;
}

int main(void)
{
    Student_t initialStudents[] = {
        {.id = 1001, .name = "Ada"},
        {.id = 1002, .name = "Grace"},
        {.id = 1003, .name = "Dennis"}};

    const ADT_ElementTypeInfo_t studentType = ADT_ELEMENT_TYPE_INFO(
        Student_t,
        CompareStudent,
        PrintStudent,
        StudentIdToNumber,
        NULL);

    DynamicArray_t students = {0};

    if (!da_InitFrom(
            &students,
            initialStudents,
            ARRAY_COUNT(initialStudents),
            studentType))
    {
        return EXIT_FAILURE;
    }

    adt_Print(&students);

    Student_t target = {.id = 1002};
    size_t index = 0;

    if (da_IndexOf(&students, &target, &index))
    {
        printf("Student %d is at index %zu.\n", target.id, index);
    }

    da_Destroy(&students);
    return EXIT_SUCCESS;
}
