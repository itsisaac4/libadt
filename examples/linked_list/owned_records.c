#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libadt/linked_list.h"

typedef struct
{
    int id;
    char *name;
} Student_t;

static char *CopyString(const char *source)
{
    size_t size = strlen(source) + 1;
    char *copy = malloc(size);

    if (copy != NULL)
    {
        memcpy(copy, source, size);
    }

    return copy;
}

static int CompareStudent(const void *first, const void *second)
{
    const Student_t *left = first;
    const Student_t *right = second;
    return (left->id > right->id) - (left->id < right->id);
}

static void PrintStudent(const void *element)
{
    const Student_t *student = element;
    printf("{id: %d, name: \"%s\"}", student->id, student->name);
}

static void DestroyStudent(void *element)
{
    Student_t *student = element;
    free(student->name);
    student->name = NULL;
}

static bool AppendStudent(LinkedList_t *students, int id, const char *name)
{
    Student_t student = {
        .id = id,
        .name = CopyString(name)};

    if (student.name == NULL)
    {
        return false;
    }

    if (!ll_AppendRef(students, &student))
    {
        free(student.name);
        return false;
    }

    return true;
}

int main(void)
{
    const ADT_TypeInfo_t studentType = {
        .elementSize = sizeof(Student_t),
        .compare = CompareStudent,
        .print = PrintStudent,
        .destroy = DestroyStudent};
    LinkedList_t students = {0};

    if (!ll_Init(&students, studentType) ||
        !AppendStudent(&students, 1003, "Dennis") ||
        !AppendStudent(&students, 1001, "Ada") ||
        !AppendStudent(&students, 1002, "Grace") ||
        !adt_Sort(&students, ADT_SORT_QUICK))
    {
        ll_Destroy(&students);
        return EXIT_FAILURE;
    }

    adt_Print(&students);

    Student_t transferred = {0};
    if (!ll_Take(&students, 0, &transferred))
    {
        ll_Destroy(&students);
        return EXIT_FAILURE;
    }

    printf("transferred: ");
    PrintStudent(&transferred);
    printf("\n");

    DestroyStudent(&transferred);
    ll_Destroy(&students);
    return EXIT_SUCCESS;
}
