# Polymorphism

libadt demonstrates runtime polymorphism in C by combining a common leading
structure with a vtable.

## The shared base

`ADT_Super_t` is the first member of every container:

```c
typedef struct
{
    ADT_Super_t super;
    struct
    {
        void *data;
        size_t capacity;
    } _private;
} DynamicArray_t;
```

```c
typedef struct
{
    ADT_Super_t super;
    struct
    {
        LinkedListNode_t *head;
        LinkedListNode_t *tail;
    } _private;
} LinkedList_t;
```

Static assertions in the container headers enforce that `super` remains at
offset zero. Therefore, a pointer to either complete container has the same
address as its `ADT_Super_t`.

Shared functions accept the opaque `ADT_t` parameter type and inspect only the
common leading state. Shared metadata and representation-specific fields are
grouped under `_private`; use `adt_Size()` and `adt_ElementType()` to inspect public
metadata.

The `_private` name documents an unsafe boundary rather than enforcing one,
because C does not support private structure members. Linked-list nodes use a
public forward declaration while their complete definition remains in an
internal detail header.

## Traversal vtable

Containers store elements differently, so the shared layer does not perform
storage-specific indexing. Each container supplies two traversal operations:

- `visit` exposes every element as `const void *`.
- `visitMutable` exposes every element as `void *`.

The vtable also provides a human-readable container name. Dynamic-array
traversal advances through contiguous storage, while linked-list traversal
follows node links.

This allows one function to process either representation:

```c
static bool ProcessContainer(ADT_t *container)
{
    return adt_Print(container) &&
           adt_Sort(container, ADT_SORT_QUICK) &&
           adt_Print(container);
}
```

```c
ADT_t *containers[] = {&array, &list};

for (size_t i = 0; i < ARRAY_COUNT(containers); i++)
{
    ProcessContainer(containers[i]);
}
```

## Shared operations

| Function | Required behavior |
| --- | --- |
| `adt_ForEach` | Read-only traversal |
| `adt_ForEachMutable` | Mutable traversal |
| `adt_Print` | Read-only traversal and an element printer |
| `adt_Min`, `adt_Max` | Read-only traversal and an element comparator |
| `adt_Mean`, `adt_Median`, `adt_Mode` | Read-only traversal and a numeric projection |
| `adt_Sort` | Read-only and mutable traversal plus a comparator |

Sorting first copies elements into a temporary contiguous buffer, sorts that
buffer, and writes the reordered values back through mutable traversal. This
keeps sorting independent of the container's physical representation.

## Concrete operations stay concrete

Operations such as `da_Append` and `ll_Insert` are not part of
`ADT_Super_t`. They describe sequence storage, not behavior guaranteed for
every future ADT.

A stack should expose `push`, `pop`, and `peek`; a queue should expose
`enqueue`, `dequeue`, and `front`. Keeping these operations out of the universal
vtable prevents unsupported base operations and unnecessary `NULL` function
pointers.

Generic wrappers follow the same rule. Primitive values are dispatched by
their C type, while custom values use the same operation name with an address.
Each concrete ADT explicitly defines which element operations it supports.

## Adding another container

A new polymorphic container must:

1. Place `ADT_Super_t` first.
2. Provide read-only and mutable traversal functions.
3. Define a static `ADT_VTable_t`.
4. Initialize the private vtable, size, and type fields in `super`.
5. Maintain the private size field as elements are added and removed.
6. Follow the same shallow-copy and destroy-callback ownership contract.

The new container can then use shared printing, extrema, and sorting without
exposing its internal representation to those algorithms.
