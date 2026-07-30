#ifndef LIBADT_PRIMITIVE_DISPATCH_H
#define LIBADT_PRIMITIVE_DISPATCH_H

#include "abstract_data_type.h"

/**
 * @brief Applies a macro to every supported primitive.
 * @param Apply Macro accepting `(Suffix, Type)`.
 * @return The combined macro expansions.
 */
#define ADT_FOR_EACH_PRIMITIVE(Apply) \
    ADT_DETAIL_FOR_EACH_PRIMITIVE(ADT_DETAIL_APPLY_PRIMITIVE, Apply)

#define ADT_DETAIL_FOR_EACH_PRIMITIVE(Apply, Context) \
    Apply(Context, Char, char)                        \
    Apply(Context, Int, int)                          \
    Apply(Context, UnsignedInt, unsigned int)         \
    Apply(Context, Long, long)                        \
    Apply(Context, Float, float)                      \
    Apply(Context, Double, double)

#define ADT_DETAIL_APPLY_PRIMITIVE(Apply, Suffix, Type) \
    Apply(Suffix, Type)

#ifndef __cplusplus
#define ADT_DETAIL_JOIN_RAW(First, Second) First##Second
#define ADT_DETAIL_JOIN(First, Second) \
    ADT_DETAIL_JOIN_RAW(First, Second)

#define ADT_DETAIL_PRIMITIVE_ASSOCIATION(Prefix, Suffix, Type) \
    Type: ADT_DETAIL_JOIN(Prefix, Suffix),

#define ADT_DETAIL_DISPATCH_ELEMENT(Prefix, Value) \
    _Generic((Value),                              \
        ADT_DETAIL_FOR_EACH_PRIMITIVE(             \
            ADT_DETAIL_PRIMITIVE_ASSOCIATION,       \
            Prefix)                                \
        default: ADT_DETAIL_JOIN(Prefix, Ref))

/**
 * @brief Selects the built-in comparator for a C type.
 * @param Type Element type.
 * @return Its CompareFn_t, or NULL for an unsupported type.
 */
#define COMPARATOR(Type)                    \
    _Generic(((Type *)0),                   \
        char *: CompareChar,                \
        int *: CompareInt,                  \
        unsigned int *: CompareUnsignedInt, \
        long *: CompareLong,                \
        float *: CompareFloat,              \
        double *: CompareDouble,            \
        default: ((CompareFn_t)0))

/**
 * @brief Selects the built-in printer for a C type.
 * @param Type Element type.
 * @return Its PrintFn_t, or NULL for an unsupported type.
 */
#define PRINTER(Type)                   \
    _Generic(((Type *)0),                 \
        char *: PrintChar,                \
        int *: PrintInt,                  \
        unsigned int *: PrintUnsignedInt, \
        long *: PrintLong,                \
        float *: PrintFloat,              \
        double *: PrintDouble,            \
        default: ((PrintFn_t)0))
#endif

/**
 * @brief Checks whether a container stores elements of a given size.
 * @param adt Container to inspect, or NULL.
 * @param elementSize Size of the prospective element.
 * @return true when the element sizes match; otherwise false.
 */
static inline bool adt_AcceptsElementSize(const ADT_t *adt, size_t elementSize)
{
    const ADT_TypeInfo_t *type = adt_Type(adt);
    return type != NULL && type->elementSize == elementSize;
}

#endif
