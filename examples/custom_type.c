#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libadt/dynamic_array.h"

typedef struct
{
    int id;
    char *name;
} Student;

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
    const Student *a = first;
    const Student *b = second;
    return (a->id > b->id) - (a->id < b->id);
}

static void PrintStudent(const void *element)
{
    const Student *student = element;
    printf("{id: %d, name: \"%s\"}", student->id, student->name);
}

static void DestroyStudent(void *element)
{
    Student *student = element;
    free(student->name);
    student->name = NULL;
}

static bool AppendStudent(DynamicArray_t *students, int id, const char *name)
{
    Student student = {
        .id = id,
        .name = CopyString(name)};

    if (student.name == NULL)
    {
        return false;
    }

    if (!da_Append(students, &student))
    {
        free(student.name);
        return false;
    }

    /* The copied pointer is now owned by the element stored in the array. */
    return true;
}

int main(void)
{
    const ADT_TypeInfo_t studentType = {
        .elementSize = sizeof(Student),
        .compare = CompareStudent,
        .print = PrintStudent,
        .destroy = DestroyStudent};

    DynamicArray_t students = {0};

    if (!da_Init(&students, studentType))
    {
        fputs("Failed to initialize the student array.\n", stderr);
        return EXIT_FAILURE;
    }

    if (!AppendStudent(&students, 1001, "Ada") ||
        !AppendStudent(&students, 1002, "Grace") ||
        !AppendStudent(&students, 1003, "Dennis"))
    {
        da_Destroy(&students);
        return EXIT_FAILURE;
    }

    puts("Students:");
    da_Print(&students);

    Student target = {.id = 1002, .name = NULL};
    size_t index = 0;

    if (da_IndexOf(&students, &target, &index))
    {
        printf("Student 1002 is at index %zu.\n", index);
    }

    DA_DEBUG(students);

    /* Calls DestroyStudent for every remaining element. */
    da_Destroy(&students);
    return EXIT_SUCCESS;
}
