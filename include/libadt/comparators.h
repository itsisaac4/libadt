#ifndef LIBADT_COMPARATORS_H
#define LIBADT_COMPARATORS_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Compares two elements.
     * @param first First element.
     * @param second Second element.
     * @return A negative value, zero, or a positive value when first is less
     * than, equal to, or greater than second.
     */
    typedef int (*CompareFn_t)(const void *first, const void *second);

    /**
     * @brief Compares two char values.
     * @param first First char.
     * @param second Second char.
     * @return -1, 0, or 1 based on ordering.
     */
    int CompareChar(const void *first, const void *second);

    /**
     * @brief Compares two int values.
     * @param first First int.
     * @param second Second int.
     * @return -1, 0, or 1 based on ordering.
     */
    int CompareInt(const void *first, const void *second);

    /**
     * @brief Compares two unsigned int values.
     * @param first First unsigned int.
     * @param second Second unsigned int.
     * @return -1, 0, or 1 based on ordering.
     */
    int CompareUnsignedInt(const void *first, const void *second);

    /**
     * @brief Compares two long values.
     * @param first First long.
     * @param second Second long.
     * @return -1, 0, or 1 based on ordering.
     */
    int CompareLong(const void *first, const void *second);

    /**
     * @brief Compares two float values, sorting NaN last.
     * @param first First float.
     * @param second Second float.
     * @return -1, 0, or 1 based on ordering.
     */
    int CompareFloat(const void *first, const void *second);

    /**
     * @brief Compares two double values, sorting NaN last.
     * @param first First double.
     * @param second Second double.
     * @return -1, 0, or 1 based on ordering.
     */
    int CompareDouble(const void *first, const void *second);

    /**
     * @brief Compares two stored pointer addresses.
     * @param first First pointer element.
     * @param second Second pointer element.
     * @return -1, 0, or 1 based on address ordering.
     */
    int ComparePointer(const void *first, const void *second);

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus
/**
 * @brief Selects the built-in comparator for a C type.
 * @param type Element type.
 * @return Its CompareFn_t, or NULL for an unsupported type.
 */
#define COMPARATOR(type)                    \
    _Generic(((type *)0),                   \
        char *: CompareChar,                \
        int *: CompareInt,                  \
        unsigned int *: CompareUnsignedInt, \
        long *: CompareLong,                \
        float *: CompareFloat,              \
        double *: CompareDouble,            \
        default: ((CompareFn_t)0))
#endif

#endif
