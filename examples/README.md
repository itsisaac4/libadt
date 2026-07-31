# libadt examples

Each example is a focused C23 program with its own executable. From this
directory, run `make list`, `make` to build everything, or one target below.

| Target | Example | Demonstrates |
| --- | --- | --- |
| `make polymorphism` | `common/polymorphism.c` | One shared function printing, inspecting, and sorting either container |
| `make dynamic-array` | `dynamic_array/quick_start.c` | Unified element operations and lookup |
| `make student-records` | `dynamic_array/student_records.c` | Custom type information, comparison, and printing |
| `make debug-view` | `dynamic_array/debug_view.c` | Shared `ADT_DEBUG` output |
| `make linked-list` | `linked_list/quick_start.c` | Typed initialization, insertion, printing, and `ll_Take` |
| `make owned-records` | `linked_list/owned_records.c` | Resource destruction and ownership transfer |
| `make sorting` | `sorting/algorithms.c` | Bubble, selection, insertion, quick, and bounded bogo sort |
| `make custom-order` | `sorting/custom_order.c` | Default and per-call comparators for a custom type |
| `make statistics` | `statistics/summary.c` | Mean, median, and mode through one polymorphic API |

The source and generated executables remain separate:

```text
examples/
├── common/
├── dynamic_array/
├── linked_list/
├── sorting/
├── statistics/
└── build/
```

Run `make clean` to remove generated example executables.
