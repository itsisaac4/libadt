# Custom element types

libadt can store any complete C object type. A custom
`ADT_ElementTypeInfo_t` acts as its runtime descriptor by providing the size
and callback contracts needed after type erasure.

For most custom types, setup is four steps:

1. Define the C type.
2. Implement only the callbacks its operations need.
3. Collect them in an `ADT_ElementTypeInfo_t`.
4. Initialize a container with that descriptor.

## Define the element type

This project uses the `_t` suffix for type names:

```c
typedef struct
{
    int id;
    char *name;
    double score;
} Student_t;
```

Containers copy `sizeof(Student_t)` bytes into their own storage. Pointer
members are copied shallowly; the pointed-to allocation is not cloned.

## Callback signatures

Custom callbacks must exactly match the library function-pointer types:

```c
int CompareStudent(const void *first, const void *second);
void PrintStudent(const void *element);
double StudentScoreToNumber(const void *element);
void DestroyStudent(void *element);
```

Each callback receives the type-erased address of a stored `Student_t`, so it
casts the generic pointer before accessing fields.

### Comparison

```c
int CompareStudent(const void *first, const void *second)
{
    const Student_t *left = first;
    const Student_t *right = second;
    return (left->id > right->id) - (left->id < right->id);
}
```

The result must be negative, zero, or positive when `left` belongs before,
equals, or belongs after `right`. Searching, extrema, and default sorting use
this callback.

### Printing

```c
void PrintStudent(const void *element)
{
    const Student_t *student = element;
    printf("{id: %d, name: \"%s\", score: %.1f}",
           student->id,
           student->name,
           student->score);
}
```

Print only the element. `adt_Print` supplies container formatting, separators,
and the trailing newline.

### Numeric projection

```c
double StudentScoreToNumber(const void *element)
{
    const Student_t *student = element;
    return student->score;
}
```

The conversion returns the `double` used by the statistics functions. It can
return whichever field the program wants to analyze.

### Resource destruction

```c
void DestroyStudent(void *element)
{
    Student_t *student = element;
    free(student->name);
    student->name = NULL;
}
```

Release only resources owned by the element. Do not free `element` itself;
that storage belongs to the container.

## Create the element descriptor

`ADT_ELEMENT_TYPE_INFO` takes the stored type and all four optional callbacks:

```c
const ADT_ElementTypeInfo_t studentType = ADT_ELEMENT_TYPE_INFO(
    Student_t,
    CompareStudent,
    PrintStudent,
    StudentScoreToNumber,
    DestroyStudent);
```

Only `elementSize` is always required. Do not invent meaningless callbacks
just to fill the descriptor; pass `NULL` for behavior that does not apply:

```c
const ADT_ElementTypeInfo_t pointType = ADT_ELEMENT_TYPE_INFO(
    Point_t,
    ComparePoint,
    PrintPoint,
    NULL,
    NULL);
```

Each callback enables a group of operations:

| Descriptor field | Enables |
| --- | --- |
| `elementSize` | Allocation and shallow element copies |
| `compare` | Ordering, equality, extrema, and sorting |
| `print` | Shared container printing |
| `toNumber` | Numeric statistics |
| `destroy` | Cleanup of resources owned by elements |

An operation returns `false` when its required callback is unavailable. That
keeps an unsupported concept—such as the mean of an arbitrary record—from
silently producing a made-up result.

## Initialize and use a container

Custom elements use the normal initialization functions:

```c
DynamicArray_t students = {0};

if (!da_Init(&students, studentType))
{
    return false;
}
```

Pass custom values by address:

```c
Student_t student = MakeStudent(1001, "Ada", 94.5);

if (!da_Append(&students, &student))
{
    DestroyStudent(&student);
}
```

After a successful insertion, the stored shallow copy assumes responsibility
for resources described by `destroy`. This is the point where reading
[ownership](ownership.md) saves real debugging time, especially when the
structure contains owning pointers.

## Share a custom type across source files

Place the element definition, callback declarations, and descriptor declaration
in an application header:

```c
#ifndef STUDENT_H
#define STUDENT_H

#include "libadt/libadt.h"

typedef struct
{
    int id;
    char *name;
    double score;
} Student_t;

int CompareStudent(const void *first, const void *second);
void PrintStudent(const void *element);
double StudentScoreToNumber(const void *element);
void DestroyStudent(void *element);

extern const ADT_ElementTypeInfo_t STUDENT_ELEMENT_TYPE;

#endif
```

Define the callbacks and descriptor once in the matching source file:

```c
const ADT_ElementTypeInfo_t STUDENT_ELEMENT_TYPE =
    ADT_ELEMENT_TYPE_INFO(
        Student_t,
        CompareStudent,
        PrintStudent,
        StudentScoreToNumber,
        DestroyStudent);
```

Use `static` callbacks instead when the custom type is needed in only one
source file. This keeps those names private to that translation unit.

## Why custom types need a descriptor

C can calculate `sizeof(Student_t)`, but it cannot guess whether students
should compare by ID or name, how they should print, which field is numeric, or
whether they own pointer fields.

`ADT_ElementTypeInfo_t` connects those choices to the stored type. In a
class-based language, similar behavior might live in methods or interfaces.
In C, the descriptor makes that contract explicit instead of pretending the
compiler can infer it.
