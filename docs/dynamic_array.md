# Dynamic Arrays

`DynamicArray_t` stores elements in one allocation and grows that allocation when it runs out of capacity.

Choose it when indexed access and cache-friendly traversal matter more than the cost of shifting elements during middle insertions.

## Quick Start

```c
#include <stdlib.h>

#include "libadt/libadt.h"

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

Use `DA_INIT(&array, type)` for an empty primitive array and `DA_INIT_FROM(&array, values)` for a fixed-size C array.

## Operations

| Category | Functions |
| --- | --- |
| Initialization | `da_Init`, `da_InitFrom`, `DA_INIT`, `DA_INIT_FROM` |
| Access | `da_Get`, `da_Set` |
| Search | `da_IndexOf`, `da_Contains` |
| Insertion | `da_Insert`, `da_Prepend`, `da_Append` |
| Removal | `da_Remove`, `da_Take`, `da_Clear`, `da_Destroy` |
| Shared | `adt_Size`, `adt_IsEmpty`, `adt_Print`, statistics, `adt_Sort` |

Each operation supports any type matching the array's initialized element type. Pass supported primitives directly and all other types by address.

## Search and Replacement

`da_IndexOf` writes the first matching index to its output parameter, while `da_Contains` reports only whether a match exists. Both use the configured comparator when one is available and otherwise compare the stored bytes. A comparator is the safer definition of equality for custom structures with padding or pointer members.

`da_Set` releases resources owned by the element being replaced and then stores a shallow copy of the replacement. After a successful call, the stored copy assumes responsibility for resources described by the descriptor's `destroy` callback.

## Access and Removal

`da_Get` copies an element into caller-provided storage:

```c
int value = 0;

if (da_Get(&numbers, 2, &value))
{
    UseValue(value);
}
```

This is a non-owning shallow copy. It does not remove the element or transfer its resources.

Use `da_Remove` when the removed element should be destroyed. Use `da_Take` when its value and owned resources should transfer to the caller:

```c
Student_t student = {0};

if (da_Take(&students, index, &student))
{
    UseStudent(&student);
    DestroyStudent(&student);
}
```

The output storage passed to `da_Take` cannot point inside the dynamic array, because moving the remaining elements would invalidate or overwrite it.

## Custom Element Types

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

da_Destroy(&students);
```

The append copies the structure, not resources referenced by its fields. See [custom element types](custom_types.md) and [ownership](ownership.md) before using a non-`NULL` destroy callback.

## Complexity

| Operation | Complexity |
| --- | --- |
| Get or set by index | O(1) |
| Append | Amortized O(1) |
| Prepend or indexed insertion | O(n) |
| Search | O(n) |
| Remove | O(n) |
| Clear or destroy | O(n) |

In practice:

- Capacity doubles as needed, so every append does not cause a reallocation.
- `Clear` keeps the allocation for reuse.
- `Destroy` releases the allocation and resets the array.

## Lifecycle

Zero-initialize before the first initialization:

```c
DynamicArray_t array = {0};
```

Do not initialize a live array again. Call `da_Destroy` first. After `da_Destroy`, the array is reset and may be initialized again.
