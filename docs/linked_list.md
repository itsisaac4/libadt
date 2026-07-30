# Linked lists

`LinkedList_t` stores elements in a doubly linked sequence. Each node owns a
separate element allocation, and the list tracks both its head and tail.

## Quick start

```c
#include <stdlib.h>

#include "libadt/linked_list.h"

int main(void)
{
    int values[] = {20, 30, 40};
    LinkedList_t numbers = {0};

    if (!LL_INIT_FROM(&numbers, values) ||
        !ll_PrependValue(&numbers, 10) ||
        !ll_AppendValue(&numbers, 50))
    {
        ll_Destroy(&numbers);
        return EXIT_FAILURE;
    }

    adt_Print(&numbers);
    ll_Destroy(&numbers);
    return EXIT_SUCCESS;
}
```

Use `LL_INIT(&list, type)` for an empty primitive list and
`LL_INIT_FROM(&list, values)` for a fixed-size C array.

## Operations

| Category | Functions |
| --- | --- |
| Initialization | `ll_Init`, `ll_InitFrom`, `LL_INIT`, `LL_INIT_FROM` |
| Access | `ll_Get`, `ll_SetRef`, `ll_SetValue` |
| Search | `ll_IndexOfRef`, `ll_IndexOfValue`, `ll_ContainsRef`, `ll_ContainsValue` |
| Insertion | `ll_InsertRef`, `ll_InsertValue`, `ll_PrependRef`, `ll_PrependValue`, `ll_AppendRef`, `ll_AppendValue` |
| Removal | `ll_Remove`, `ll_Take`, `ll_Clear`, `ll_Destroy` |
| Shared | `adt_Size`, `adt_IsEmpty`, `adt_Print`, `adt_Min`, `adt_Max`, `adt_Sort` |

Functions ending in `Value` accept supported primitive values. Functions
ending in `Ref` accept the address of custom, pointer, or primitive element
storage.

## Access and removal

`ll_Get` copies an element into caller-provided storage without removing it:

```c
int value = 0;
ll_Get(&numbers, 2, &value);
```

Use `ll_Remove` to destroy a removed element. Use `ll_Take` to transfer its
value and owned resources:

```c
Student_t student = {0};

if (ll_Take(&students, index, &student))
{
    UseStudent(&student);
    DestroyStudent(&student);
}
```

The output storage passed to `ll_Take` cannot point into any element currently
stored by the list.

## Custom element types

```c
const ADT_TypeInfo_t studentType = {
    .elementSize = sizeof(Student_t),
    .compare = CompareStudent,
    .print = PrintStudent,
    .destroy = DestroyStudent
};

LinkedList_t students = {0};

if (ll_Init(&students, studentType))
{
    Student_t student = MakeStudent(1001, "Ada");

    if (!ll_AppendRef(&students, &student))
    {
        DestroyStudent(&student);
    }
}
```

After a successful append, the stored shallow copy is responsible for resources
described by `DestroyStudent`. See [ownership](ownership.md) for the complete
contract.

## Complexity

| Operation | Complexity |
| --- | --- |
| Prepend or append | O(1) |
| Access or set by index | O(min(i, n - i)) |
| Indexed insertion | O(min(i, n - i)) |
| Search | O(n) |
| Remove or take by index | O(min(i, n - i)) |
| Clear or destroy | O(n) |

Indexed lookup starts from the closer end of the list. Unlike the dynamic
array, linked-list insertion does not shift existing elements, but each new
element requires node and element allocations.

`Clear` releases every node while retaining runtime type information. The list
can immediately be reused. `Destroy` also resets the shared type information.

## Lifecycle

Zero-initialize before the first initialization:

```c
LinkedList_t list = {0};
```

Do not initialize a live list again. Call `ll_Destroy` first. After
`ll_Destroy`, the list is reset and may be initialized again.
