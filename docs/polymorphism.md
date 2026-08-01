# Polymorphism

I combined first-member embedding with a small vtable so the shared API could operate on every container without hiding their different representations. The shared layout supports inheritance-like upcasting, while the vtable provides dynamic dispatch.

These are two separate mechanisms:

1. `ADT_Super_t` gives shared functions a common view of container state.
2. `ADT_VTable_t` tells those functions how to traverse the concrete representation.

The first shares data layout; the second selects behavior.

## Shared Layout

`ADT_Super_t` is the first member of every container:

```c
typedef struct
{
    ADT_Super_t super;
    struct
    {
        ContiguousStorage_t storage;
    } _private;
} DynamicArray_t;
```

```c
typedef struct
{
    ADT_Super_t super;
    struct
    {
        LinkedStorage_t storage;
    } _private;
} LinkedList_t;
```

The container headers use static assertions to keep `super` at offset zero. The container and its embedded base therefore have the same address, allowing the container pointer to be upcast to `ADT_t *`. Shared functions only access the common base fields.

This is the part where C requires discipline. It is a memory-layout convention, not language-supported inheritance. Moving `super` away from the first member would invalidate the upcast, which is why the static assertions matter.

## Traversal Vtable

The shared algorithms need to visit elements, but they should not need to know whether those elements are in an array or linked nodes. Each container provides two functions through `ADT_VTable_t`:

- `visit` passes each element as `const void *`.
- `visitMutable` passes each element as `void *`.

The vtable also stores a container name for printing and debugging.

Why both visitors?

- Printing and statistics should not mutate elements, so they use `visit`.
- Sorting must write reordered elements back, so it uses `visitMutable`.

```c
static bool ProcessContainer(ADT_t *container)
{
    return adt_Print(container) &&
           adt_Sort(container, ADT_SORT_QUICK) &&
           adt_Print(container);
}
```

```c
ADT_t *containers[] = {&array, &list, &stack, &queue};

for (size_t i = 0; i < ARRAY_COUNT(containers); i++)
{
    ProcessContainer(containers[i]);
}
```

The call to `ProcessContainer` is the same for all four types. At runtime, dynamic dispatch selects the traversal function for the concrete container. Together, the type-erased base pointer and vtable provide runtime polymorphism.

## What the Shared Operations Require

| Function | What It Uses |
| --- | --- |
| `adt_ForEach` | Read-only traversal |
| `adt_ForEachMutable` | Mutable traversal |
| `adt_Print` | Read-only traversal and an element printer |
| `adt_Min`, `adt_Max` | Read-only traversal and a comparator |
| `adt_Mean`, `adt_Median`, `adt_Mode` | Read-only traversal and a numeric conversion |
| `adt_Sort` | Read-only traversal, mutable traversal, and a comparator |
| `adt_isSorted` | Read-only traversal and a comparator |

Sorting is the clearest tradeoff in this design:

1. Copy elements into a temporary contiguous buffer.
2. Sort the buffer.
3. Write the result back through mutable traversal.

That is not the most specialized algorithm for every representation, but it keeps one sorting implementation independent of array or node storage.

See [sorting](sorting.md) for the available algorithms and the consequences of this temporary-buffer design.

## What Stays Container-Specific

The vtable is deliberately small. Stack and queue endpoint operations remain container-specific.

Putting them in the shared base would weaken the abstraction.

The same rule applies to primitive wrappers. Each concrete ADT defines the element operations it actually supports.

## Adding Another Container

A new container can use the shared operations if it:

1. Places `ADT_Super_t` first.
2. Implements read-only and mutable traversal.
3. Creates an `ADT_VTable_t`.
4. Initializes the shared size, element information, and vtable.
5. Updates the shared size after successful insertions and removals.
6. Follows the library's shallow-copy and ownership rules.

The container may use an existing storage component or provide a different representation. See [storage composition](storage.md) for the storage layer.
