# Storage layer

The containers separate raw storage from public ADT behavior through
composition and delegation. This allows multiple ADTs to reuse allocation and
mutation code without inheriting another container's interface.

| Container | Internal storage | Layout |
| --- | --- | --- |
| `DynamicArray_t` | `ContiguousStorage_t` | Resizable allocation |
| `Stack_t` | `ContiguousStorage_t` | Resizable allocation |
| `LinkedList_t` | `LinkedStorage_t` | Doubly linked nodes |
| `Queue_t` | `LinkedStorage_t` | Doubly linked nodes |

Each container owns a storage component. Storage is not an `ADT_Super_t`
subtype and does not provide user-facing container operations.

This boundary is intentionally narrow:

- Storage knows where bytes live.
- The container knows what those bytes mean.

## Division of work

The storage layer handles:

- Allocating and freeing raw memory
- Growing contiguous capacity or updating node links
- Copying element bytes
- Inserting and erasing physical storage
- Returning internal element addresses
- Checking whether an address points into owned storage

The container handles:

- `ADT_Super_t` and the logical size
- `ADT_ElementTypeInfo_t`
- Element callbacks and ownership
- Vtable traversal
- Public operations such as `Append`, `Push`, or `Dequeue`

Storage only knows the number of bytes in an element. It never calls an
element's comparator, printer, converter, or destroy function. Keeping that
dependency pointed one way makes the storage useful to more than one ADT.

## Why storage does not keep the size

The logical size in `ADT_Super_t` is the single source of truth. Storage
functions receive it when they need bounds or an insertion position. A second
stored count could diverge from the public ADT state.

The concrete container updates the shared size only after a storage operation
succeeds.

## Internal element pointers

`contiguousStorage_At` and `linkedStorage_At` return borrowed addresses inside
storage. The `AtConst` versions return `const` addresses. These pointers alias
container-owned memory; they are not element copies.

The public functions use output parameters instead:

```c
int value = 0;
da_Get(&array, index, &value);
st_Peek(&stack, &value);
qu_Front(&queue, &value);
```

Growing or shifting contiguous storage can invalidate its internal pointers. A
linked element pointer stays valid until its node is erased or the storage is
destroyed. Neither kind of pointer should be freed by the caller.

## Address checks

Contiguous storage can check one allocation range in O(1). Linked storage must
walk its separate element allocations, so its check is O(n).

Containers use these checks during `Take`, `Pop`, and `Dequeue`.

For example, taking an array element into an output address inside that same
array is unsafe: shifting the remaining bytes may overwrite the output.
Linked storage has the same problem in a different form—the removed node may
own the output address that is about to be freed.

## Internal visibility

The storage headers live under `include/libadt/internal/storage`. They are used
by container implementations and are not part of the supported user API.

The layouts still have to be visible to the public container headers because
the containers can be allocated as local values. C cannot make those fields
private, so `_private` and `internal` are warnings rather than enforcement.

## Stack and queue reuse

The payoff is visible in stack and queue:

- `Stack_t` treats the end of contiguous storage as its top.
- `Queue_t` treats the linked head as its front and tail as its back.

They reuse allocation and mutation code without exposing dynamic-array or
linked-list operations that do not belong in their public APIs.
