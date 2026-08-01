# Linked Lists

`LinkedList_t` is a doubly linked list with stored head and tail pointers. Each node has its own element allocation.

Choose it when insertion at either end matters more than indexed access or contiguous memory locality.

## Quick Start

```c
#include <stdlib.h>

#include "libadt/libadt.h"

int main(void)
{
    int values[] = {20, 30, 40};
    LinkedList_t numbers = {0};

    if (!LL_INIT_FROM(&numbers, values) ||
        !ll_Prepend(&numbers, 10) ||
        !ll_Append(&numbers, 50))
    {
        ll_Destroy(&numbers);
        return EXIT_FAILURE;
    }

    adt_Print(&numbers);
    ll_Destroy(&numbers);
    return EXIT_SUCCESS;
}
```

Use `LL_INIT(&list, type)` for an empty primitive list and `LL_INIT_FROM(&list, values)` for a fixed-size C array.

## Operations

| Category | Functions |
| --- | --- |
| Initialization | `ll_Init`, `ll_InitFrom`, `LL_INIT`, `LL_INIT_FROM` |
| Access | `ll_Get`, `ll_Set` |
| Search | `ll_IndexOf`, `ll_Contains` |
| Insertion | `ll_Insert`, `ll_Prepend`, `ll_Append` |
| Removal | `ll_Remove`, `ll_Take`, `ll_Clear`, `ll_Destroy` |
| Shared | `adt_Size`, `adt_IsEmpty`, `adt_Print`, statistics, `adt_Sort` |

Each operation supports any type matching the list's initialized element type. Pass supported primitives directly and all other types by address.

## Search and Replacement

`ll_IndexOf` writes the first matching index to its output parameter, while `ll_Contains` reports only whether a match exists. Both use the configured comparator when one is available and otherwise compare the stored bytes. A comparator is the safer definition of equality for custom structures with padding or pointer members.

`ll_Set` releases resources owned by the element being replaced and then stores a shallow copy of the replacement. After a successful call, the stored copy assumes responsibility for resources described by the descriptor's `destroy` callback.

## Access and Removal

`ll_Get` copies an element into caller-provided storage without removing it:

```c
int value = 0;
ll_Get(&numbers, 2, &value);
```

Use `ll_Remove` to destroy a removed element. Use `ll_Take` to transfer its value and owned resources:

```c
Student_t student = {0};

if (ll_Take(&students, index, &student))
{
    UseStudent(&student);
    DestroyStudent(&student);
}
```

The output storage passed to `ll_Take` cannot point into any element currently stored by the list.

## Custom Element Types

```c
const ADT_ElementTypeInfo_t studentType = ADT_ELEMENT_TYPE_INFO(
    Student_t,
    CompareStudent,
    PrintStudent,
    NULL,
    DestroyStudent);

LinkedList_t students = {0};

if (ll_Init(&students, studentType))
{
    Student_t student = MakeStudent(1001, "Ada");

    if (!ll_Append(&students, &student))
    {
        DestroyStudent(&student);
    }
}

ll_Destroy(&students);
```

After a successful append, the stored shallow copy is responsible for resources described by `DestroyStudent`. See [custom element types](custom_types.md) and [ownership](ownership.md) for the ownership rules.

## Complexity

| Operation | Complexity |
| --- | --- |
| Prepend or append | O(1) |
| Access or set by index | O(min(i, n - i)) |
| Indexed insertion | O(min(i, n - i)) |
| Search | O(n) |
| Remove or take by index | O(min(i, n - i)) |
| Clear or destroy | O(n) |

Two representation details explain most of the complexity:

- Indexed lookup starts from whichever end is closer.
- Insertion does not shift existing elements, but each element needs a node allocation and an element allocation.

`Clear` releases every node while retaining runtime type information. The list can immediately be reused. `Destroy` also resets the shared type information.

## Lifecycle

Zero-initialize before the first initialization:

```c
LinkedList_t list = {0};
```

Do not initialize a live list again. Call `ll_Destroy` first. After `ll_Destroy`, the list is reset and may be initialized again.
