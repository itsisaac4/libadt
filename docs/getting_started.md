# Getting Started

libadt provides dynamic arrays, linked lists, stacks, and queues. Start with a concrete prefix such as `da_*` or `qu_*` for container operations, then use the shared `adt_*` API for printing, traversal, statistics, and sorting.

## Build the Project

From the repository root:

```sh
make build
make test
make -C examples
```

The tests require CppUTest. The library and examples require a compiler with C23 support.

To compile a small program directly against the sources:

```sh
cc -std=c23 -Iinclude program.c \
    src/shared/*.c src/shared/element/*.c src/shared/storage/*.c \
    src/containers/*/*.c -o program
```

## Create a Container

Include `libadt/libadt.h` for the full public API. The initialization macros fill in type information for supported primitive types:

```c
#include <stdlib.h>

#include "libadt/libadt.h"

int main(void)
{
    int initial[] = {30, 10, 20};
    DynamicArray_t numbers = {0};

    if (!DA_INIT_FROM(&numbers, initial) ||
        !da_Append(&numbers, 40))
    {
        da_Destroy(&numbers);
        return EXIT_FAILURE;
    }

    adt_Sort(&numbers, ADT_SORT_QUICK);
    adt_Print(&numbers);

    da_Destroy(&numbers);
    return EXIT_SUCCESS;
}
```

Three rules cover most first-time mistakes:

- Zero-initialize the container.
- Check operations that return `bool`.
- Call the matching destroy function when finished.

### Initialization Forms

Each container has two uppercase convenience macros and two lowercase initialization functions. Use the `DA`/`da`, `LL`/`ll`, `ST`/`st`, or `QU`/`qu` prefix for the container being initialized.

The `*_INIT` macros create an empty primitive container and infer its comparator, printer, and numeric projection from the type:

```c
DynamicArray_t numbers = {0};

if (!DA_INIT(&numbers, int))
{
    return false;
}
```

The `*_INIT_FROM` macros also infer the element count and type from a fixed-size C array:

```c
int values[] = {30, 10, 20};
DynamicArray_t numbers = {0};

if (!DA_INIT_FROM(&numbers, values))
{
    return false;
}
```

These macros support `char`, `int`, `unsigned int`, `long`, `float`, and `double`. An `_INIT_FROM` argument must be an actual array because the macro uses `ARRAY_COUNT`; a pointer does not contain its element count.

The lowercase `*_Init` functions accept an explicit descriptor, which is the normal choice for custom types:

```c
const ADT_ElementTypeInfo_t studentType = ADT_ELEMENT_TYPE_INFO(
    Student_t,
    CompareStudent,
    PrintStudent,
    StudentScoreToNumber,
    DestroyStudent);

DynamicArray_t students = {0};

if (!da_Init(&students, studentType))
{
    return false;
}
```

Use `*_InitFrom` when both an explicit descriptor and initial elements are available:

```c
if (!da_InitFrom(
        &students,
        initialStudents,
        studentCount,
        studentType))
{
    return false;
}
```

`*_InitFrom` accepts a pointer to contiguous elements and a separate `size_t` count. Both `FROM` forms make shallow copies in traversal order: index order for arrays and lists, bottom to top for stacks, and front to back for queues.

Do not initialize a live container again. Call its matching `Destroy` function first; destruction resets it so it can be initialized again.

Individual headers are also available when a program only wants part of the API. Include `libadt/abstract_data_type.h` for shared `adt_*` operations, then include only the concrete container headers the program uses.

## Element Operations

The same operation names accept every initialized element type. Supported primitives can be passed directly:

```c
da_Append(&numbers, 42);
ll_Prepend(&numbersList, 42);
st_Push(&numberStack, 42);
qu_Enqueue(&numberQueue, 42);
```

The supported primitive types are `char`, `int`, `unsigned int`, `long`, `float`, and `double`. Value dispatch uses C23 `_Generic`.

Macros such as `da_Append` and `st_Push` are the common entry points. For primitives, `_Generic` selects a value wrapper.

Custom structures, pointers, and function pointers use the reference path. Both paths keep the same public operation name.

Pass the address of custom structures, pointer elements, function pointers, and other types:

```c
Person_t person = {.id = 1001};
da_Append(&people, &person);
```

The important rule is that the argument type must match the type used to initialize the container. The macro improves call syntax, but after dispatch the implementation still works with type-erased bytes and cannot recover a mismatched source type.

## Shared Operations

Functions prefixed with `adt_` accept any initialized libadt container:

```c
adt_Print(&numbers);
adt_Min(&numbers, &minimum);
adt_Max(&numbers, &maximum);
adt_Mean(&numbers, &mean);
adt_Median(&numbers, &median);
adt_Mode(&numbers, &mode);
adt_Sort(&numbers, ADT_SORT_INSERTION);
bool sorted = adt_isSorted(&numbers);
```

These functions use the container's traversal vtable and element callbacks. See [runtime element type information](runtime_type_info.md) and [polymorphism](polymorphism.md) for the implementation.

Default extrema, sorting, and sortedness use `compare`. Default statistics use `toNumber`.

The `adt_*By` variants override a callback for one call without modifying the descriptor.

The O(n) sortedness checks work with every ADT. Binary search is limited to contiguous dynamic arrays and stacks.

Binary search requires the same comparator that defines the current order. When unsure, pair the default functions or pass one comparator to both `By` functions.

## Next Steps

- [Dynamic arrays](dynamic_array.md)
- [Linked lists](linked_list.md)
- [Stacks](stack.md)
- [Queues](queue.md)
- [Storage composition](storage.md)
- [Ownership and shallow copying](ownership.md)
- [Runtime element type information](runtime_type_info.md)
- [Numeric statistics](statistics.md)
- [Sorting](sorting.md)
- [Polymorphism](polymorphism.md)
- [Compilable examples](../examples/README.md)
