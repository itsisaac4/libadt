#ifndef LIB_PRINTERS_H
#define LIB_PRINTERS_H

#ifdef __cplusplus
extern "C"
{
#endif

    /** Prints one element without adding a newline. */
    typedef void (*PrintFn)(
        const void *element);

    /** Prints a char value to standard output. */
    void PrintChar(const void *element);

    /** Prints an int value to standard output. */
    void PrintInt(const void *element);

    /** Prints an unsigned int value to standard output. */
    void PrintUnsignedInt(const void *element);

    /** Prints a long value to standard output. */
    void PrintLong(const void *element);

    /** Prints a float value to standard output. */
    void PrintFloat(const void *element);

    /** Prints a double value to standard output. */
    void PrintDouble(const void *element);

    /** Prints a stored pointer value to standard output. */
    void PrintPointer(const void *element);

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus

#define PRINTER(type)                     \
    _Generic(((type *)0),                 \
        char *: PrintChar,                \
        int *: PrintInt,                  \
        unsigned int *: PrintUnsignedInt, \
        long *: PrintLong,                \
        float *: PrintFloat,              \
        double *: PrintDouble,            \
        default: PrintPointer)

#endif

#endif
