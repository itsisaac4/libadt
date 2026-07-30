# libadt

A generic C23 abstract data type library for the CS3003 Programming Languages
final project. It demonstrates encapsulation, runtime polymorphism, generic
programming, and resource ownership in an imperative language.

## Containers

- `DynamicArray_t`: resizable contiguous storage.
- `LinkedList_t`: doubly linked storage.
- Stack and queue adapters are planned.

Both containers begin with `ADT_Super_t`. Shared functions accept `ADT_t *` and
dispatch traversal through `ADT_VTable_t`, allowing the same call to work with
either container:

```c
adt_Print(&container);
adt_Min(&container, &minimum);
adt_Max(&container, &maximum);
adt_Sort(&container, ADT_SORT_QUICK);
```

## Runtime type information

`ADT_TypeInfo_t` describes each stored element:

- `elementSize` controls storage and copying.
- `compare` defines equality and ordering.
- `print` defines element output.
- `destroy` releases resources owned by an element.

Containers own their element storage. Operations make shallow element copies.
A configured destroy callback releases only resources owned by an element.
`Remove` destroys those resources, while `Take` transfers them to the caller.

C23 initialization and primitive value wrappers keep common calls concise:

```c
int values[] = {3, 1, 2};
DynamicArray_t array = {0};
LinkedList_t list = {0};

DA_INIT_FROM(&array, values);
LL_INIT(&list, int);
ll_AppendValue(&list, 4);
```

Custom types use the `Ref` APIs with the address of their element storage.

## Documentation

- [Getting started](docs/getting_started.md)
- [Ownership and shallow copying](docs/ownership.md)
- [Runtime type information](docs/runtime_type_info.md)
- [Polymorphism](docs/polymorphism.md)
- [Dynamic arrays](docs/dynamic_array.md)
- [Linked lists](docs/linked_list.md)
- [Compilable examples](examples/README.md)

Primitive `_Generic` dispatch is isolated in `primitive_dispatch.h`. Container
storage implementations and generated primitive wrappers are also separated:

```text
src/
├── shared/
│   ├── abstract_data_type.c
│   ├── statistics.c
│   ├── sorting.c
│   ├── comparators.c
│   └── printers.c
└── containers/
    ├── dynamic_array.c
    ├── dynamic_array_primitives.c
    ├── linked_list.c
    └── linked_list_primitives.c
```

## Sorting

The shared sorting API supports:

- Bubble sort
- Selection sort
- Insertion sort
- Quick sort
- Bounded bogo sort

`adt_Sort` uses the configured comparator. `adt_SortBy` accepts an override for
alternate orderings of the same type.

## Build and test

```sh
make build
make test
make sanitize
```

The test suite uses CppUTest and includes C23 compile checks for `_Generic`
dispatch. Examples are documented in [`examples/README.md`](examples/README.md).
