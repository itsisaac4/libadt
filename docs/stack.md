# Stacks

`Stack_t` stores elements in last-in, first-out order. It uses `ContiguousStorage_t`, so the top of the stack is the end of a resizable allocation.

The public API stays focused on stack operations, with binary search available for stacks whose bottom-to-top traversal is already sorted.

## Quick Start

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
| Search | `st_BinarySearch`, `st_BinarySearchBy` |
| Insertion | `st_Push` |
| Removal | `st_Pop`, `st_Discard`, `st_Clear`, `st_Destroy` |
| Shared | `adt_Size`, `adt_IsEmpty`, `adt_Print`, statistics, `adt_Sort` |

`ST_INIT_FROM` reads its input from bottom to top, so the last array element becomes the top. Shared traversal also runs from bottom to top.

Stacks provide binary search because they use `ContiguousStorage_t`. Linked-storage ADTs do not provide it.

`st_BinarySearch` uses the configured comparator. `st_BinarySearchBy` uses an override.

The stack must use that ordering from bottom to top. Duplicate searches return the first bottom-based match. Failure leaves `outIndex` unchanged.

Use `adt_isSorted` before `st_BinarySearch` when the ordering is uncertain. Both use the comparator supplied at initialization.

For overrides, pass the same comparator to `adt_isSortedBy` and `st_BinarySearchBy`. Skip the check when the ordering is known.

Use `ST_INIT` and `ST_INIT_FROM` for inferred primitive behavior. Use `st_Init` or `st_InitFrom` with an `ADT_ElementTypeInfo_t` when the element is a custom type.

## Ownership

`st_Push` stores a shallow copy. `st_Pop` transfers the removed element and its owned resources to the caller. `st_Discard`, `st_Clear`, and `st_Destroy` invoke the configured destroy callback.

The output passed to `st_Pop` cannot point inside the stack's contiguous storage because removal could invalidate that address.

## Complexity

| Operation | Complexity |
| --- | --- |
| Push | Amortized O(1) |
| Peek | O(1) |
| Pop or discard | O(1) |
| Binary search | O(log n) |
| Clear or destroy | O(n) |

`st_Clear` retains the allocation for reuse. `st_Destroy` releases it and resets the shared type information.

## Lifecycle

Zero-initialize a stack before its first initialization. Do not initialize a live stack again; call `st_Destroy` first. After destruction, the reset stack may be initialized again.

See [ownership](ownership.md) for custom resource rules and [storage composition](storage.md) for the contiguous-storage design.
