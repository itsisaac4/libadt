# Numeric statistics

libadt calculates numeric statistics through the shared ADT traversal
interface. The same functions operate on dynamic arrays, linked lists, and
future traversable containers.

## Numeric projections

`ToNumberFn_t` converts one stored element to `double`:

```c
typedef double (*ToNumberFn_t)(const void *element);
```

Primitive initialization configures the matching built-in projection
automatically. Custom types select the numeric field that statistics should
use:

```c
static double StudentScoreToNumber(const void *element)
{
    const Student_t *student = element;
    return student->score;
}
```

Store the default projection in `ADT_ElementTypeInfo_t.toNumber`.

## Default and override operations

Default operations use the configured projection:

```c
double mean = 0.0;
double median = 0.0;
double mode = 0.0;

adt_Mean(&students, &mean);
adt_Median(&students, &median);
adt_Mode(&students, &mode);
```

The `By` variants accept a projection for one call:

```c
adt_MeanBy(&students, StudentAgeToNumber, &mean);
adt_MedianBy(&students, StudentAgeToNumber, &median);
adt_ModeBy(&students, StudentAgeToNumber, &mode);
```

This allows one custom element type to expose several numeric views without
changing its stored descriptor.

## Mean

`adt_Mean` traverses the container once and accumulates projected values in a
`long double`. The result is returned as `double`.

```text
mean = sum of projections / element count
```

Mean requires a nonempty container, a numeric projection, and output storage.

## Median

`adt_Median` copies numeric projections into a temporary buffer and sorts that
buffer without changing the container.

- An odd-sized collection returns its middle projected value.
- An even-sized collection averages its two middle projected values.

The even calculation divides each middle value before adding them, reducing
the chance of intermediate overflow.

## Mode

`adt_Mode` sorts projected values and finds the longest repeated group.

- A mode must occur at least twice.
- A collection with no repeated projection returns `false`.
- When several projections have the same highest frequency, the smallest is
  returned.
- Floating-point projections use exact equality through `CompareDouble`.

The output value is not modified when no mode exists.

## Failure behavior

Numeric statistics return `false` when:

- The container or output pointer is `NULL`.
- The container is empty.
- The selected numeric projection is `NULL`.
- Temporary buffer allocation or size validation fails.
- The container does not provide read-only traversal.

Mean does not allocate. Median and mode allocate a temporary `double` buffer
proportional to the element count.

## Floating-point behavior

All projections ultimately become `double`. Very large integer values may lose
precision during conversion. Mean accumulation uses `long double`, but the
public result is converted back to `double`.

`NaN` and infinity follow normal floating-point arithmetic. `CompareDouble`
sorts `NaN` after ordinary numbers for median and mode.

See [custom element types](custom_types.md) for complete callback and
descriptor definitions.
