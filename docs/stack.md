# Stacks

`Stack_t` stores elements in last-in, first-out order. It uses
`ContiguousStorage_t`, so the top of the stack is the end of a resizable
allocation.

The public API stays intentionally small: push, inspect the top, and either
pop or discard it.

## Quick start

```c
#include "libadt/libadt.h"

Stack_t stack = {0};

if (ST_INIT(&stack, int))
{
    st_Push(&stack, 10);
    st_Push(&stack, 20);

    int value = 0;
    st_Pop(&stack, &value);
    st_Destroy(&stack);
}
```

## Operations

| Category | Functions |
| --- | --- |
| Initialization | `st_Init`, `st_InitFrom`, `ST_INIT`, `ST_INIT_FROM` |
| Access | `st_Peek` |
| Insertion | `st_Push` |
| Removal | `st_Pop`, `st_Discard`, `st_Clear`, `st_Destroy` |
| Shared | `adt_Size`, `adt_IsEmpty`, `adt_Print`, statistics, `adt_Sort` |

`ST_INIT_FROM` reads its input from bottom to top, so the last array element
becomes the top. Shared traversal also runs from bottom to top.

## Ownership

`st_Push` stores a shallow copy. `st_Pop` transfers the removed element and its
owned resources to the caller. `st_Discard`, `st_Clear`, and `st_Destroy`
invoke the configured destroy callback.

The output passed to `st_Pop` cannot point inside the stack's contiguous
storage because removal could invalidate that address.

## Complexity

| Operation | Complexity |
| --- | --- |
| Push | Amortized O(1) |
| Peek | O(1) |
| Pop or discard | O(1) |
| Clear or destroy | O(n) when elements require destruction |

`st_Clear` retains the allocation for reuse. `st_Destroy` releases it and
resets the shared type information.
