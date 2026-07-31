248 checks pass, and every example compiles with strict warnings.

# Custom element types

libadt can store any complete C object type. A custom type must provide its
size, while callbacks opt it into shared behaviors such as comparison,
printing, numeric statistics, and resource destruction.

## Define the element type

Give the stored structure a typedef ending in `_t`:

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

Each callback receives the address of a stored `Student_t`, so it casts the
generic pointer before accessing fields.

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

The projection gives numeric statistics one common `double` representation.
It can select whichever field is meaningful for the application.

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

`ADT_ELEMENT_TYPE_INFO` requires an explicit decision for every capability:

```c
const ADT_ElementTypeInfo_t studentType = ADT_ELEMENT_TYPE_INFO(
    Student_t,
    CompareStudent,
    PrintStudent,
    StudentScoreToNumber,
    DestroyStudent);
```

Only `elementSize` is universally required. Pass `NULL` for behavior that does
not apply:

```c
const ADT_ElementTypeInfo_t pointType = ADT_ELEMENT_TYPE_INFO(
    Point_t,
    ComparePoint,
    PrintPoint,
    NULL,
    NULL);
```

Callbacks act as optional interfaces:

| Descriptor field | Enables |
| --- | --- |
| `elementSize` | Allocation and shallow element copies |
| `compare` | Ordering, equality, extrema, and sorting |
| `print` | Shared container printing |
| `toNumber` | Numeric statistics |
| `destroy` | Cleanup of resources owned by elements |

An operation returns `false` when its required callback is unavailable.

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
for resources described by `destroy`. See [ownership](ownership.md) before
storing structures that contain owning pointers.

## Share a custom type across source files

Place the element definition, callback declarations, and descriptor declaration
in an application header:

```c
#ifndef STUDENT_H
#define STUDENT_H

#include "libadt/abstract_data_type.h"

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

## Language constraint

C can infer `sizeof(Student_t)`, but it cannot infer whether students compare
by ID or name, print in a particular format, project to score or age, or own
their pointer fields. An OOP language also requires those methods to be
implemented, but usually constructs the associated vtable automatically.

`ADT_ElementTypeInfo_t` makes that relationship explicit. It serves as the
element type's manually constructed interface table while allowing different
descriptors or per-operation overrides for different behaviors.
