# libadt examples

Each example is a focused C23 program with its own executable. From this
directory, run `make list`, `make` to build everything, or one target below.
The examples include `libadt/libadt.h`, which exposes the complete public API.

| Target | Example | Demonstrates |
| --- | --- | --- |
| `make polymorphism` | `common/polymorphism.c` | One shared function over all four container types |
| `make dynamic-array` | `dynamic_array/quick_start.c` | Unified element operations and lookup |
| `make dynamic-array-basic` | `dynamic_array/basic.c` | Unchecked array operations with state after each change |
| `make student-records` | `dynamic_array/student_records.c` | Custom type information, comparison, and printing |
| `make debug-view` | `dynamic_array/debug_view.c` | Shared `ADT_DEBUG` output |
| `make linked-list` | `linked_list/quick_start.c` | Typed initialization, insertion, printing, and `ll_Take` |
| `make linked-list-basic` | `linked_list/basic.c` | Unchecked list operations with state after each change |
| `make owned-records` | `linked_list/owned_records.c` | Resource destruction and ownership transfer |
| `make stack` | `stack/quick_start.c` | Contiguous-storage push, peek, and LIFO pop |
| `make stack-basic` | `stack/basic.c` | Unchecked stack operations with state after each change |
| `make queue` | `queue/quick_start.c` | Linked-storage enqueue, front/back, and FIFO dequeue |
| `make queue-basic` | `queue/basic.c` | Unchecked queue operations with state after each change |
| `make sorting` | `sorting/algorithms.c` | Bubble, selection, insertion, quick, and bounded bogo sort |
| `make custom-order` | `sorting/custom_order.c` | Default and per-call comparators for a custom type |
| `make statistics` | `statistics/summary.c` | Mean, median, and mode through one polymorphic API |

The source and generated executables remain separate:

```text
examples/
├── common/
├── dynamic_array/
├── linked_list/
├── stack/
├── queue/
├── sorting/
├── statistics/
└── build/
```

Run `make clean` to remove generated example executables.
