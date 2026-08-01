# Sorting

Every libadt container uses the shared `adt_*` sorting API.

The implementation copies through read-only traversal, sorts a temporary buffer, and writes back through mutable traversal.

## Default and Override Comparators

`adt_Sort` uses the comparator stored in the container's `ADT_ElementTypeInfo_t`:

```c
if (!adt_Sort(&students, ADT_SORT_QUICK))
{
    HandleSortFailure();
}
```

`adt_SortBy` replaces that comparator for one call without changing the descriptor:

```c
static int CompareStudentScoreDescending(const void *first, const void *second)
{
    const Student_t *left = first;
    const Student_t *right = second;
    return (right->score > left->score) - (right->score < left->score);
}

adt_SortBy(
    &students,
    ADT_SORT_INSERTION,
    CompareStudentScoreDescending);
```

Use the `By` form when one operation needs a different ordering. Both forms require a non-`NULL` comparator.

## Checking Existing Order

`adt_isSorted` uses the configured comparator. `adt_isSortedBy` uses a per-call comparator.

Both are O(n) checks available for every container.

Binary search is available only for dynamic arrays and stacks. Those ADTs use `ContiguousStorage_t`.

Linked lists and queues do not provide binary search.

Binary search requires the same comparator that defines the current order. Check first when the ordering is uncertain:

```c
if (adt_isSorted(&numbers))
{
    int target = 42;
    size_t index = 0;
    da_BinarySearch(&numbers, &target, &index);
}
```

The default sortedness and binary-search functions use the comparator stored during initialization. This is sufficient when a type uses one ordering.

For an override ordering, pass the same comparator to both `By` functions:

```c
if (adt_isSortedBy(&numbers, CompareStudentScoreDescending) &&
    da_BinarySearchBy(
        &numbers,
        CompareStudentScoreDescending,
        &target,
        &index))
{
    UseIndex(index);
}
```

Empty, single-element, and equal-element sequences are sorted.

The check costs O(n). Binary search costs O(log n). Skip the check when the ordering is already known.

### Why Sortedness Is Checked on Demand

Sortedness is checked on demand because a cache is easy to invalidate incorrectly:

- Sortedness depends on the comparator.
- Every mutable operation would need to update the cache.
- Referenced comparison keys can change outside the container.
- Comparator behavior can depend on external state.

Callers may check uncertain ordering or search directly under a known ordering contract.

A future cache would need a known/unknown state, comparator identity, and systematic mutation tracking. A single flag in `ADT_Super_t` would not be sufficient.

## Available Algorithms

The algorithm is selected with `ADT_SortAlgorithm_t` from `include/libadt/abstract_data_type.h`:

| Enum Value | Behavior | Stability | Typical Complexity |
| --- | --- | --- | --- |
| `ADT_SORT_BUBBLE` | Repeatedly swaps adjacent out-of-order elements and stops early when no swaps occur | Stable | O(n) best case; O(n²) average and worst case |
| `ADT_SORT_SELECTION` | Selects the smallest remaining element for each position | Not stable | O(n²) |
| `ADT_SORT_INSERTION` | Inserts each element into the already sorted prefix | Stable | O(n) best case; O(n²) average and worst case |
| `ADT_SORT_QUICK` | Partitions elements around a pivot | Not stable | O(n log n) average; O(n²) worst case |
| `ADT_SORT_BOGO` | Randomly shuffles until sorted or until a safety limit is reached | Not stable | Intentionally impractical |

Quick sort is the general-purpose choice. Insertion sort suits small or nearly sorted inputs.

Bubble and selection sort are straightforward alternatives. Bogo sort is only a bounded demonstration.

## Bounded Bogo Sort

Bogo sort first checks for existing order. It rejects unsorted collections larger than eight elements and stops after 100,000 shuffles.

Failure returns `false` without changing the container.

## Representation-Independent Sorting

The shared sorting path performs three steps:

1. Copy each element into a temporary contiguous buffer through `adt_ForEach`.
2. Run the selected algorithm on that buffer.
3. Copy the sorted bytes back through `adt_ForEachMutable`.

This design supports every container without exposing its storage layout.

It uses temporary memory proportional to the stored data, even for contiguous containers.

Sorting rearranges shallow element records without calling their destroy callbacks. Resources referenced by an element move with that record, so ownership remains with the same logical stored element.

## Failure Behavior

Sorting returns `false` when:

- The container is `NULL` or does not provide both traversal functions.
- The element size is zero.
- The selected comparator is `NULL`.
- The algorithm value is not part of `ADT_SortAlgorithm_t`.
- The temporary buffer size overflows or allocation fails.
- Bounded bogo sort cannot complete within its limits.

Sortedness functions return `false` for invalid state, a missing comparator, or allocation failure.

Therefore, `false` can mean either "not sorted" or "the check could not be performed."

The implementation finishes sorting the temporary buffer before writing anything back. Allocation, validation, and bogo-limit failures therefore leave the container order unchanged.

## Adding Another Algorithm

To add an algorithm:

1. Add its `ADT_SortAlgorithm_t` value.
2. Implement its buffer function in `src/shared/sorting.c`.
3. Connect it to `adt_SortBy`.
4. Test normal, empty, invalid, and custom-comparator cases.

See [`examples/sorting/algorithms.c`](../examples/sorting/algorithms.c) for every built-in algorithm and [`examples/sorting/custom_order.c`](../examples/sorting/custom_order.c) for per-call comparator overrides.
