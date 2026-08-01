# Queues

`Queue_t` stores elements in first-in, first-out order. It uses `LinkedStorage_t`, adding at the tail and removing from the head.

The linked representation keeps both enqueue and dequeue at O(1) without shifting a contiguous buffer.

## Quick Start

```c
#include "libadt/libadt.h"

Queue_t queue = {0};

if (QU_INIT(&queue, int))
{
    qu_Enqueue(&queue, 10);
    qu_Enqueue(&queue, 20);

    int value = 0;
    qu_Dequeue(&queue, &value);
    qu_Destroy(&queue);
}
```

## Operations

| Category | Functions |
| --- | --- |
| Initialization | `qu_Init`, `qu_InitFrom`, `QU_INIT`, `QU_INIT_FROM` |
| Access | `qu_Front`, `qu_Back` |
| Insertion | `qu_Enqueue` |
| Removal | `qu_Dequeue`, `qu_Discard`, `qu_Clear`, `qu_Destroy` |
| Shared | `adt_Size`, `adt_IsEmpty`, `adt_Print`, statistics, `adt_Sort` |

`QU_INIT_FROM` reads its input from front to back. Shared traversal uses the same order.

Use `QU_INIT` and `QU_INIT_FROM` for inferred primitive behavior. Use `qu_Init` or `qu_InitFrom` with an `ADT_ElementTypeInfo_t` when the element is a custom type.

## Ownership

`qu_Enqueue` stores a shallow copy. `qu_Dequeue` transfers the removed element and its owned resources to the caller. `qu_Discard`, `qu_Clear`, and `qu_Destroy` invoke the configured destroy callback.

The output passed to `qu_Dequeue` cannot point into any element allocation owned by the queue.

## Complexity

| Operation | Complexity |
| --- | --- |
| Enqueue | O(1) |
| Front or back | O(1) |
| Dequeue or discard | O(1) |
| Clear or destroy | O(n) |

Clearing a queue releases all nodes but retains its runtime element information. Destroying it also resets that shared information.

## Lifecycle

Zero-initialize a queue before its first initialization. Do not initialize a live queue again; call `qu_Destroy` first. After destruction, the reset queue may be initialized again.

See [ownership](ownership.md) for custom resource rules and [storage composition](storage.md) for the linked-storage design.
