#include "libadt/comparators.h"
#include <math.h>
#include <stdint.h>

int CompareChar(const void *first, const void *second)
{
    const char left = *(const char *)first;
    const char right = *(const char *)second;

    return (left > right) - (left < right);
}

int CompareInt(const void *first, const void *second)
{
    const int a = *(const int *)first;
    const int b = *(const int *)second;

    return (a > b) - (a < b);
}

int CompareUnsignedInt(const void *first, const void *second)
{
    const unsigned int a = *(const unsigned int *)first;
    const unsigned int b = *(const unsigned int *)second;

    return (a > b) - (a < b);
}

int CompareLong(const void *first, const void *second)
{
    const long a = *(const long *)first;
    const long b = *(const long *)second;

    return (a > b) - (a < b);
}

int CompareFloat(const void *first, const void *second)
{
    const float a = *(const float *)first;
    const float b = *(const float *)second;

    if (isnan(a))
    {
        return isnan(b) ? 0 : 1;
    }

    if (isnan(b))
    {
        return -1;
    }

    return (a > b) - (a < b);
}

int CompareDouble(const void *first, const void *second)
{
    const double a = *(const double *)first;
    const double b = *(const double *)second;

    if (isnan(a))
    {
        return isnan(b) ? 0 : 1;
    }

    if (isnan(b))
    {
        return -1;
    }

    return (a > b) - (a < b);
}

int ComparePointer(const void *first, const void *second)
{
    const void *a = *(void *const *)first;
    const void *b = *(void *const *)second;
    const uintptr_t aAddress = (uintptr_t)a;
    const uintptr_t bAddress = (uintptr_t)b;

    return (aAddress > bAddress) - (aAddress < bAddress);
}
