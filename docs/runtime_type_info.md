# Runtime type information

C does not attach runtime behavior to arbitrary values. libadt supplies that
behavior explicitly through `ADT_TypeInfo_t`:

```c
typedef struct
{
    size_t elementSize;
    CompareFn_t compare;
    PrintFn_t print;
    DestroyFn_t destroy;
} ADT_TypeInfo_t;
```

Every initialized container stores one copy of this information in its
`ADT_Super_t`.

## Type information fields

### `elementSize`

`elementSize` is required and must be greater than zero. Containers use it for
allocation, pointer arithmetic, and shallow byte copies.

The value must exactly match the stored type:

```c
.elementSize = sizeof(Student_t)
```

### `compare`

The comparator receives addresses of two elements and returns:

- A negative value when the first element comes before the second.
- Zero when they are equal.
- A positive value when the first element comes after the second.

```c
static int CompareStudent(const void *first, const void *second)
{
    const Student_t *left = first;
    const Student_t *right = second;
    return (left->id > right->id) - (left->id < right->id);
}
```

`adt_Min`, `adt_Max`, and `adt_Sort` require a comparator. `IndexOf` and
`Contains` use byte comparison when no comparator is configured.

`adt_MinBy`, `adt_MaxBy`, and `adt_SortBy` accept a per-call comparator without
changing the type information stored by the container.

### `print`

The printer receives one element address and should print only that element,
without a trailing newline:

```c
static void PrintStudent(const void *element)
{
    const Student_t *student = element;
    printf("{id: %d, name: \"%s\"}", student->id, student->name);
}
```

`adt_Print` supplies the container formatting, separators, and newline. It
returns `false` when no printer is configured.

### `destroy`

The destroy callback releases resources owned by an element. It must not free
the element storage passed to it because that storage belongs to the container.

Use `NULL` when the element owns no external resources:

```c
.destroy = NULL
```

See [ownership](ownership.md) for the complete destruction and transfer rules.

## Primitive type inference

`DA_INIT`, `DA_INIT_FROM`, `LL_INIT`, and `LL_INIT_FROM` infer callbacks for:

- `char`
- `int`
- `unsigned int`
- `long`
- `float`
- `double`

For example:

```c
DynamicArray_t numbers = {0};
DA_INIT(&numbers, int);
```

This configures `sizeof(int)`, `CompareInt`, `PrintInt`, and a `NULL`
destructor. Primitive value functions use the same registry for `_Generic`
dispatch.

Pointer comparators and printers are available as `ComparePointer` and
`PrintPointer`, but pointer ownership and meaning are application-specific, so
they must be configured explicitly.

## Custom type example

```c
typedef struct
{
    int id;
    char name[32];
} Student_t;

const ADT_TypeInfo_t studentType = {
    .elementSize = sizeof(Student_t),
    .compare = CompareStudent,
    .print = PrintStudent,
    .destroy = NULL
};

DynamicArray_t students = {0};

if (!da_Init(&students, studentType))
{
    return false;
}
```

Custom values are supplied by address to the unified operation macros:

```c
Student_t student = {.id = 1001, .name = "Ada"};
da_Append(&students, &student);
```
