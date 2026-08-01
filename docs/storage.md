# Storage Layer

I separated raw storage from public ADT behavior through composition and delegation. This lets multiple ADTs reuse allocation and mutation code without making one public container inherit another container's interface.

| Container | Internal Storage | Layout |
| --- | --- | --- |
| `DynamicArray_t` | `ContiguousStorage_t` | Resizable allocation |
| `Stack_t` | `ContiguousStorage_t` | Resizable allocation |
| `LinkedList_t` | `LinkedStorage_t` | Doubly linked nodes |
| `Queue_t` | `LinkedStorage_t` | Doubly linked nodes |

Each container owns a storage component. Storage is not an `ADT_Super_t` subtype and does not provide user-facing container operations.

I kept this boundary intentionally narrow:

- Storage knows where bytes live.
- The container knows what those bytes mean.

## Division of Work

The storage layer handles:

- Allocating and freeing raw memory
- Growing contiguous capacity or updating node links
- Copying element bytes
- Inserting and erasing physical storage
- Returning internal element addresses
- Checking whether an address points into owned storage
- Binary-searching an already sorted contiguous range

The container handles:

- `ADT_Super_t` and the logical size
- `ADT_ElementTypeInfo_t`
- Element callbacks and ownership
- Vtable traversal
- Public operations such as `Append`, `Push`, or `Dequeue`

Storage normally knows only the element size. `contiguousStorage_BinarySearchBy` also receives a comparator for one call.

Storage does not own or select callbacks. It never uses printers, numeric converters, or destroy functions.

## Contiguous Binary Search

`contiguousStorage_BinarySearchBy` searches the first `count` slots in O(log n). They must use the supplied comparator's ordering.

Duplicate searches return the first physical match. A missing target leaves the output index unchanged.

Dynamic arrays and stacks expose the public binary-search functions. Stack index zero is the bottom.

Linked lists and queues use `LinkedStorage_t`, so they do not expose binary search.

The shared O(n) sortedness checks work with both storage representations. They can verify uncertain ordering before a search.

Default functions use the initialization comparator. The `By` forms must receive the same override comparator.

## Why Storage Does Not Keep the Size

The logical size in `ADT_Super_t` is the single source of truth. Storage functions receive it when they need bounds or an insertion position. A second stored count could diverge from the public ADT state.

The concrete container updates the shared size only after a storage operation succeeds.

## Internal Element Pointers

`contiguousStorage_At` and `linkedStorage_At` return borrowed addresses inside storage. The `AtConst` versions return `const` addresses. These pointers alias container-owned memory; they are not element copies.

The public functions use output parameters instead:

```c
int value = 0;
da_Get(&array, index, &value);
st_Peek(&stack, &value);
qu_Front(&queue, &value);
```

Growing or shifting contiguous storage can invalidate its internal pointers. A linked element pointer stays valid until its node is erased or the storage is destroyed. Neither kind of pointer should be freed by the caller.

## Address Checks

Contiguous storage can check one allocation range in O(1). Linked storage must walk its separate element allocations, so its check is O(n).

Containers use these checks during `Take`, `Pop`, and `Dequeue`.

Taking an array element into its own storage is unsafe because shifting may overwrite the output.

Linked storage has a similar risk: the removed node may own the output address being freed.

## Internal Visibility

The storage headers live under `include/libadt/internal/storage`. They are used by container implementations and are not part of the supported user API.

The layouts still have to be visible to the public container headers because the containers can be allocated as local values. C cannot make those fields private, so `_private` and `internal` are warnings rather than enforcement.

## Stack and Queue Reuse

The payoff is visible in stack and queue:

- `Stack_t` treats the end of contiguous storage as its top.
- `Queue_t` treats the linked head as its front and tail as its back.

For a stack, pushing and popping at the end avoids shifting existing elements; capacity only grows when the contiguous allocation is full. This also keeps stack traversal cache-friendly and avoids a separate allocation for every element.

Linked storage lets queues append at the tail and unlink from the head without shifting elements.

The tradeoff is separate allocations and weaker memory locality.

Both ADTs reuse allocation and mutation code without exposing dynamic-array or linked-list operations that do not belong in their public APIs.
