# Dynamic arrays

`DynamicArray_t` stores elements contiguously and grows its allocation when
more capacity is needed.

## Quick start

```c
#include <stdlib.h>

#include "libadt/dynamic_array.h"

int main(void)
{
    int values[] = {20, 30, 40};
    DynamicArray_t numbers = {0};

    if (!DA_INIT_FROM(&numbers, values) ||
        !da_Prepend(&numbers, 10) ||
        !da_Append(&numbers, 50))
    {
        da_Destroy(&numbers);
        return EXIT_FAILURE;
    }

    adt_Print(&numbers);
    da_Destroy(&numbers);
    return EXIT_SUCCESS;
}
```

Use `DA_INIT(&array, type)` for an empty primitive array and
`DA_INIT_FROM(&array, values)` for a fixed-size C array.

## Operations

| Category | Functions |
| --- | --- |
| Initialization | `da_Init`, `da_InitFrom`, `DA_INIT`, `DA_INIT_FROM` |
| Access | `da_Get`, `da_Set` |
| Search | `da_IndexOf`, `da_Contains` |
| Insertion | `da_Insert`, `da_Prepend`, `da_Append` |
| Removal | `da_Remove`, `da_Take`, `da_Clear`, `da_Destroy` |
| Shared | `adt_Size`, `adt_IsEmpty`, `adt_Print`, statistics, `adt_Sort` |

Each operation supports any type matching the array's initialized element
type. Pass supported primitives directly and all other types by address.

## Access and removal

`da_Get` copies an element into caller-provided storage:

```c
int value = 0;

if (da_Get(&numbers, 2, &value))
{
    UseValue(value);
}
```

This does not remove the element or transfer resources it owns.

Use `da_Remove` when the removed element should be destroyed. Use `da_Take`
when its value and owned resources should transfer to the caller:

```c
Student_t student = {0};

if (da_Take(&students, index, &student))
{
    UseStudent(&student);
    DestroyStudent(&student);
}
```

The output storage passed to `da_Take` cannot point inside the dynamic array,
because moving the remaining elements would invalidate or overwrite it.

## Custom element types

```c
const ADT_ElementTypeInfo_t studentType = ADT_ELEMENT_TYPE_INFO(
    Student_t,
    CompareStudent,
    PrintStudent,
    NULL,
    DestroyStudent);

DynamicArray_t students = {0};

if (da_Init(&students, studentType))
{
    Student_t student = MakeStudent(1001, "Ada");

    if (!da_Append(&students, &student))
    {
        DestroyStudent(&student);
    }
}
```

The append copies the structure, not resources referenced by its fields. See
[custom element types](custom_types.md) and [ownership](ownership.md) before
using a non-`NULL` destroy callback.

## Complexity

| Operation | Complexity |
| --- | --- |
| Get or set by index | O(1) |
| Append | Amortized O(1) |
| Prepend or indexed insertion | O(n) |
| Search | O(n) |
| Remove | O(n) |
| Clear or destroy | O(n) when elements require destruction |

Capacity grows geometrically, so repeated append operations do not reallocate
for every element. `Clear` retains the current allocation for reuse;
`Destroy` releases it.

## Lifecycle

Zero-initialize before the first initialization:

```c
DynamicArray_t array = {0};
```

Do not initialize a live array again. Call `da_Destroy` first. After
`da_Destroy`, the array is reset and may be initialized again.
