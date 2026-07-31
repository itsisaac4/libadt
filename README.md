# libadt

libadt is a generic C23 abstract data type library created for the CS3003
Programming Languages final project. It provides dynamic arrays and doubly
linked lists while demonstrating how abstraction, generic programming, runtime
polymorphism, and resource ownership can be implemented in an imperative
language without built-in classes or generics.

**Course:** Programming Languages (CS3003)

**Term:** Summer 2026

## Quick start

Build the library, run its tests, and compile every example from the repository
root:

```sh
make build
make test
make -C examples
```

The test suite requires CppUTest. The library requires a compiler with C23
support.

```c
#include <stdlib.h>

#include "libadt/dynamic_array.h"

int main(void)
{
    int values[] = {3, 1, 2};
    DynamicArray_t numbers = {0};

    if (!DA_INIT_FROM(&numbers, values) ||
        !da_Append(&numbers, 4) ||
        !adt_Sort(&numbers, ADT_SORT_QUICK) ||
        !adt_Print(&numbers))
    {
        da_Destroy(&numbers);
        return EXIT_FAILURE;
    }

    da_Destroy(&numbers);
    return EXIT_SUCCESS;
}
```

The same operation names support every configured element type. Supported
primitives are passed directly:

```c
ll_Append(&numbers, 4);
```

Structures, pointer elements, function pointers, and other types are passed by
address:

```c
Student_t student = {.id = 1001};
da_Append(&students, &student);
```

In both cases, the argument must match the element type used to initialize the
container.

## Features

- Resizable contiguous storage with `DynamicArray_t`
- Doubly linked storage with `LinkedList_t`
- Shared traversal, printing, extrema, and sorting
- Runtime element information through `ADT_ElementTypeInfo_t`
- C23 `_Generic` element operations
- Custom comparators, printers, and resource destructors
- Explicit resource transfer through `Take`
- Bubble, selection, insertion, quick, and bounded bogo sort
- Overflow, invalid-argument, aliasing, and ownership checks

Stack and queue containers are planned.

## Shared operations

Both containers begin with `ADT_Super_t`. Shared functions accept `ADT_t *` and
dispatch traversal through `ADT_VTable_t`, allowing the same function to
operate on either representation:

```c
adt_Print(&container);
adt_Min(&container, &minimum);
adt_Max(&container, &maximum);
adt_Mean(&container, &mean);
adt_Median(&container, &median);
adt_Mode(&container, &mode);
adt_Sort(&container, ADT_SORT_QUICK);
```

`adt_Sort` uses the configured comparator. `adt_SortBy` accepts a comparator
override for alternate orderings of the same element type.

## Runtime type information

`ADT_ElementTypeInfo_t` defines the size and behavior of one stored element:

- `elementSize` controls allocation and shallow copying.
- `compare` defines equality and ordering.
- `print` defines how one element is displayed.
- `toNumber` projects an element for numeric statistics.
- `destroy` releases resources owned by an element.

Containers own their element storage. Operations make shallow copies of element
bytes. A destroy callback releases only resources owned by an element.
`Remove` destroys those resources, while `Take` transfers them to the caller.

## Documentation

- [Getting started](docs/getting_started.md)
- [Custom element types](docs/custom_types.md)
- [Ownership and shallow copying](docs/ownership.md)
- [Runtime type information](docs/runtime_type_info.md)
- [Numeric statistics](docs/statistics.md)
- [Polymorphism](docs/polymorphism.md)
- [Dynamic arrays](docs/dynamic_array.md)
- [Linked lists](docs/linked_list.md)
- [Compilable examples](examples/README.md)

## Repository structure

```text
include/libadt/
├── abstract_data_type.h
├── dynamic_array.h
├── linked_list.h
├── element/
│   ├── comparators.h
│   ├── number_converters.h
│   └── printers.h
└── internal/
    ├── dynamic_array_operations.h
    ├── linked_list_operations.h
    └── primitive_dispatch.h

src/
├── shared/
│   ├── abstract_data_type.c
│   ├── statistics.c
│   ├── sorting.c
│   └── element/
│       ├── comparators.c
│       ├── number_converters.c
│       └── printers.c
└── containers/
    ├── dynamic_array/
    │   ├── dynamic_array.c
    │   └── primitive_operations.c
    └── linked_list/
        ├── linked_list.c
        ├── linked_list_node.h
        └── primitive_operations.c
```

# Final project write-up

## Overview

This project applies the imperative paradigm by implementing reusable abstract
data types with explicit control flow and memory management. C23 was chosen
because it does not provide classes, inheritance, function overloading, or
built-in generic containers. Although it does include `_Generic()` and `typeof()` functions that are not available in earlier versions of C. Working around these limits is the main focus of
the project.

The library currently implements dynamic arrays and doubly linked lists. Both
support the same element model and can share operations such as printing,
extrema, numeric statistics, and sorting.

## Connections to course concepts

| Course concept | Use in this project |
| --- | --- |
| Abstract data types | Users work through container operations instead of implementing storage algorithms themselves. |
| Encapsulation | Source files and `detail` APIs separate supported operations from implementation details. |
| Composition/ Inheritance | Each container includes `ADT_Super_t` as its first member to model super/sub classes. |
| Runtime polymorphism | A vtable lets shared functions traverse different container types. |
| Dynamic dispatch | The active container selects its own traversal functions at runtime. |
| Generic programming | `void *`, runtime type information, and C23 `_Generic` allow one library to support many element types. |
| Higher-order functions | Comparators, printers, destructors, and visitors are passed as function pointers. |
| Resource ownership | `Remove` destroys owned resources, while `Take` transfers them. |

## Abstraction design

Each container starts with `ADT_Super_t`, whose private state stores its size,
element information, and vtable. This gives dynamic arrays and linked lists a
common interface, so functions such as `adt_Print` and `adt_Sort` can work with
either one. The vtable provides dynamic dispatch by selecting the correct
traversal code at runtime.

Operations that are not shared stay with the concrete container. This keeps the
base abstraction small and lets future stacks and queues use operations that
fit their behavior.

## Generic element operations

The containers store bytes instead of one fixed C type. `ADT_ElementTypeInfo_t`
provides the element size and optional compare, print, numeric projection, and
destroy functions. C23 `_Generic` then chooses how a public operation handles
its argument:

```c
da_Append(&numbers, 5);
da_Append(&students, &student);
```

Supported primitives are passed directly; other types are passed by address.
This is how the project provides generic programming without C++ templates or
Java-style generic types.

### Tooling tradeoff

`_Generic` must be used in a macro because C does not support function
overloading. The resulting API is concise, but editors may show the macro
instead of a normal function signature or report errors through an internal
function. A function-only API would improve editor hints but require every
value to be passed by address. This project keeps the macro to demonstrate C23
generic programming.

## Encapsulation constraints

C has no private structure members. The container layouts remain visible so
users can allocate them on the stack:

```c
DynamicArray_t array = {0};
```

Internal fields are grouped under `_private` to warn users that changing them
can violate container invariants. Linked-list nodes are opaque outside the
implementation. This convention does not enforce privacy, but the public API,
separate source files, and `detail`-named functions provide practical
encapsulation while keeping stack allocation simple.

## Pointer safety and runtime typing

The storage code uses `void *`, so an element's original compile-time type is
not available inside the container. Its size and behavior are restored through
`ADT_ElementTypeInfo_t`.

The library checks primitive sizes, indexes, `NULL` arguments, allocation
failures, overflow, and unsafe storage aliasing. For a custom type, the caller
must still pass an address matching the type used at initialization. C cannot
fully verify that pointer after it has been converted to `void *`.

## Ownership model

Containers own their element storage and make shallow copies. A destroy
callback releases resources owned by an element. `Remove` destroys those
resources, while `Take` transfers them to the caller. This keeps ownership
changes explicit instead of hiding them behind automatic memory management.

## Testing and development

CppUTest covers container operations, invalid arguments, ownership, overflow
protection, primitive and custom types, function pointers, shared traversal,
printing, statistics, and sorting.

```sh
make test
make sanitize
```

The repository was developed through incremental commits for the build system,
dynamic array, linked list, shared polymorphism, primitive dispatch, and
documentation rather than one final upload.

## Current limitations and future work

- Stack and queue containers are planned.
- Custom elements are shallow-copied; there is no automatic clone callback.
- Stack allocation prevents complete enforcement of private container fields.
- A live container must be destroyed before being initialized again.

More detailed explanations are available in the linked files under `docs/`.
