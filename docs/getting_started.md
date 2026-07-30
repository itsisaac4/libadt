# Getting started

libadt is a C23 container library built around runtime element information.
Dynamic arrays and linked lists have concrete storage APIs while printing,
traversal, extrema, and sorting operate through their shared `ADT_Super_t`
state.

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
cc -std=c23 -Iinclude program.c src/shared/*.c src/containers/*.c -o program
```

## Create a container

Include the header for the concrete container you want to use. The
initialization macros infer type information for supported primitive types:

```c
#include <stdlib.h>

#include "libadt/dynamic_array.h"

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

Always zero-initialize the container, check operations that return `bool`, and
call the matching destroy function when finished.

`DA_INIT_FROM` and `LL_INIT_FROM` require a fixed-size C array. Passing a pointer
would make `ARRAY_COUNT` calculate the size of the pointer rather than the
number of elements.

## Element operations

The same operation names accept every initialized element type. Supported
primitives can be passed directly:

```c
da_Append(&numbers, 42);
ll_Prepend(&numbersList, 42);
```

The supported primitive types are `char`, `int`, `unsigned int`, `long`,
`float`, and `double`. Value dispatch uses C23 `_Generic`.

Pass the address of custom structures, pointer elements, function pointers, and
other types:

```c
Person_t person = {.id = 1001};
da_Append(&people, &person);
```

The argument type must match the type used to initialize the container. The
container copies `sizeof(Person_t)` bytes from the supplied address; it does not
perform a deep copy.

## Shared operations

Functions prefixed with `adt_` accept any initialized libadt container:

```c
adt_Print(&numbers);
adt_Min(&numbers, &minimum);
adt_Max(&numbers, &maximum);
adt_Sort(&numbers, ADT_SORT_INSERTION);
```

These functions use the container's traversal vtable and configured type
callbacks. See [runtime type information](runtime_type_info.md) and
[polymorphism](polymorphism.md) for the underlying design.

## Next steps

- [Dynamic arrays](dynamic_array.md)
- [Linked lists](linked_list.md)
- [Ownership and shallow copying](ownership.md)
- [Runtime type information](runtime_type_info.md)
- [Polymorphism](polymorphism.md)
- [Compilable examples](../examples/README.md)
