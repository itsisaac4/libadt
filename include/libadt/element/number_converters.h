#ifndef LIBADT_NUMBER_CONVERTERS_H
#define LIBADT_NUMBER_CONVERTERS_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Converts an element to a numeric projection.
     * @param element Element to convert.
     * @return The projected numeric value.
     */
    typedef double (*ToNumberFn_t)(const void *element);

    /**
     * @brief Converts a char to its character code.
     * @param element Char element.
     * @return The character code as a double.
     */
    double ToNumberChar(const void *element);

    /**
     * @brief Converts an int to double.
     * @param element Int element.
     * @return The converted value.
     */
    double ToNumberInt(const void *element);

    /**
     * @brief Converts an unsigned int to double.
     * @param element Unsigned int element.
     * @return The converted value.
     */
    double ToNumberUnsignedInt(const void *element);

    /**
     * @brief Converts a long to double.
     * @param element Long element.
     * @return The converted value.
     */
    double ToNumberLong(const void *element);

    /**
     * @brief Converts a float to double.
     * @param element Float element.
     * @return The converted value.
     */
    double ToNumberFloat(const void *element);

    /**
     * @brief Reads a double value.
     * @param element Double element.
     * @return The stored value.
     */
    double ToNumberDouble(const void *element);

#ifdef __cplusplus
}
#endif

#endif
