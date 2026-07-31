# Getting started

libadt provides dynamic arrays, linked lists, stacks, and queues. Start with a
concrete prefix such as `da_*` or `qu_*` for container operations, then use the
shared `adt_*` API for printing, traversal, statistics, and sorting.

## Build the project

From the repository root:

```sh
make build
make test
make -C examples
```

The tests require CppUTest. The library and examples require a compiler with
C23 support.

To compile a small program directly against the sources:

```sh
cc -std=c23 -Iinclude program.c \
    src/shared/*.c src/shared/element/*.c src/shared/storage/*.c \
    src/containers/*/*.c -o program
```

## Create a container

Include `libadt/libadt.h` for the full public API. The initialization macros
fill in type information for supported primitive types:

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

Individual container headers are also available if a program only wants part
of the API.

One easy trap: `DA_INIT_FROM`, `LL_INIT_FROM`, `ST_INIT_FROM`, and
`QU_INIT_FROM` require a fixed-size C array. A pointer does not carry its
element count, so it cannot be used here.

## Element operations

The same operation names accept every initialized element type. Supported
primitives can be passed directly:

```c
da_Append(&numbers, 42);
ll_Prepend(&numbersList, 42);
st_Push(&numberStack, 42);
qu_Enqueue(&numberQueue, 42);
```

The supported primitive types are `char`, `int`, `unsigned int`, `long`,
`float`, and `double`. Value dispatch uses C23 `_Generic`.

Pass the address of custom structures, pointer elements, function pointers, and
other types:

```c
Person_t person = {.id = 1001};
da_Append(&people, &person);
```

The important rule is that the argument type must match the type used to
initialize the container. The container copies `sizeof(Person_t)` bytes; it
does not perform a deep copy.

## Shared operations

Functions prefixed with `adt_` accept any initialized libadt container:

```c
adt_Print(&numbers);
adt_Min(&numbers, &minimum);
adt_Max(&numbers, &maximum);
adt_Mean(&numbers, &mean);
adt_Median(&numbers, &median);
adt_Mode(&numbers, &mode);
adt_Sort(&numbers, ADT_SORT_INSERTION);
```

These functions use the container's traversal vtable and element callbacks.
See [runtime type information](runtime_type_info.md) and
[polymorphism](polymorphism.md) for the implementation.

## Next steps

- [Dynamic arrays](dynamic_array.md)
- [Linked lists](linked_list.md)
- [Stacks](stack.md)
- [Queues](queue.md)
- [Storage composition](storage.md)
- [Ownership and shallow copying](ownership.md)
- [Runtime type information](runtime_type_info.md)
- [Numeric statistics](statistics.md)
- [Polymorphism](polymorphism.md)
- [Compilable examples](../examples/README.md)
