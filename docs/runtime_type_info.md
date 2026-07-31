# Runtime type information

Container internals operate on type-erased elements through `void *`.
`ADT_ElementTypeInfo_t` is the runtime descriptor that preserves the element
size and behavior needed to interpret those bytes:

```c
typedef struct
{
    size_t elementSize;
    CompareFn_t compare;
    PrintFn_t print;
    ToNumberFn_t toNumber;
    DestroyFn_t destroy;
} ADT_ElementTypeInfo_t;
```

Every initialized container stores this descriptor in `ADT_Super_t`.

Think of the descriptor as five answers the erased type can no longer provide:

- How many bytes make one element?
- How are two elements ordered?
- How is one element printed?
- How can it be projected to a number?
- How are its owned resources released?

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
`Contains` can fall back to byte comparison, but semantic comparison is
usually the better choice for structures with padding or pointer members.

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

### `toNumber`

The numeric conversion returns the `double` used by the statistics functions:

```c
static double StudentScoreToNumber(const void *element)
{
    const Student_t *student = element;
    return student->score;
}
```

Use `NULL` when numeric statistics do not make sense for the type. A custom
type can project a specific field—such as a student's score—without changing
the stored structure.

### `destroy`

The destroy callback releases resources owned by an element. It must not free
the element storage passed to it because that storage belongs to the container.

Use `NULL` when the element owns no external resources:

```c
.destroy = NULL
```

See [ownership](ownership.md) for the complete destruction and transfer rules.

## Primitive type inference

`DA_INIT`, `DA_INIT_FROM`, `LL_INIT`, `LL_INIT_FROM`, `ST_INIT`,
`ST_INIT_FROM`, `QU_INIT`, and `QU_INIT_FROM` infer callbacks for:

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

This configures `sizeof(int)`, `CompareInt`, `PrintInt`, `ToNumberInt`, and a
`NULL` destructor. Primitive value functions use the same registry for
compile-time `_Generic` dispatch.

`ComparePointer` and `PrintPointer` are available for pointer values. The
library intentionally does not infer pointer ownership; an address alone does
not say whether it is borrowed, shared, or uniquely owned.

## Custom type example

```c
typedef struct
{
    int id;
    char name[32];
} Student_t;

const ADT_ElementTypeInfo_t studentType = ADT_ELEMENT_TYPE_INFO(
    Student_t,
    CompareStudent,
    PrintStudent,
    StudentScoreToNumber,
    NULL);

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

See [custom element types](custom_types.md) for complete callback declarations,
resource ownership, and descriptors shared across source files.
