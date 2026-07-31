#include "libadt/element/number_converters.h"

double ToNumberChar(const void *element)
{
    return (double)*(const unsigned char *)element;
}

double ToNumberInt(const void *element)
{
    return (double)*(const int *)element;
}

double ToNumberUnsignedInt(const void *element)
{
    return (double)*(const unsigned int *)element;
}

double ToNumberLong(const void *element)
{
    return (double)*(const long *)element;
}

double ToNumberFloat(const void *element)
{
    return (double)*(const float *)element;
}

double ToNumberDouble(const void *element)
{
    return *(const double *)element;
}
