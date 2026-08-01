# Sorting

Every libadt container can be sorted through the shared `adt_*` API. The public call stays the same across representations because the implementation reads elements through read-only traversal, sorts a temporary contiguous buffer, and writes the result back through mutable traversal.

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

This is useful when the descriptor defines the normal ordering, such as student ID, but one operation needs a different ordering, such as descending score. Both functions require a non-`NULL` comparator; the `By` form is an override, not a fallback to the stored comparator.

## Available Algorithms

The algorithm is selected with `ADT_SortAlgorithm_t` from `include/libadt/abstract_data_type.h`:

| Enum Value | Behavior | Stability | Typical Complexity |
| --- | --- | --- | --- |
| `ADT_SORT_BUBBLE` | Repeatedly swaps adjacent out-of-order elements and stops early when no swaps occur | Stable | O(n) best case; O(n²) average and worst case |
| `ADT_SORT_SELECTION` | Selects the smallest remaining element for each position | Not stable | O(n²) |
| `ADT_SORT_INSERTION` | Inserts each element into the already sorted prefix | Stable | O(n) best case; O(n²) average and worst case |
| `ADT_SORT_QUICK` | Partitions elements around a pivot | Not stable | O(n log n) average; O(n²) worst case |
| `ADT_SORT_BOGO` | Randomly shuffles until sorted or until a safety limit is reached | Not stable | Intentionally impractical |

Quick sort is the general-purpose choice in this library. Insertion sort is useful for small or nearly sorted inputs, while bubble and selection sort are included as straightforward algorithm implementations. Bogo sort is included as a bounded demonstration rather than a practical choice.

## Bounded Bogo Sort

The bogo implementation first checks whether the input is already sorted. Otherwise, it refuses collections larger than eight elements and stops after 100,000 shuffles. If it reaches either bound without producing a sorted order, `adt_Sort` returns `false` and the container keeps its original order.

## Representation-Independent Sorting

The shared sorting path performs three steps:

1. Copy each element into a temporary contiguous buffer through `adt_ForEach`.
2. Run the selected algorithm on that buffer.
3. Copy the sorted bytes back through `adt_ForEachMutable`.

This design lets one implementation sort dynamic arrays, linked lists, stacks, and queues without exposing their storage layouts to the sorting algorithms. It uses temporary memory proportional to the number and size of the stored elements, even when the original container already uses contiguous storage.

Sorting rearranges shallow element records without calling their destroy callbacks. Resources referenced by an element move with that record, so ownership remains with the same logical stored element.

## Failure Behavior

Sorting returns `false` when:

- The container is `NULL` or does not provide both traversal functions.
- The element size is zero.
- The selected comparator is `NULL`.
- The algorithm value is not part of `ADT_SortAlgorithm_t`.
- The temporary buffer size overflows or allocation fails.
- Bounded bogo sort cannot complete within its limits.

The implementation finishes sorting the temporary buffer before writing anything back. Allocation, validation, and bogo-limit failures therefore leave the container order unchanged.

## Adding Another Algorithm

Adding an algorithm requires more than adding a name to the enum. Add the new value to `ADT_SortAlgorithm_t`, implement its temporary-buffer sorting function in `src/shared/sorting.c`, connect it to the dispatch switch in `adt_SortBy`, and add tests for normal, empty, invalid, and custom-comparator cases.

See [`examples/sorting/algorithms.c`](../examples/sorting/algorithms.c) for every built-in algorithm and [`examples/sorting/custom_order.c`](../examples/sorting/custom_order.c) for per-call comparator overrides.
