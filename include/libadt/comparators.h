#ifndef COMPARATORS_H
#define COMPARATORS_H

#ifdef __cplusplus
extern "C"
{
#endif

    /** Compares two elements and returns a negative value, zero, or a positive value. */
    typedef int (*CompareFn)(
        const void *first,
        const void *second);

    /** Compares two char values. */
    int CompareChar(const void *first, const void *second);

    /** Compares two int values. */
    int CompareInt(const void *first, const void *second);

    /** Compares two unsigned int values. */
    int CompareUnsignedInt(const void *first, const void *second);

    /** Compares two long values. */
    int CompareLong(const void *first, const void *second);

    /** Compares two float values. */
    int CompareFloat(const void *first, const void *second);

    /** Compares two double values. */
    int CompareDouble(const void *first, const void *second);

    /** Compares two stored pointer values by address. */
    int ComparePointer(const void *first, const void *second);

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus

#define COMPARATOR(type)                    \
    _Generic(((type *)0),                   \
        char *: CompareChar,                \
        int *: CompareInt,                  \
        unsigned int *: CompareUnsignedInt, \
        long *: CompareLong,                \
        float *: CompareFloat,              \
        double *: CompareDouble,            \
        default: ComparePointer)

#endif

#endif
