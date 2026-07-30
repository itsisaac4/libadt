#ifndef LIBADT_PRINTERS_H
#define LIBADT_PRINTERS_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Prints one element without a trailing newline.
     * @param element Element to print.
     * @return Nothing.
     */
    typedef void (*PrintFn_t)(const void *element);

    /**
     * @brief Prints a char.
     * @param element Char element.
     * @return Nothing.
     */
    void PrintChar(const void *element);

    /**
     * @brief Prints an int.
     * @param element Int element.
     * @return Nothing.
     */
    void PrintInt(const void *element);

    /**
     * @brief Prints an unsigned int.
     * @param element Unsigned int element.
     * @return Nothing.
     */
    void PrintUnsignedInt(const void *element);

    /**
     * @brief Prints a long.
     * @param element Long element.
     * @return Nothing.
     */
    void PrintLong(const void *element);

    /**
     * @brief Prints a float.
     * @param element Float element.
     * @return Nothing.
     */
    void PrintFloat(const void *element);

    /**
     * @brief Prints a double.
     * @param element Double element.
     * @return Nothing.
     */
    void PrintDouble(const void *element);

    /**
     * @brief Prints a stored pointer address.
     * @param element Pointer element.
     * @return Nothing.
     */
    void PrintPointer(const void *element);

#ifdef __cplusplus
}
#endif

#endif
